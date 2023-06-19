#include "SEApp.hpp"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace SE {

	struct PushConstantData 
	{
		alignas(16) glm::vec3 color;
		alignas(16) glm::vec2 offset;
	};

#pragma region Lifecycle
	SEApp::SEApp()
	{
		load_meshes();
		create_pipeline_layout();
		recreate_swap_chain();
		create_command_buffers();
	}

	SEApp::~SEApp()
	{
		vkDestroyPipelineLayout(m_GraphicsDevice.device(), m_PipelineLayout, nullptr);
	}
#pragma endregion Lifecycle

void SEApp::run()
{
	while (!m_SEWindow.should_close()) 
	{
		glfwPollEvents();
		draw_frame();
	}

	vkDeviceWaitIdle(m_GraphicsDevice.device());
}

void SEApp::create_command_buffers()
{
	m_CommandBuffers.resize(m_SwapChain->get_image_count());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_GraphicsDevice.get_command_pool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_GraphicsDevice.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void SEApp::create_pipeline()
{
	assert(m_SwapChain != nullptr && "Cannot create pipeline before swap chain.");
	assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

	PipelineConfigInfo pipelineConfig{};
	SERenderPipeline::default_pipeline_config_info(pipelineConfig);
	pipelineConfig.renderPass = m_SwapChain->get_render_pass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;

	m_Pipeline = std::make_unique<SERenderPipeline>(m_GraphicsDevice, "shaders/basic_shader.vert.spv", "shaders/basic_shader.frag.spv", pipelineConfig);
}

void SEApp::create_pipeline_layout()
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(m_GraphicsDevice.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void SEApp::draw_frame()
{
	uint32_t imageIndex;
	VkResult result = m_SwapChain->acquire_next_image(&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) 
	{
		recreate_swap_chain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
	{
		throw std::runtime_error("Failed to acquire swap chain image.");
	}

	record_command_buffers(imageIndex);
	result = m_SwapChain->submit_command_buffers(&m_CommandBuffers[imageIndex], &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_SEWindow.was_window_resized()) 
	{
		m_SEWindow.reset_window_resized_flag();
		recreate_swap_chain();
		return;
	}

	if (result != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to present swap chain image.");
	}
}

void SEApp::load_meshes()
{
	std::vector<SEMesh::Vertex> verticies 
	{
		{ {0.0f, -0.5f},	{0.5f, 0.0f, 0.0f} },
		{ {0.5f, 0.5f},	{0.0f, 0.5f, 0.0f} },
		{ {-0.5f, 0.5f},	{0.0f, 0.0f, 0.5f} }
	};

	m_Mesh = std::make_unique<SEMesh>(m_GraphicsDevice, verticies);
}

void SEApp::recreate_swap_chain()
{
	VkExtent2D windowExtent = m_SEWindow.get_window_extent();

	while (windowExtent.width == 0 || windowExtent.height == 0) 
	{
		windowExtent = m_SEWindow.get_window_extent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(m_GraphicsDevice.device());

	if (m_SwapChain == nullptr) 
	{
		m_SwapChain = std::make_unique<SESwapChain>(m_GraphicsDevice, windowExtent);
	} else {
		m_SwapChain = std::make_unique<SESwapChain>(m_GraphicsDevice, windowExtent, std::move(m_SwapChain));
		if (m_SwapChain->get_image_count() != m_CommandBuffers.size()) 
		{
			free_command_buffers();
			create_command_buffers();
		}
	}
	create_pipeline();
}

void SEApp::record_command_buffers(int32_t imageIndex)
{
	static int32_t frameNumber = 0;
	frameNumber = (frameNumber + 1) % 1000;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_SwapChain->get_render_pass();
	renderPassInfo.framebuffer = m_SwapChain->get_frame_buffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChain->get_spawchain_extent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChain->get_spawchain_extent().width);
	viewport.height = static_cast<float>(m_SwapChain->get_spawchain_extent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{{0, 0}, m_SwapChain->get_spawchain_extent()};
	vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

	m_Pipeline->bind_command_buffer(m_CommandBuffers[imageIndex]);
	m_Mesh->bind_command_buffer(m_CommandBuffers[imageIndex]);

	for (int index = 0; index < 4; index++)
	{
		PushConstantData push{};
		push.offset = { -0.5f + frameNumber * 0.001, -0.4f + index * 0.25f };
		push.color = { 0.0f, 0.0f, 0.2f + 0.2f * index};

		vkCmdPushConstants(m_CommandBuffers[imageIndex], m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
		m_Mesh->draw(m_CommandBuffers[imageIndex]);
	}

	vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

	if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void SEApp::free_command_buffers()
{
	vkFreeCommandBuffers(m_GraphicsDevice.device(), m_GraphicsDevice.get_command_pool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	m_CommandBuffers.clear();
}

} // namespace SE
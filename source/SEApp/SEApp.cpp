#include "SEApp.hpp"
#include <stdexcept>
#include <array>

namespace SE {

#pragma region Lifecycle
	SEApp::SEApp()
	{
		create_pipeline_layout();
		create_pipeline();
		create_command_buffers();
	}

	SEApp::~SEApp()
	{
		vkDestroyPipelineLayout(m_GraphicsDevice.device(), m_PipelineLayout, nullptr);
	}
#pragma endregion Lifecycle

void SEApp::run()
{
	while (!m_SEWindow.should_close()) {
		glfwPollEvents();
		draw_frame();
	}

	vkDeviceWaitIdle(m_GraphicsDevice.device());
}

void SEApp::create_command_buffers()
{
	m_CommandBuffers.resize(m_SwapChain.get_image_count());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_GraphicsDevice.get_command_pool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_GraphicsDevice.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (int i = 0; i < m_CommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain.get_render_pass();
		renderPassInfo.framebuffer = m_SwapChain.get_frame_buffer(i);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain.get_spawchain_extent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_Pipeline->bind_command_buffer(m_CommandBuffers[i]);

		vkCmdDraw(m_CommandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(m_CommandBuffers[i]);

		if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void SEApp::create_pipeline()
{
	PipelineConfigInfo pipelineConfig = SERenderPipeline::default_pipeline_config_info(m_SwapChain.get_swapchain_width(), m_SwapChain.get_swapchain_height());
	pipelineConfig.renderPass = m_SwapChain.get_render_pass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;

	m_Pipeline = std::make_unique<SERenderPipeline>(m_GraphicsDevice, "shaders/basic_shader.vert.spv", "shaders/basic_shader.frag.spv", pipelineConfig);
}

void SEApp::create_pipeline_layout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(m_GraphicsDevice.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void SEApp::draw_frame()
{
	uint32_t imageIndex;
	VkResult result = m_SwapChain.acquire_next_image(&imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image.");
	}

	result = m_SwapChain.submit_command_buffers(&m_CommandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image.");
	}
}

} // namespace SE
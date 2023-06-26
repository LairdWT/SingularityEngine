#include "SEApp.hpp"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace SE {

	struct PushConstantData 
	{
		glm::mat2 transform{1.0f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

#pragma region Lifecycle
	SEApp::SEApp()
	{
		load_game_objects();
		create_pipeline_layout();
		create_pipeline();
	}

	SEApp::~SEApp()
	{
		vkDestroyPipelineLayout(m_GraphicsDevice.device(), m_PipelineLayout, nullptr);
	}
#pragma endregion Lifecycle

void SEApp::run()
{
	while (!m_Window.should_close()) 
	{
		glfwPollEvents();
		if (VkCommandBuffer commandBuffer = m_Renderer.begin_frame())
		{
			m_Renderer.begin_swap_chain_render_pass(commandBuffer);
			render_game_objects(commandBuffer);
			m_Renderer.end_swap_chain_render_pass(commandBuffer);
			m_Renderer.end_frame();
		}
	}

	vkDeviceWaitIdle(m_GraphicsDevice.device());
}

void SEApp::create_pipeline()
{
	assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

	PipelineConfigInfo pipelineConfig{};
	SERenderPipeline::default_pipeline_config_info(pipelineConfig);
	pipelineConfig.renderPass = m_Renderer.get_swap_chain_render_pass();
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

void SEApp::load_game_objects()
{
	std::vector<SEMesh::Vertex> verticies 
	{
		{ {0.0f, -0.5f},	{0.5f, 0.0f, 0.0f} },
		{ {0.5f, 0.5f},	{0.0f, 0.5f, 0.0f} },
		{ {-0.5f, 0.5f},	{0.0f, 0.0f, 0.5f} }
	};

	auto mesh = std::make_shared<SEMesh>(m_GraphicsDevice, verticies);

	auto TriangleGameObject = SEGameObject::create_game_object();
	TriangleGameObject.m_Mesh = mesh;
	TriangleGameObject.m_Color = { 0.1f, 0.12f, 0.15f };
	TriangleGameObject.m_Transform2d.Translation.x = 0.2f;
	TriangleGameObject.m_Transform2d.Scale = { 1.0f, 1.0f };
	TriangleGameObject.m_Transform2d.Rotation = 0.25f * glm::two_pi<float>();

	m_GameObjects.push_back(std::move(TriangleGameObject));
}

void SEApp::render_game_objects(VkCommandBuffer commandBuffer)
{
	m_Pipeline->bind_command_buffer(commandBuffer);

	for (auto& obj : m_GameObjects) 
	{
		obj.m_Transform2d.Rotation += 0.001f;

		PushConstantData push{};
		push.offset = obj.m_Transform2d.Translation;
		push.color = obj.m_Color;
		push.transform = obj.m_Transform2d.get_mat2();

		vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
		obj.m_Mesh->bind_command_buffer(commandBuffer);
		obj.m_Mesh->draw(commandBuffer);
	}
}

} // namespace SE
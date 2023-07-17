#include "SERendering/SERenderSystems/SERenderSystem.hpp"
#include "SECore/SEUtilities/SEMatrixUtilities.hpp"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace SE {

	struct PushConstantData
	{
		glm::mat4 transform{1.0f};
		glm::mat4 normalMatrix{1.0f};
	};

#pragma region Lifecycle
	SERenderSystem::SERenderSystem(SEGraphicsDevice& graphicsDevice, VkRenderPass renderPass) : m_GraphicsDevice(graphicsDevice)
	{
		create_pipeline_layout();
		create_pipeline(renderPass);
	}

	SERenderSystem::~SERenderSystem()
	{
		vkDestroyPipelineLayout(m_GraphicsDevice.device(), m_PipelineLayout, nullptr);
	}
#pragma endregion Lifecycle

	void SERenderSystem::create_pipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		SERenderPipeline::default_pipeline_config_info(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_Pipeline = std::make_unique<SERenderPipeline>(m_GraphicsDevice, "shaders/basic_shader.vert.spv", "shaders/basic_shader.frag.spv", pipelineConfig);
	}

	void SERenderSystem::create_pipeline_layout()
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

	void SERenderSystem::render_game_objects(VkCommandBuffer commandBuffer, std::vector<SEGameObject>& gameObjects, const SECamera& camera)
	{
		m_Pipeline->bind_command_buffer(commandBuffer);

		auto projectionView = camera.get_projection_matrix() * camera.get_view_matrix();

		for (auto& obj : gameObjects)
		{
			PushConstantData push{};
			auto modelMatrix = get_transform_matrix(obj.m_TransformComponent.Translation, obj.m_TransformComponent.Rotation, obj.m_TransformComponent.Scale);
			push.transform = projectionView * modelMatrix;
			push.normalMatrix = get_normal_matrix(obj.m_TransformComponent.Translation, obj.m_TransformComponent.Rotation, obj.m_TransformComponent.Scale);

			vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
			obj.m_Mesh->bind_command_buffer(commandBuffer);
			obj.m_Mesh->draw(commandBuffer);
		}
	}

} // namespace SE
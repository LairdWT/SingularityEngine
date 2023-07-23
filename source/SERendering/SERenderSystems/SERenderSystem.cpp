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
		glm::mat4 meshMatrix{1.0f};
		glm::mat4 normalMatrix{1.0f};
	};

#pragma region Lifecycle
	SERenderSystem::SERenderSystem(SEGraphicsDevice& graphicsDevice, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout) : m_GraphicsDevice(graphicsDevice)
	{
		create_pipeline_layout(globalDescriptorSetLayout);
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

	void SERenderSystem::create_pipeline_layout(VkDescriptorSetLayout globalDescriptorSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalDescriptorSetLayout};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_GraphicsDevice.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void SERenderSystem::render_game_objects(FFrameInfo& frameInfo, std::vector<SEGameObject>& gameObjects)
	{
		m_Pipeline->bind_command_buffer(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

		for (SEGameObject& gameObject : gameObjects)
		{
			PushConstantData push{};
			push.meshMatrix = get_transform_matrix(gameObject.m_TransformComponent.Translation, gameObject.m_TransformComponent.Rotation, gameObject.m_TransformComponent.Scale);
			push.normalMatrix = get_normal_matrix(gameObject.m_TransformComponent.Translation, gameObject.m_TransformComponent.Rotation, gameObject.m_TransformComponent.Scale);

			vkCmdPushConstants(frameInfo.commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
			gameObject.m_Mesh->bind_command_buffer(frameInfo.commandBuffer);
			gameObject.m_Mesh->draw(frameInfo.commandBuffer);
		}
	}

} // namespace SE
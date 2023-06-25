#pragma once

#include "SERendering/SEGraphicsDevice/SEGraphicsDEvice.hpp"
#include <string>
#include <vector>

namespace SE {

struct PipelineConfigInfo {
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class SERenderPipeline {

public:
#pragma region Lifecycle
	SERenderPipeline(SEGraphicsDevice& graphicsDevice, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
	~SERenderPipeline();
	SERenderPipeline(const SERenderPipeline&) = delete;
	SERenderPipeline& operator=(const SERenderPipeline&) = delete;
#pragma endregion Lifecycle

	static void default_pipeline_config_info(PipelineConfigInfo& configInfo);
	void bind_command_buffer(VkCommandBuffer commandBuffer);

#ifdef NDEBUG
	const bool ENABLE_SPIRV_VALIDATION = true;
#else
	const bool ENABLE_SPIRV_VALIDATION = true;
#endif

private:
	static std::vector<char> read_file(const std::string& filepath);

	void create_graphics_pipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);

	void create_shader_module(const std::vector<char>& shaderCode, VkShaderModule* shaderModule);

	bool validate_spirv_code(const std::vector<char>& shaderCode);

	SEGraphicsDevice& m_GraphicsDevice;
	VkPipeline m_GraphicsPipeline;
	VkShaderModule m_VertShaderModule;
	VkShaderModule m_FragShaderModule;
};

}	// end SE namespace
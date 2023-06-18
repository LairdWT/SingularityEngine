#include "SERenderPipeline.hpp"

#include "SECore/SEEntities/SEMesh.hpp"

#include <spirv-tools/libspirv.h>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cassert>

namespace SE {

#pragma region Lifecycle
SERenderPipeline::SERenderPipeline(SEGraphicsDevice& graphicsDevice, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) : m_GraphicsDevice{ graphicsDevice } {
create_graphics_pipeline(vertFilepath, fragFilepath, configInfo);
}

SERenderPipeline::~SERenderPipeline() {
		vkDestroyShaderModule(m_GraphicsDevice.device(), m_VertShaderModule, nullptr);
		vkDestroyShaderModule(m_GraphicsDevice.device(), m_FragShaderModule, nullptr);
		vkDestroyPipeline(m_GraphicsDevice.device(), m_GraphicsPipeline, nullptr);
}
#pragma endregion Lifecycle

void SERenderPipeline::default_pipeline_config_info(PipelineConfigInfo& configInfo)
{
	configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configInfo.viewportInfo.viewportCount = 1;
	configInfo.viewportInfo.pViewports = nullptr;
	configInfo.viewportInfo.scissorCount = 1;
	configInfo.viewportInfo.pScissors = nullptr;

	configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
	configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	configInfo.rasterizationInfo.lineWidth = 1.0f;
	configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
	configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
	configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
	configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

	configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
	configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
	configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
	configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
	configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

	configInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
	configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

	configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
	configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
	configInfo.colorBlendInfo.attachmentCount = 1;
	configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
	configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

	configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
	configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
	configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.front = {};  // Optional
	configInfo.depthStencilInfo.back = {};   // Optional

	configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
	configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
	configInfo.dynamicStateInfo.flags = 0;
}


void SERenderPipeline::bind_command_buffer(VkCommandBuffer commandBuffer)
{
	/* Bind the graphics pipeline
	*  the bind point options include compute, graphics, ray tracing, and ray tracing NV
	*/
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
}

std::vector<char> SERenderPipeline::read_file(const std::string& filepath)
{
	std::ifstream fileIn(filepath, std::ios::ate | std::ios::binary);

	if (!fileIn.is_open()) {
		throw std::runtime_error("Failed to open file: " + filepath);
	}

	size_t fileSize = static_cast<size_t>(fileIn.tellg());
	std::vector<char> buffer(fileSize);

	fileIn.seekg(0);
	fileIn.read(buffer.data(), fileSize);
	fileIn.close();

	return buffer;
}

void SERenderPipeline::create_graphics_pipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo)
{
	assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline, no pipelineLayout in configInfo");
	assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline, no renderPass in configInfo");

	std::vector<char> vertCode = read_file(vertFilepath);
	std::vector<char> fragCode = read_file(fragFilepath);

	create_shader_module(vertCode, &m_VertShaderModule);
	create_shader_module(fragCode, &m_FragShaderModule);


	VkPipelineShaderStageCreateInfo shaderStages[2];
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = m_VertShaderModule;
	shaderStages[0].pName = "main";
	shaderStages[0].flags = 0;
	shaderStages[0].pNext = nullptr;
	shaderStages[0].pSpecializationInfo = nullptr;

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = m_FragShaderModule;
	shaderStages[1].pName = "main";
	shaderStages[1].flags = 0;
	shaderStages[1].pNext = nullptr;
	shaderStages[1].pSpecializationInfo = nullptr;


	std::vector<VkVertexInputBindingDescription> bindingDescriptions = SEMesh::Vertex::get_binding_descriptions();
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions =  SEMesh::Vertex::get_attribute_descriptions();
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
	pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
	pipelineInfo.pViewportState = &configInfo.viewportInfo;
	pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
	pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
	pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
	pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
	pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

	pipelineInfo.layout = configInfo.pipelineLayout;
	pipelineInfo.renderPass = configInfo.renderPass;
	pipelineInfo.subpass = configInfo.subpass;

	pipelineInfo.basePipelineIndex = -1;  // Optional
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional

	if (vkCreateGraphicsPipelines(m_GraphicsDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline.");
	}
}

void SERenderPipeline::create_shader_module(const std::vector<char>& shaderCode, VkShaderModule* shaderModule)
{
	try {
		if (!validate_spirv_code(shaderCode))
		{
			throw std::runtime_error("Shader Code not valid format.");
		}
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Validation of SPIRV code failed with error: " << e.what() << "\n";
		return;
	}

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	if (vkCreateShaderModule(m_GraphicsDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module");
	}
}


bool SERenderPipeline::validate_spirv_code(const std::vector<char>& shaderCodeChar)
{
	if (!ENABLE_SPIRV_VALIDATION) {
		std::cout << "SPIRV validation disabled. Skipping...\n";
		return true;
	}

	if (shaderCodeChar.size() % sizeof(uint32_t) != 0) {
		throw std::runtime_error("Shader code size is not a multiple of uint32_t");
	}

	std::vector<uint32_t> shaderCode(reinterpret_cast<const uint32_t*>(shaderCodeChar.data()),
		reinterpret_cast<const uint32_t*>(shaderCodeChar.data() + shaderCodeChar.size()));

	if (shaderCode.empty()) {
		throw std::runtime_error("No shader code provided for validation");
	}

	spv_context context = spvContextCreate(SPV_ENV_VULKAN_1_0);
	if (!context) {
		throw std::runtime_error("Failed to create SPIRV validation context");
	}

	spv_diagnostic diagnostic = nullptr;
	spv_const_binary_t binary = { shaderCode.data(), shaderCode.size() };
	spv_result_t result = spvValidate(context, &binary, &diagnostic);

	if (result != SPV_SUCCESS) {
		if (diagnostic) {
			std::ostringstream error_msg;
			error_msg << "SPIRV validation failed with the following diagnostic: \n";
			error_msg << diagnostic->error;
			spvDiagnosticDestroy(diagnostic);
			throw std::runtime_error(error_msg.str());
		}
		else {
			throw std::runtime_error("SPIRV validation failed");
		}
	}
	else {
		std::cout << "SPIRV validation succeeded. Shader module size: "
			<< shaderCodeChar.size() << " bytes. Number of SPIR-V instructions: "
			<< shaderCode.size() << ".\n";
	}

	if (diagnostic) {
		spvDiagnosticDestroy(diagnostic);
	}
	spvContextDestroy(context);

	return result == SPV_SUCCESS;
}

}	// end SE namespace
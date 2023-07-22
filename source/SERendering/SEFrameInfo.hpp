#pragma once

#include "SECore/SEEntities/SECamera.hpp"

#include <vulkan/vulkan.h>

namespace SE
{

struct FFrameInfo 
{
	uint32_t frameIndex;
	float deltaTime;
	VkCommandBuffer commandBuffer;
	SECamera& camera;
};

}
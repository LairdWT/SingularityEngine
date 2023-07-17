#include "SERendering/SERenderer.hpp"
#include <stdexcept>
#include <array>


namespace SE {

	SERenderer::SERenderer(SEWindow& window, SEGraphicsDevice& graphicsDevice) : m_SEWindow{ window }, m_GraphicsDevice{ graphicsDevice }
	{
		recreate_swap_chain();
		create_command_buffers();
	}

	SERenderer::~SERenderer()
	{
		free_command_buffers();
	}

	VkCommandBuffer SERenderer::begin_frame()
	{
		assert(!m_bIsFrameStarted && "Can't call begin_frame while already in progress");

		VkResult result = m_SwapChain->acquire_next_image(&m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreate_swap_chain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image.");
		}

		m_bIsFrameStarted = true;

		VkCommandBuffer commandBuffer = get_current_command_buffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void SERenderer::end_frame()
	{
		assert(m_bIsFrameStarted && "Can't call end_frame while frame is not in progress");

		VkCommandBuffer commandBuffer = get_current_command_buffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

		VkResult result = m_SwapChain->submit_command_buffers(&commandBuffer, &m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_SEWindow.was_window_resized())
		{
			m_SEWindow.reset_window_resized_flag();
			recreate_swap_chain();
		} else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image.");
		}

		m_bIsFrameStarted = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SESwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void SERenderer::begin_swap_chain_render_pass(VkCommandBuffer commandBuffer)
	{
		assert(m_bIsFrameStarted && "Can't call begin_swap_chain_render_pass if frame is not in progress");
		assert(commandBuffer == get_current_command_buffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->get_render_pass();
		renderPassInfo.framebuffer = m_SwapChain->get_frame_buffer(m_CurrentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->get_spawchain_extent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->get_spawchain_extent().width);
		viewport.height = static_cast<float>(m_SwapChain->get_spawchain_extent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_SwapChain->get_spawchain_extent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void SERenderer::end_swap_chain_render_pass(VkCommandBuffer commandBuffer)
	{
		assert(m_bIsFrameStarted && "Can't call end_swap_chain_render_pass if frame is not in progress");
		assert(commandBuffer == get_current_command_buffer() && "Can't begin render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

	VkCommandBuffer SERenderer::get_current_command_buffer() const
	{
		assert(m_bIsFrameStarted && "Can't get command buffer when frame is not in progress");
		return m_CommandBuffers[m_CurrentFrameIndex];
	}

	uint32_t SERenderer::get_current_frame_index() const
	{
		assert(m_bIsFrameStarted && "Can't get frame index when frame is not in progress");
		return m_CurrentFrameIndex;
	}

	void SERenderer::create_command_buffers()
	{
		m_CommandBuffers.resize(SESwapChain::MAX_FRAMES_IN_FLIGHT);

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

	void SERenderer::free_command_buffers()
	{
		if (!m_CommandBuffers.empty()) 
		{
			// Wait for the device to finish all operations
			vkDeviceWaitIdle(m_GraphicsDevice.device());

			vkFreeCommandBuffers(m_GraphicsDevice.device(), m_GraphicsDevice.get_command_pool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
			m_CommandBuffers.clear();
		}
	}

	void SERenderer::recreate_swap_chain()
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
			std::shared_ptr<SESwapChain> oldSwapChain = std::move(m_SwapChain);
			m_SwapChain = std::make_unique<SESwapChain>(m_GraphicsDevice, windowExtent, oldSwapChain);

			if (!oldSwapChain->compare_swapchain_formats(*m_SwapChain.get()))
			{
				throw std::runtime_error("Swap chain image format has changed!");
			}
		}
	}

} // end SE namespace
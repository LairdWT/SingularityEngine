#pragma once

#include "SERendering/SEWindow/SEWindow.hpp"
#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"
#include "SERendering/SERenderPipeline/SESwapChain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace SE {

	class SERenderer {

	public:

#pragma region Lifecycle
		SERenderer(SEWindow& window, SEGraphicsDevice& graphicsDevice);
		~SERenderer();

		SERenderer(const SERenderer&) = delete;
		SERenderer& operator=(const SERenderer&) = delete;
#pragma endregion Lifecycle

	public:

		VkCommandBuffer begin_frame();
		void end_frame();
		void begin_swap_chain_render_pass(VkCommandBuffer commandBuffer);
		void end_swap_chain_render_pass(VkCommandBuffer commandBuffer);

		bool is_frame_in_progress() const { return m_bIsFrameStarted; }
		VkCommandBuffer get_current_command_buffer() const 
		{ 
			assert(m_bIsFrameStarted && "Can't get command buffer when frame is not in progress");
			return m_CommandBuffers[m_CurrentImageIndex]; 
		}
		VkRenderPass get_swap_chain_render_pass() const { return m_SwapChain->get_render_pass(); }

	private:

		void create_command_buffers();
		void free_command_buffers();
		void recreate_swap_chain();



		SEWindow& m_SEWindow;
		SEGraphicsDevice& m_GraphicsDevice;

		std::unique_ptr<SESwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex = 0;
		bool m_bIsFrameStarted = false;
	};

} // end SE namespace
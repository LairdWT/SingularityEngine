#pragma once

#include "SEWindow/SEWindow.hpp"
#include "SERenderPipeline/SERenderPipeline.hpp"
#include "SEGraphicsDevice/SEGraphicsDevice.hpp"
#include "SERenderPipeline/SESwapChain.hpp"
#include <memory>
#include <vector>

namespace SE {

class SEApp {

public:
	
#pragma region Lifecycle
	SEApp();
	~SEApp();
	SEApp(const SEApp&) = delete;
	SEApp& operator=(const SEApp&) = delete;
#pragma endregion Lifecycle

	void run();

	static constexpr uint32_t m_WindowWidth = 1024;
	static constexpr uint32_t m_WindowHeight = 1024;

private:
	void create_command_buffers();
	void create_pipeline();
	void create_pipeline_layout();
	void draw_frame();

	SEWindow m_SEWindow{m_WindowWidth, m_WindowHeight, "Singularity Engine"};
	SEGraphicsDevice m_GraphicsDevice{ m_SEWindow };
	SESwapChain m_SwapChain{ m_GraphicsDevice, m_SEWindow.get_window_extent() };
	std::unique_ptr<SERenderPipeline> m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
	std::vector<VkCommandBuffer> m_CommandBuffers;
};

} // end SE namespace
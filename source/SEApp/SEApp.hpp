#pragma once

#include "SERendering/SEWindow/SEWindow.hpp"
#include "SERendering/SERenderPipeline/SERenderPipeline.hpp"
#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"
#include "SERendering/SERenderPipeline/SESwapChain.hpp"
#include "SECore/SEEntities/SEGameObject.hpp"

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

	static constexpr uint32_t m_WindowWidth = 1920;
	static constexpr uint32_t m_WindowHeight = 1080;

private:
	void create_command_buffers();
	void record_command_buffers(int32_t imageIndex);
	void free_command_buffers();
	void create_pipeline();
	void create_pipeline_layout();
	void draw_frame();
	void load_game_objects();
	void render_game_objects(VkCommandBuffer commandBuffer);
	void recreate_swap_chain();


	SEWindow m_SEWindow{m_WindowWidth, m_WindowHeight, "Singularity Engine"};
	SEGraphicsDevice m_GraphicsDevice{ m_SEWindow };
	VkPipelineLayout m_PipelineLayout;

	std::unique_ptr<SESwapChain> m_SwapChain;
	std::unique_ptr<SERenderPipeline> m_Pipeline;
	std::vector<VkCommandBuffer> m_CommandBuffers;
	std::vector<SEGameObject> m_GameObjects;
};

} // end SE namespace
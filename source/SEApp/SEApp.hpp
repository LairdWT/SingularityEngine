#pragma once

#include "SERendering/SEWindow/SEWindow.hpp"
#include "SERendering/SERenderPipeline/SERenderPipeline.hpp"
#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"
#include "SERendering/SERenderer.hpp"
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

	void create_pipeline();
	void create_pipeline_layout();
	void load_game_objects();
	void render_game_objects(VkCommandBuffer commandBuffer);




	SEWindow m_Window{m_WindowWidth, m_WindowHeight, "Singularity Engine"};
	SEGraphicsDevice m_GraphicsDevice{ m_Window };
	VkPipelineLayout m_PipelineLayout;
	SERenderer m_Renderer{m_Window, m_GraphicsDevice};

	std::unique_ptr<SERenderPipeline> m_Pipeline;
	std::vector<SEGameObject> m_GameObjects;
};

} // end SE namespace
#pragma once

#include "SERendering/SEWindow/SEWindow.hpp"
#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"
#include "SERendering/SERenderer.hpp"
#include "SECore/SEEntities/SEGameObject.hpp"
#include "SECore/SESystems/SETimeManager.hpp"

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
	void on_tick();

	static constexpr uint32_t m_WindowWidth = 1920;
	static constexpr uint32_t m_WindowHeight = 1080;
	const std::string m_WindowName = "Singularity Engine";
	const float m_FixedTimeStep = 1.0f / 240.0f; // 240 fps

private:

	void load_game_objects();

	SEWindow m_Window{m_WindowWidth, m_WindowHeight, m_WindowName};
	SEGraphicsDevice m_GraphicsDevice{ m_Window };
	SERenderer m_Renderer{m_Window, m_GraphicsDevice};

	std::vector<SEGameObject> m_GameObjects;
	std::unique_ptr<SETimeManager> m_TimeManager;
	SETickDelegate m_TickDelegate;
	std::atomic<uint64_t> m_TickCounter{0};
};

} // end SE namespace
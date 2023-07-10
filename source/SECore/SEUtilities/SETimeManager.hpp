#pragma once

#include <chrono>
#include <functional>

namespace SE {


class SETimeManager {

public:
#pragma region Lifecycle
	SETimeManager();
	SETimeManager(float fixedTimeStep);
	~SETimeManager();
	SETimeManager(const SETimeManager&) = delete;
	SETimeManager& operator=(const SETimeManager&) = delete;
#pragma endregion Lifecycle

	// Create a delegate to be called every tick
	void add_tick_delegate(std::function<void()> tickDelegate);
	void initialize();
	void update();

	// Getters
	float get_delta_time() const { return m_DeltaTime; }
	float get_fixed_time_step() const { return m_FixedTimeStep; }
	std::chrono::time_point<std::chrono::steady_clock> get_current_frame_time() const { return m_CurrentFrameTime; }
	std::chrono::time_point<std::chrono::steady_clock> get_last_frame_time() const { return m_LastFrameTime; }
	uint16_t get_fps() const { return m_FPS; }

private:
	void print_fps();



	// For rendering
	std::chrono::time_point<std::chrono::steady_clock> m_LastFrameTime;
	std::chrono::time_point<std::chrono::steady_clock> m_CurrentFrameTime;
	float m_DeltaTime{0.0f};

	// For fixed time step
	float m_AccumulatedTime;
	float m_FixedTimeStep;
	std::vector<std::function<void()>> m_TickDelegates;

	uint16_t m_FrameCount{0};
	uint16_t m_FPS{0};
	std::chrono::time_point<std::chrono::steady_clock> m_LastFPSCalculation;

};
} // end SE namespace
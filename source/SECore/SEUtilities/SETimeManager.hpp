#pragma once

#include <chrono>

namespace SE {

class SETimeManager {

public:
#pragma region Lifecycle
	SETimeManager();
	~SETimeManager();
	SETimeManager(const SETimeManager&) = delete;
	SETimeManager& operator=(const SETimeManager&) = delete;
#pragma endregion Lifecycle

	void initialize();
	void update();

	// Getters
	float get_delta_time() const { return m_DeltaTime; }
	std::chrono::time_point<std::chrono::steady_clock> get_current_frame_time() const { return m_CurrentFrameTime; }
	std::chrono::time_point<std::chrono::steady_clock> get_last_frame_time() const { return m_LastFrameTime; }
	uint16_t get_fps() const { return m_FPS; }

private:
	void update_frame_times();
	void update_delta_time();
	void update_fps();
	void print_fps();

	std::chrono::time_point<std::chrono::steady_clock> m_LastFrameTime;
	std::chrono::time_point<std::chrono::steady_clock> m_CurrentFrameTime;
	float m_DeltaTime{0.0f};

	uint16_t m_FrameCount{0};
	uint16_t m_FPS{0};
	std::chrono::time_point<std::chrono::steady_clock> m_LastFPSCalculation;

};
} // end SE namespace
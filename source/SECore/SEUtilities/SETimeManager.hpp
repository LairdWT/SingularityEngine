#pragma once

#include <chrono>
#include <functional>
#include <map>

namespace SE {

class SETimeManager {
public:

#pragma region Lifecycle
	// Default constructor initializes the Time Manager with a fixed time step of approximately 60 FPS
	// custom fixed time step can be passed in as a parameter to the second constructor
	SETimeManager();
	SETimeManager(float fixedTimeStep);

	~SETimeManager();

	// Delete copy constructor and assignment operator
	SETimeManager(const SETimeManager&) = delete;
	SETimeManager& operator=(const SETimeManager&) = delete;
#pragma endregion Lifecycle


	// Initializes the time manager, setting the current and last frame times to the current time
	void initialize();
	// Updates the time manager, calculating the delta time and executing tick delegates
	void update();
	// Adds a delegate function that will be called on each tick of the game loop
	std::size_t  add_tick_delegate(std::function<void()> tickDelegate);
	// Removes a delegate function
	void remove_tick_delegate(std::size_t key);
	// Prints the current frames per second to the console
	void print_fps();


	// Getters
	inline float get_delta_time() const { return m_DeltaTime; }  // Returns the time elapsed since the last frame
	inline float get_fixed_time_step() const { return m_FixedTimeStep; }
	inline std::chrono::time_point<std::chrono::steady_clock> get_current_frame_time() const { return m_CurrentFrameTime; }
	inline std::chrono::time_point<std::chrono::steady_clock> get_last_frame_time() const { return m_LastFrameTime; }
	inline uint16_t get_fps() const { return m_FPS; }


private:

	std::chrono::time_point<std::chrono::steady_clock> m_LastFrameTime;
	std::chrono::time_point<std::chrono::steady_clock> m_CurrentFrameTime;
	float m_DeltaTime{ 0.0f };
	float m_AccumulatedTime;
	float m_FixedTimeStep;
	uint16_t m_FrameCount{ 0 };
	uint16_t m_FPS{ 0 };
	std::chrono::time_point<std::chrono::steady_clock> m_LastFPSCalculation;

	// Tick delegates data
	std::size_t m_NextDelegateKey = 0;  // The key that will be assigned to the next delegate added
	std::vector<std::function<void()>> m_TickDelegates;  // The vector of delegates
	std::map<std::size_t, std::size_t> m_TickDelegateKeys;  // Maps keys to indices in the vector
	std::map<std::size_t, std::size_t> m_TickDelegateIndices;
};
} // end SE namespace
#include "SETimeManager.hpp"

#include <iostream>
#include <iomanip>

namespace SE {

SETimeManager::SETimeManager() : m_AccumulatedTime(0.0f), m_FixedTimeStep(0.0166666666f)
{
	initialize();
}

SETimeManager::SETimeManager(float fixedTimeStep) : m_AccumulatedTime(0.0f), m_FixedTimeStep(fixedTimeStep)
{
	initialize();
}

SETimeManager::~SETimeManager()
{

}

void SETimeManager::initialize()
{
	m_CurrentFrameTime = std::chrono::steady_clock::now();
	m_LastFrameTime = m_CurrentFrameTime;
}

void SETimeManager::update()
{
	m_LastFrameTime = m_CurrentFrameTime;
	m_CurrentFrameTime = std::chrono::steady_clock::now();

	// Delta Time
	m_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(m_CurrentFrameTime - m_LastFrameTime).count();
	m_AccumulatedTime += m_DeltaTime;

	// FPS and FrameCount
	++m_FrameCount;
	float elapsedSeconds = std::chrono::duration<float, std::chrono::seconds::period>(m_CurrentFrameTime - m_LastFPSCalculation).count();
	if (elapsedSeconds > 1.0f)
	{
		m_FPS = static_cast<uint16_t>(m_FrameCount / elapsedSeconds);
		m_LastFPSCalculation = m_CurrentFrameTime;
		m_FrameCount = 0;
	}

	// Execute the tick delegates for each fixed time step - while used to catch up if the game is running slow
	size_t numDelegates = m_TickDelegates.size();
	while (m_AccumulatedTime >= m_FixedTimeStep)
	{
		for (size_t i = 0; i < numDelegates; ++i)
		{
			m_TickDelegates[i]();
		}
		m_AccumulatedTime -= m_FixedTimeStep;
	}
}

void SETimeManager::print_fps()
{
	std::cout << "\rCurrent FPS: " << std::setw(3) << m_FPS << "   " << std::endl;
}

void SETimeManager::add_tick_delegate(std::function<void()> tickDelegate)
{
	m_TickDelegates.push_back(tickDelegate);
}

} // end SE namespace
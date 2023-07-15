#include "SETimeManager.hpp"

#include <iostream>
#include <iomanip>

namespace SE {

SETimeManager::SETimeManager() : m_AccumulatedTime(0.0f), m_FixedTimeStep(0.0166666666f)
{
	initialize();
	m_TickDelegates.reserve(16);

}

SETimeManager::SETimeManager(float fixedTimeStep) : m_AccumulatedTime(0.0f), m_FixedTimeStep(fixedTimeStep)
{
	initialize();
	m_TickDelegates.reserve(16);
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
	while (m_AccumulatedTime >= m_FixedTimeStep)
	{
		for (auto& tickDelegate : m_TickDelegates)
		{
			tickDelegate();
		}
		m_AccumulatedTime -= m_FixedTimeStep;
	}
}

void SETimeManager::print_fps()
{
	std::cout << "\rCurrent FPS: " << std::setw(3) << m_FPS << "   \n";
}

std::size_t SETimeManager::add_tick_delegate(std::function<void()> tickDelegate)
{
	std::size_t key = m_NextDelegateKey++;
	m_TickDelegates.push_back(std::move(tickDelegate));
	std::size_t index = m_TickDelegates.size() - 1;
	m_TickDelegateKeys[key] = index;
	m_TickDelegateIndices[index] = key;
	return key;
}

void SETimeManager::remove_tick_delegate(std::size_t key)
{
	auto keyIter = m_TickDelegateKeys.find(key);
	if (keyIter == m_TickDelegateKeys.end())
	{
		return;
	}

	std::size_t index = keyIter->second;
	if (index != m_TickDelegates.size() - 1) {
		// Swap the delegate to remove with the last delegate
		std::swap(m_TickDelegates[index], m_TickDelegates.back());
		// Update the key and index of the moved delegate
		std::size_t movedKey = m_TickDelegateIndices[m_TickDelegates.size() - 1];
		m_TickDelegateKeys[movedKey] = index;
		m_TickDelegateIndices[index] = movedKey;
	}
	// Remove the last delegate
	m_TickDelegates.pop_back();
	m_TickDelegateKeys.erase(key);
	m_TickDelegateIndices.erase(m_TickDelegates.size());  // The size has been reduced by pop_back

	std::cout << "\nTick Delegate Removed with key: " << key << "\n";
}

} // end SE namespace
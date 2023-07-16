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

void SETimeManager::print_fps()
{
	float elapsedSeconds = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::steady_clock::now() - m_LastFPSCalculation).count();
	if (elapsedSeconds > 1.0f)
	{
		m_FPS = static_cast<uint16_t>(m_FrameCount / elapsedSeconds);
		m_LastFPSCalculation = std::chrono::steady_clock::now();
		m_FrameCount = 0;
		std::cout << "\rCurrent FPS: " << std::setw(3) << m_FPS << "   " << std::flush;
	}
}

void SETimeManager::update()
{
	m_LastFrameTime = m_CurrentFrameTime;
	m_CurrentFrameTime = std::chrono::steady_clock::now();

	// Delta Time
	m_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(m_CurrentFrameTime - m_LastFrameTime).count();
	m_AccumulatedTime += m_DeltaTime;

	// FrameCount
	++m_FrameCount;

	// Execute Tick Delegates
	while (m_AccumulatedTime >= m_FixedTimeStep) {
		for (auto& tickDelegate : m_TickDelegates) {
			tickDelegate();
		}
		m_AccumulatedTime -= m_FixedTimeStep;
	}

	print_fps();
}

SETickDelegate SETimeManager::add_tick_delegate(std::function<void()> tickDelegate)
{
	std::size_t key = m_NextDelegateKey++;
	m_TickDelegates.push_back(std::move(tickDelegate));
	m_TickDelegateKeys[key] = m_TickDelegates.size() - 1;
	return { key, ETickDelegateStatus::Valid };
}

void SETimeManager::remove_tick_delegate(SETickDelegate& tickDelegate)
{
	auto it = m_TickDelegateKeys.find(tickDelegate.Key);
	if (it != m_TickDelegateKeys.end()) {
		// Swap with the last delegate to maintain a contiguous array of active delegates
		if (it->second != m_TickDelegates.size() - 1) {
			std::swap(m_TickDelegates[it->second], m_TickDelegates.back());
			// Update the key of the moved delegate
			auto keyOfMovedDelegate = m_TickDelegateKeys.find(m_TickDelegates.size() - 1);
			m_TickDelegateKeys[keyOfMovedDelegate->first] = it->second;
		}

		m_TickDelegates.pop_back();
		m_TickDelegateKeys.erase(it);
		tickDelegate.Status = ETickDelegateStatus::Invalid;
	}

	std::cout << "Removed tick delegate with key: " << tickDelegate.Key << std::endl;
}

} // end SE namespace
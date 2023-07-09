#include "SETimeManager.hpp"

#include <iostream>
#include <iomanip>

namespace SE {

SETimeManager::SETimeManager()
{

}

SETimeManager::~SETimeManager()
{

}

void SETimeManager::initialize()
{
	m_CurrentFrameTime = std::chrono::steady_clock::now();
	update_frame_times();
}

void SETimeManager::update()
{
	update_frame_times();
	update_delta_time();
	update_fps();
	print_fps();
}

void SETimeManager::update_frame_times()
{
	auto newTime = std::chrono::steady_clock::now();
	m_LastFrameTime = m_CurrentFrameTime;
	m_CurrentFrameTime = newTime;
}

void SETimeManager::update_delta_time()
{
	m_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(m_CurrentFrameTime - m_LastFrameTime).count();
}

void SETimeManager::update_fps()
{
	++m_FrameCount;
	float elapsedSeconds = std::chrono::duration<float, std::chrono::seconds::period>(m_CurrentFrameTime - m_LastFPSCalculation).count();

	if (elapsedSeconds > 1.0f)
	{
		m_FPS = static_cast<uint16_t>(m_FrameCount / elapsedSeconds);
		m_LastFPSCalculation = m_CurrentFrameTime;
		m_FrameCount = 0;
	}
}

void SETimeManager::print_fps()
{
	std::cout << "\rCurrent FPS: " << std::setw(3) << m_FPS << "   " << std::flush;
}

} // end SE namespace
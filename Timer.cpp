#include "pch.h"
#include "Timer.h"

Timer::Timer() : m_lastTime(std::chrono::steady_clock::now())
{
	m_frameTimes.resize(MAX_SAMPLE_COUNT, 0.016f);
}
// 400명동접 로비서버 room 방 100개

float Timer::Tick(float lockFPS)
{
	// 1. 경과 시간 측정
	auto currentTime = std::chrono::steady_clock::now();
	auto elapsed = currentTime - m_lastTime;
	float deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();

	// FPS 제한(yield) 로직
	if (lockFPS > 0.f) {
		float targetSeconds = 1.f / lockFPS; // 60FPS 기준 0.1666...초
		while (deltaTime < targetSeconds) {
			currentTime = std::chrono::steady_clock::now();
			elapsed = currentTime - m_lastTime;
			deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();
			if (deltaTime < targetSeconds) {
				std::this_thread::yield();
			}
		}
	}

	// 현재 시간을 지난시간으로 update
	m_lastTime = currentTime;

	// 스무딩 (deltaTime을 링버퍼에 저장) > 클럭이 튀면 끊기는게 아닌 끊긴만큼 게임이 빨라짐
	m_frameTimes[m_currentSample] = deltaTime;
	m_currentSample = (m_currentSample + 1) % MAX_SAMPLE_COUNT;

	// FPS 계산 로직
	m_frameCount++;
	m_fpsTimeElapsed += deltaTime;
	if (m_fpsTimeElapsed > 1.f) {
		m_currentFPS = m_frameCount;
		m_frameCount = 0;
		m_fpsTimeElapsed -= 1.f;
	}
	return deltaTime;
}

float Timer::GetTimeElapsed() const
{
	float sum = std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.f);
	float avg = sum / MAX_SAMPLE_COUNT;
	return avg;
}

unsigned int Timer::GetFrameRate() const
{
	return m_currentFPS;
}

std::wstring Timer::GetFrameRateStringW() const
{
	return (std::to_wstring(m_currentFPS) + L"FPS");
}

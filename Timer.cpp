#include "pch.h"
#include "Timer.h"

Timer::Timer() : m_lastTime(std::chrono::steady_clock::now())
{
	m_frameTimes.resize(MAX_SAMPLE_COUNT, 0.016f);
}
// 400���� �κ񼭹� room �� 100��

float Timer::Tick(float lockFPS)
{
	// 1. ��� �ð� ����
	auto currentTime = std::chrono::steady_clock::now();
	auto elapsed = currentTime - m_lastTime;
	float deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();

	// FPS ����(yield) ����
	if (lockFPS > 0.f) {
		float targetSeconds = 1.f / lockFPS; // 60FPS ���� 0.1666...��
		while (deltaTime < targetSeconds) {
			currentTime = std::chrono::steady_clock::now();
			elapsed = currentTime - m_lastTime;
			deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();
			if (deltaTime < targetSeconds) {
				std::this_thread::yield();
			}
		}
	}

	// ���� �ð��� �����ð����� update
	m_lastTime = currentTime;

	// ������ (deltaTime�� �����ۿ� ����) > Ŭ���� Ƣ�� ����°� �ƴ� ���丸ŭ ������ ������
	m_frameTimes[m_currentSample] = deltaTime;
	m_currentSample = (m_currentSample + 1) % MAX_SAMPLE_COUNT;

	// FPS ��� ����
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

#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <numeric>
#include <string>

class GameTimer {
public:
	GameTimer() : m_lastTime(std::chrono::steady_clock::now())
	{
		m_frameTimes.resize(MAX_SAMPLE_COUNT, 0.01667f);
	}

	float Tick(float lockFPS = 0.f)
	{
		// 1. 경과 시간 측정
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsed = currentTime - m_lastTime;
		float deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();

		// FPS 제한(yield) 로직
		if (lockFPS > 0.f) {
			float targetSeconds = 1.f / lockFPS; // 60FPS 기준 0.1666...초
			while (deltaTime < targetSeconds) {
				std::this_thread::yield();
				currentTime = std::chrono::steady_clock::now();
				elapsed = currentTime - m_lastTime;
				deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();
			}
		}

		// 현재 시간을 지난시간으로 update
		m_lastTime = currentTime;

		// 스무딩 (deltaTime을 버퍼에 저장)
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
		return GetTimeElapsed();
	}

	float GetTimeElapsed() const
	{
		float sum = std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.f);
		float avg = sum / MAX_SAMPLE_COUNT;
		return avg;
	}

	unsigned int GetFrameRate() const
	{
		return m_currentFPS;
	}

	std::string GetFrameRateString() const
	{
		return std::to_string(m_currentFPS) + " FPS";
	}

private:
	std::chrono::time_point<std::chrono::steady_clock> m_lastTime;

	// 스무딩을 위한 변수
	static const int MAX_SAMPLE_COUNT = 60;
	std::vector<float> m_frameTimes;
	int m_currentSample = 0;

	// FPS 계산용
	unsigned int m_frameCount = 0;
	unsigned int m_currentFPS = 0;
	float m_fpsTimeElapsed = 0.f;
};

int main()
{
	GameTimer timer;
	
	float lockFPS = 60.f;
	int cnt = 0;
	float deltaTime;
	while (true) {
		deltaTime = timer.Tick(lockFPS);
		unsigned int fps = timer.GetFrameRate();
		printf("%d: %dfps, deltaTime: %f\n",cnt++, fps, deltaTime);
		if (!(cnt % 100)) {
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}
}
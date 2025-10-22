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
		// 1. ��� �ð� ����
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsed = currentTime - m_lastTime;
		float deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();

		// FPS ����(yield) ����
		if (lockFPS > 0.f) {
			float targetSeconds = 1.f / lockFPS; // 60FPS ���� 0.1666...��
			while (deltaTime < targetSeconds) {
				std::this_thread::yield();
				currentTime = std::chrono::steady_clock::now();
				elapsed = currentTime - m_lastTime;
				deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();
			}
		}

		// ���� �ð��� �����ð����� update
		m_lastTime = currentTime;

		// ������ (deltaTime�� ���ۿ� ����)
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

	// �������� ���� ����
	static const int MAX_SAMPLE_COUNT = 60;
	std::vector<float> m_frameTimes;
	int m_currentSample = 0;

	// FPS ����
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
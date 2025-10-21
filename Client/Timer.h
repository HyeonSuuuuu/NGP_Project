#pragma once





class Timer {
public:
	Timer();

	float Tick(float lockFPS = 0.f);

	float GetTimeElapsed() const;

	unsigned int GetFrameRate() const;

	std::wstring GetFrameRateStringW() const;

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
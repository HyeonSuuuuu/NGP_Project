// windows 버전 및 최적화 설정
#include "targetver.h"

// 자주 안쓰는 API 제외 (컴파일 속도 향상)
#define WIN32_LEAN_AND_MEAN

// Windows 핵심 header
#include <windows.h>

// c++ 표준 라이브러리
#include <vector>
#include <string>
#include <random>
#include <functional>
#include <chrono>
#include <thread>
#include <numeric>
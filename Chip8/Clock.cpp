#include <chrono>
#include <cmath>

#include "Clock.h"

Clock::Clock() {}

Clock::~Clock() {}

void Clock::init(const float _mhz){
	cpuPeriodMs = 1 / (_mhz * pow(10, 3));
    REGISTER_PERIOD_MS =1 / (60 * pow(10, 3)); // 60hz
    lastStepTime = std::chrono::steady_clock::now();
    lastRegisterTime = std::chrono::steady_clock::now();
}

bool Clock::checkCpuClock(){
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastStepTime).count() > cpuPeriodMs){
		lastStepTime = std::chrono::steady_clock::now();
		return true;
	}
	return false;
}

bool Clock::checkRegisterClock(){
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastRegisterTime).count() > REGISTER_PERIOD_MS){
		lastRegisterTime = std::chrono::steady_clock::now();
		return true;
	}
	return false;
}

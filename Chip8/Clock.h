#pragma once

#include <chrono>
class Clock{
	private:
		double REGISTER_PERIOD_MS; // Register tick period
	
		double cpuPeriodMs; // Cpu tick period
		
		std::chrono::steady_clock::time_point lastStepTime, lastRegisterTime;
		
	public:
		
		bool checkRegisterClock();
		
		bool checkCpuClock();
		
		Clock();
		
		~Clock();

		void init(const float _mhz);
};


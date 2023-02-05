#ifndef _MCPUTIMER_
#define _MCPUTIMER_

#include "Utilities.h"

namespace myLib {
	struct CPUTime {
		CPUTime(double _wall, double _user, double _system) {
			wall = _wall; user = _user; system = _system;
			cpuUsage = (user.value() + system.value()) / wall.value();
		}
		CPUTime() {};
		bool hasValue() { return cpuUsage.has_value(); }
		std::optional<double> wall, user, system, cpuUsage;
	};

	enum class TimerViewDuration {
		ViewDuration_NanoSeconds = 0,
		ViewDuration_MicroSeconds,
		ViewDuration_MilliSeconds,
		ViewDuration_Seconds
	};

	struct CPUTimerInfo {
		CPUTimerInfo() {}
		CPUTimerInfo(const  TimerViewDuration _duration, const size_t _presition) :viewDuration(_duration), avaragePrecision(_presition) {
		}
		//èoóÕå`éÆ
		TimerViewDuration viewDuration = TimerViewDuration::ViewDuration_MilliSeconds;
		//ïΩãœê∏ìx
		size_t avaragePrecision = 0;
	};

	class CPUTimer {
	public:
		CPUTimer(CPUTimerInfo _info);
		CPUTimer();
		~CPUTimer();

		void StartTimer();
		void StopTimer();
		bool isTimerMeasuring() { return !m_Timer.is_stopped(); }
		CPUTime GetAvarage();
		CPUTimerInfo GetTimerInfo() { return  m_sTimerInfo; }
		std::string GetAverageStr(std::string _name);

	private:
		static CPUTimerInfo m_sTimerInfo;

		boost::timer::cpu_timer m_Timer;
		boost::circular_buffer<double> m_WallTimeRecorder;
		boost::circular_buffer<double> m_UserTimeRecorder;
		boost::circular_buffer<double> m_SystemTimeRecorder;
	};
}
#endif
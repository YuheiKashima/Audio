#ifndef _MCPUTIMER_
#define _MCPUTIMER_

#include "MStdPtrHelper.h"

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

	struct CPUPerfCounterInfo {
		CPUPerfCounterInfo() {}
		CPUPerfCounterInfo(const  TimerViewDuration _duration, const size_t _presition) :viewDuration(_duration), avaragePrecision(_presition) {
		}
		//èoóÕå`éÆ
		TimerViewDuration viewDuration = TimerViewDuration::ViewDuration_MilliSeconds;
		//ïΩãœê∏ìx
		size_t avaragePrecision = 0;
	};

	class CPUPerfCounter {
		template<typename T>
		friend class StdPtrHelper;
	public:
		static std::shared_ptr<CPUPerfCounter> CreateInstance(std::string _name) {
			auto shaPtr = StdPtrHelper<CPUPerfCounter>::make_shared(_name);
			m_sTimerList.push_back(shaPtr);
			return shaPtr;
		};
		static void ChangeTimerSetting(CPUPerfCounterInfo& _info) { m_sTimerInfo = _info; }
		static void ViewAllAvarage();

		void StartTimer();
		void StopTimer();
		bool isTimerMeasuring() { return !m_Timer.is_stopped(); }
		CPUTime GetAvarage();
		CPUPerfCounterInfo GetTimerInfo() { return  m_sTimerInfo; }
		std::string GetAverageStr(std::string _name);
	private:
		CPUPerfCounter(std::string _name);
		~CPUPerfCounter();

		static CPUPerfCounterInfo m_sTimerInfo;
		static std::vector<std::weak_ptr<CPUPerfCounter>> m_sTimerList;

		std::string m_TimerName;
		boost::timer::cpu_timer m_Timer;
		boost::circular_buffer<double> m_WallTimeRecorder;
		boost::circular_buffer<double> m_UserTimeRecorder;
		boost::circular_buffer<double> m_SystemTimeRecorder;
	};
}
#endif
#include "MCPUTimer.h"

myLib::CPUTimer::CPUTimer(myLib::CPUTimerInfo _info) :m_TimerInfo(_info) {
	if (m_TimerInfo.avaragePrecision > 0) {
		m_WallTimeRecorder = boost::circular_buffer<double>(m_TimerInfo.avaragePrecision);
		m_UserTimeRecorder = boost::circular_buffer<double>(m_TimerInfo.avaragePrecision);
		m_SystemTimeRecorder = boost::circular_buffer<double>(m_TimerInfo.avaragePrecision);
	}
}

myLib::CPUTimer::CPUTimer() {
}

myLib::CPUTimer::~CPUTimer() {
}

void myLib::CPUTimer::StartTimer() {
	if (!m_Timer.is_stopped())
		m_Timer.stop();
	if (m_TimerInfo.avaragePrecision > 0)
		m_Timer.start();
}

void myLib::CPUTimer::StopTimer() {
	if (m_TimerInfo.avaragePrecision > 0 && !m_Timer.is_stopped()) {
		m_Timer.stop();
		boost::timer::cpu_times elapsed = m_Timer.elapsed();
		m_WallTimeRecorder.push_back(static_cast<double>(elapsed.wall));
		m_UserTimeRecorder.push_back(static_cast<double>(elapsed.user));
		m_SystemTimeRecorder.push_back(static_cast<double>(elapsed.system));
	}
}

myLib::CPUTime myLib::CPUTimer::GetAvarage() {
	if (m_TimerInfo.avaragePrecision > 0) {
		//calc avarage
		double wallAve = std::reduce(m_WallTimeRecorder.begin(), m_WallTimeRecorder.end(), 0.0) / m_WallTimeRecorder.size();
		double userAve = std::reduce(m_UserTimeRecorder.begin(), m_UserTimeRecorder.end(), 0.0) / m_UserTimeRecorder.size();
		double sysAve = std::reduce(m_SystemTimeRecorder.begin(), m_SystemTimeRecorder.end(), 0.0) / m_SystemTimeRecorder.size();

		//change to viewduration
		double dur = static_cast<double>(m_TimerInfo.viewDuration);
		double durpow = std::pow(1000.0, dur);
		wallAve /= durpow;
		userAve /= durpow;
		sysAve /= durpow;

		return 	CPUTime(wallAve, userAve, sysAve);
	}
	else {
		return CPUTime();
	}
}

std::string myLib::CPUTimer::GetAverageStr(std::string _name) {
	if (m_TimerInfo.avaragePrecision <= 0)return "No setup Timer";
	CPUTime cputime = GetAvarage();
	std::string unit;
	switch (m_TimerInfo.viewDuration) {
	case TimerViewDuration::ViewDuration_NanoSeconds:
		unit = "ns";
		break;
	case TimerViewDuration::ViewDuration_MicroSeconds:
		unit = "mics";
		break;
	case TimerViewDuration::ViewDuration_MilliSeconds:
		unit = "ms";
		break;
	case TimerViewDuration::ViewDuration_Seconds:
		unit = "s";
		break;
	}
	std::stringstream strstr;
	strstr << "<" << _name << ">" <<
		"\t" << cputime.wall << unit << " wall" <<
		"\t" << cputime.user << unit << " user" <<
		"\t" << cputime.system << unit << " system" <<
		"\t(" << cputime.cpuUsage << "%)" << std::endl;

	return strstr.str();
}
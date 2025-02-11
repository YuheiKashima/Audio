#include "CPUPerfCounter.h"

using namespace myLib;
using namespace std;

CPUPerfCounterInfo CPUPerfCounter::m_sTimerInfo{ TimerViewDuration::ViewDuration_MilliSeconds,10 };

CPUPerfCounter::CPUPerfCounter(string _name) :m_TimerName(_name) {
	if (!m_Timer.is_stopped())	m_Timer.stop();
}

CPUPerfCounter::~CPUPerfCounter() {
}

void CPUPerfCounter::ViewAllAvarage() {
	erase_if(m_sTimerList, [](auto child) {return child.expired(); });
	for (auto& w_timer : m_sTimerList) {
		if (auto timer = w_timer.lock()) {
		}
	}
}

void CPUPerfCounter::StartTimer() {
	if (!m_Timer.is_stopped())	m_Timer.stop();
	if (m_sTimerInfo.avaragePrecision > 0) {
		if (m_WallTimeRecorder.capacity() != m_sTimerInfo.avaragePrecision) {
			m_WallTimeRecorder = boost::circular_buffer<double>(m_sTimerInfo.avaragePrecision);
			m_UserTimeRecorder = boost::circular_buffer<double>(m_sTimerInfo.avaragePrecision);
			m_SystemTimeRecorder = boost::circular_buffer<double>(m_sTimerInfo.avaragePrecision);
		}

		m_Timer.start();
	}
}

void CPUPerfCounter::StopTimer() {
	if (!m_Timer.is_stopped()) {
		m_Timer.stop();

		if (m_WallTimeRecorder.capacity() > 0) {
			boost::timer::cpu_times elapsed = m_Timer.elapsed();
			m_WallTimeRecorder.push_back(static_cast<double>(elapsed.wall));
			m_UserTimeRecorder.push_back(static_cast<double>(elapsed.user));
			m_SystemTimeRecorder.push_back(static_cast<double>(elapsed.system));
		}
	}
}

CPUTime CPUPerfCounter::GetAvarage() {
	if (m_WallTimeRecorder.size() > 0) {
		//calc avarage
		double wallAve = reduce(m_WallTimeRecorder.begin(), m_WallTimeRecorder.end(), 0.0) / m_WallTimeRecorder.size();
		double userAve = reduce(m_UserTimeRecorder.begin(), m_UserTimeRecorder.end(), 0.0) / m_UserTimeRecorder.size();
		double sysAve = reduce(m_SystemTimeRecorder.begin(), m_SystemTimeRecorder.end(), 0.0) / m_SystemTimeRecorder.size();

		//change to viewduration
		double dur = static_cast<double>(m_sTimerInfo.viewDuration);
		double durpow = pow(1000.0, dur);
		wallAve /= durpow;
		userAve /= durpow;
		sysAve /= durpow;

		return 	CPUTime(wallAve, userAve, sysAve);
	}
	else {
		return CPUTime();
	}
}

string CPUPerfCounter::GetAverageStr(string _name) {
	CPUTime cputime = GetAvarage();
	if (!cputime.hasValue())return string();
	string unit;
	switch (m_sTimerInfo.viewDuration) {
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
	stringstream strstr;
	strstr << "<" << _name << ">\t" <<
		cputime.wall.value() << unit << " wall\t" <<
		cputime.user.value() << unit << " user\t" <<
		cputime.system.value() << unit << " system(" <<
		cputime.cpuUsage.value() << "%)" << endl;

	return strstr.str();
}
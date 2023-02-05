﻿#include "MCPUTimer.h"

myLib::CPUTimerInfo myLib::CPUTimer::m_sTimerInfo{};

myLib::CPUTimer::CPUTimer(myLib::CPUTimerInfo _info) {
	m_sTimerInfo = _info;
	if (!m_Timer.is_stopped())	m_Timer.stop();
}

myLib::CPUTimer::CPUTimer() {
	if (!m_Timer.is_stopped())	m_Timer.stop();
}

myLib::CPUTimer::~CPUTimer() {
}

void myLib::CPUTimer::StartTimer() {
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

void myLib::CPUTimer::StopTimer() {
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

myLib::CPUTime myLib::CPUTimer::GetAvarage() {
	if (m_WallTimeRecorder.size() > 0) {
		//calc avarage
		double wallAve = std::reduce(m_WallTimeRecorder.begin(), m_WallTimeRecorder.end(), 0.0) / m_WallTimeRecorder.size();
		double userAve = std::reduce(m_UserTimeRecorder.begin(), m_UserTimeRecorder.end(), 0.0) / m_UserTimeRecorder.size();
		double sysAve = std::reduce(m_SystemTimeRecorder.begin(), m_SystemTimeRecorder.end(), 0.0) / m_SystemTimeRecorder.size();

		//change to viewduration
		double dur = static_cast<double>(m_sTimerInfo.viewDuration);
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
	CPUTime cputime = GetAvarage();
	if (!cputime.hasValue())return std::string();
	std::string unit;
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
	std::stringstream strstr;
	strstr << "<" << _name << ">\t" <<
		cputime.wall.value() << unit << " wall\t" <<
		cputime.user.value() << unit << " user\t" <<
		cputime.system.value() << unit << " system(" <<
		cputime.cpuUsage.value() << "%)" << std::endl;

	return strstr.str();
}
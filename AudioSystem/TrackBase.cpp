#include "TrackBase.h"

std::thread AS::TrackBase::m_sTaskThread;
std::queue<AS::TrackBase::TrackRequest> AS::TrackBase::m_sTaskQueue{};
std::condition_variable AS::TrackBase::m_sTaskVariable;
uint32_t AS::TrackBase::m_sTaskInstances = 0;
AS::TimerLayers AS::TrackBase::m_sCPUTimerLayer = AS::TimerLayers::Timerlayer_None;

AS::TrackBase::TrackBase(AudioFormat _format, std::string _trackType) :m_Format(_format), m_TrackType(_trackType), m_InstanceID(m_sTaskInstances) {
	if (m_sTaskInstances++ <= 0) {
		m_sTaskThread = std::thread(TrackBase::TaskThread);
	}
}

AS::TrackBase::~TrackBase() {
	if (--m_sTaskInstances <= 0 && m_sTaskThread.joinable()) {
		m_sTaskVariable.notify_all();
		m_sTaskThread.join();
	}
}

float AS::TrackBase::Volume(float _vol) {
	return m_Volume =
		Volume_Max >= _vol && _vol >= Volume_Min ? _vol :
		_vol > Volume_Max ? Volume_Max :
		_vol < Volume_Min ? Volume_Min :
		0.0f;
}

void AS::TrackBase::RegisterTask(TrackRequest& _request) {
	m_sTaskQueue.push(_request);
	m_sTaskVariable.notify_all();
}

void AS::TrackBase::TaskProcess(TrackRequest& _request) {
}

void AS::TrackBase::SetupCPUMeasure(TimerLayers _layers, CPUTimerInfo _info) {
	m_sCPUTimerLayer = _layers;
}

std::string AS::TrackBase::OutputCPUMeasure() {
	return "none";
}

void AS::TrackBase::TaskThread() {
	std::mutex taskMutex;
	std::unique_lock<std::mutex> uniqMutex(taskMutex);

	while (m_sTaskInstances > 0) {
		while (!m_sTaskQueue.empty()) {
			auto request = m_sTaskQueue.front();
			request.requestTrack.TaskProcess(request);
			m_sTaskQueue.pop();
		}
		m_sTaskVariable.wait(uniqMutex, [&] {return m_sTaskInstances <= 0 || !m_sTaskQueue.empty(); });
	}
}
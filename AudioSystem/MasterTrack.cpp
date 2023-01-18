#include "MasterTrack.h"

AS::MasterTrack::MasterTrack(AudioFormat _format, uint32_t _createFrames) :TrackBase(_format, "MasterTrack") {
	std::stringstream strstr;
	strstr << "Track\t\t:" << m_InstanceID << std::endl;
	strstr << "Type\t\t:" << m_TrackType << std::endl;
	strstr << "TrackFormat<" << std::endl;
	strstr << "\t" << "Channel\t:" << m_Format.channnels << std::endl;
	strstr << "\t" << "SamplingRate\t:" << m_Format.samplingRate << std::endl;
	strstr << "\t" << "BitDepth\t:" << m_Format.bitDepth << std::endl;
	strstr << ">" << std::endl;
	strstr << "CreateFrameSize\t:" << _createFrames << std::endl;
	Log::Logging(Log::ASLOG_INFO, strstr.str(), std::source_location::current());
}

AS::MasterTrack::~MasterTrack() {
}

std::string AS::MasterTrack::OutputCPUMeasure() {
	std::string dest = m_CPUTimer.GetAverageStr("Master");
	for (auto& wpchild : m_Children) {
		if (auto child = wpchild.lock()) {
			dest += child->OutputCPUMeasure();
		}
	}
	return dest;
}

size_t AS::MasterTrack::GetBuffer(LineBuffer<float>& _dest, uint32_t _frames) {
	if (static_cast<uint32_t>(m_CPUTimerLayer) & static_cast<uint32_t>(TimerLayers::Timerlayer_MasterTime))
		m_CPUTimer.StartTimer();

	std::erase_if(m_Children, [](auto child) {return child.expired(); });

	const uint32_t avxAdd = sizeof(__m256) / sizeof(float);
	LineBuffer<float> mixBuffer(m_Format.channnels, _frames);

	for (auto& wpchild : m_Children) {
		if (auto child = wpchild.lock()) {
			AudioFormat childFormat = child->GetFormat();
			if (m_Format != childFormat)continue;
			mixBuffer.zeroclear();

			child->GetBuffer(mixBuffer, _frames);
			_dest.add(mixBuffer);
		}
	}

	if (static_cast<uint32_t>(m_CPUTimerLayer) & static_cast<uint32_t>(TimerLayers::Timerlayer_MasterTime))
		m_CPUTimer.StopTimer();
	return _frames;
}

size_t AS::MasterTrack::ConnectTrack(const std::weak_ptr<TrackBase> _child) {
	if (auto child = _child.lock()) {
		if (typeid(child) == typeid(MasterTrack))return m_Children.size();

		auto foundItr = std::find_if(m_Children.begin(), m_Children.end(), [&child](auto children) {return child == children.lock(); });
		if (foundItr == m_Children.end()) {
			m_Children.push_back(_child);
		}

		std::stringstream strstr;
		strstr << "Connect" << std::endl;
		strstr << "\t" << this->m_TrackType << "(Track:" << m_InstanceID << ")" << "<===>" << child->GetTrackType() << "(Track:" << child->GetInstanceID() << ")" << std::endl;
		Log::Logging(Log::ASLOG_INFO, strstr.str());
	}

	return m_Children.size();
}
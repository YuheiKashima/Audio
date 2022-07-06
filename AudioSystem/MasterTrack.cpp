#include "MasterTrack.h"

AS::MasterTrack::MasterTrack(AudioFormat _format, uint32_t _createFrames) :TrackBase(_format, "MasterTrack") {
	std::stringstream strstr;
	strstr << "Track\t\t:" << m_InstanceID << std::endl;
	strstr << "Type\t\t:" << m_TrackType << std::endl;
	strstr << "TrackFormat<" << std::endl;
	strstr << "\t" << "Channel\t\t:" << m_Format.channnels << std::endl;
	strstr << "\t" << "SamplingRate\t:" << m_Format.samplingRate << std::endl;
	strstr << "\t" << "BitDepth\t:" << m_Format.bitDepth << std::endl;
	strstr << ">" << std::endl;
	strstr << "CreateFrameSize\t:" << _createFrames << std::endl;
	Log::Logging(Log::ASLOG_INFO, strstr.str());
}

AS::MasterTrack::~MasterTrack() {
}

size_t AS::MasterTrack::GetBuffer(LineBuffer<float>& _dest, uint32_t _frames) {
	std::erase_if(m_Children, [](auto child) {return child.expired(); });

	const uint32_t avxAdd = sizeof(__m256) / sizeof(float);
	LineBuffer<float> mixBuffer(m_Format.channnels, _frames);

#if MEASUREMENT_MASTER
	timestamp start = Chrono::GetTime();
#endif

	for (auto& wpchild : m_Children) {
		if (auto child = wpchild.lock()) {
			AudioFormat childFormat = child->GetFormat();
			if (m_Format != childFormat)continue;

			child->GetBuffer(mixBuffer, _frames);
			_dest.avx_add(mixBuffer);
		}
	}
#if MEASUREMENT_MASTER
	m_DevMeasurement[m_DevMeasurementCount++] = Chrono::GetDuration<nanoseconds>(start, Chrono::GetTime()) / 1000000.0;
	if (m_DevMeasurementCount >= MEASUREMENT_AVERAGE) {
		std::stringstream strstr;
		strstr << "Master : " << std::reduce(m_DevMeasurement.begin(), m_DevMeasurement.end(), 0.0) / MEASUREMENT_AVERAGE << "ms" << std::endl;
		m_DevMeasurementCount = 0;
		Log::Logging(strstr.str(), false);
	}
#endif

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
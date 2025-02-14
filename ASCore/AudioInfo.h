/**

	@file      AudioInfo.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      15.02.2025

**/

#ifndef _AUDIOINFO_
#define _AUDIOINFO_

#include <string>

#include "AudioFormat.h"

namespace AS {
	/**

		@class   AudioInfo
		@brief
		@details ~

	**/
	class AudioInfo : public AudioFormat {
	private:
		std::optional<std::string> m_audioName;
		std::optional<int32_t> m_allSamples;

	public:
		AudioInfo() :m_audioName(""), m_allSamples(0) {
		}
		AudioInfo(std::string _audioName, int32_t _allSamples, int32_t _channel, int32_t _bitDepth, uint32_t _samplingFreq) :
			AudioFormat(_channel, _bitDepth, _samplingFreq), m_audioName(_audioName), m_allSamples(_allSamples) {
		}
		~AudioInfo() {
		}

		bool operator ==(const AudioInfo _f) {
			if (m_channel != _f.m_channel)return false;
			if (m_bitDepth != _f.m_bitDepth)return false;
			if (m_samplingFreq != _f.m_samplingFreq)return false;
			return true;
		}

		void operator =(const AudioInfo& _f) {
			m_audioName = _f.m_audioName;
			m_allSamples = _f.m_allSamples;
			m_channel = _f.m_channel;
			m_bitDepth = _f.m_bitDepth;
			m_samplingFreq = _f.m_samplingFreq;
		}

		/**
			@fn     audioName
			@brief
			@retval  -
		**/
		constexpr std::optional<std::string> audioName() const { return m_audioName.has_value() ? m_audioName : std::optional<std::string>("No Name"); }
		/**
			@fn     allSamples
			@brief
			@retval  -
		**/
		constexpr std::optional<int32_t> allSamples() const { return m_allSamples.has_value() ? m_allSamples : std::nullopt; }
	};
}

#endif // _AUDIOINFO_
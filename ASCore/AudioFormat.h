/**

	@file      AudioFormat.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      14.02.2025

**/

#ifndef _AUDIOFORMAT_
#define _AUDIOFORMAT_

#include <optional>

namespace AS {
	/**

		@class   AudioFormat
		@brief
		@details ~

	**/
	class AudioFormat {
	protected:
		std::optional<int32_t> m_channel;
		std::optional<int32_t> m_bitDepth;
		std::optional<int32_t> m_samplingFreq;
	public:
		AudioFormat() :m_channel(0), m_bitDepth(0), m_samplingFreq(0) {
		}
		AudioFormat(int32_t _channel, int32_t _bitDepth, int32_t _samplingFreq) :
			m_channel(_channel), m_bitDepth(_bitDepth), m_samplingFreq(_samplingFreq) {
		}
		~AudioFormat() {
		}

		bool operator ==(const AudioFormat _f) {
			if (m_channel != _f.m_channel)return false;
			if (m_bitDepth != _f.m_bitDepth)return false;
			if (m_samplingFreq != _f.m_samplingFreq)return false;
			return true;
		}

		void operator =(const AudioFormat& _f) {
			m_channel = _f.m_channel;
			m_bitDepth = _f.m_bitDepth;
			m_samplingFreq = _f.m_samplingFreq;
		}

		/**
			@fn     channel
			@brief
			@retval  -
		**/
		constexpr std::optional<int32_t> channel() const { return m_channel.has_value() ? m_channel : std::nullopt; }
		/**
			@fn     bitDepth
			@brief
			@retval  -
		**/
		constexpr std::optional<int32_t> bitDepth() const { return m_bitDepth.has_value() ? m_bitDepth : std::nullopt; }
		/**
			@fn     samplingFreq
			@brief
			@retval  -
		**/
		constexpr std::optional<int32_t> samplingFreq() const { return m_samplingFreq.has_value() ? m_samplingFreq : std::nullopt; }
	};
}
#endif
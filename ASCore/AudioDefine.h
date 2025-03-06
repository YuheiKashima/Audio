/**

	@file      AudioDefine.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      15.02.2025

**/

#ifndef _AUDIODEFINE_
#define _AUDIODEFINE_

#include <vector>
#include <numeric>
#include <boost/config/detail/suffix.hpp>

#include "AudioInfo.h"
#include "LineBuffer.h"

namespace AS {
	using byte = uint8_t;
	const int32_t LoopInfinity = std::numeric_limits<int32_t>::max BOOST_PREVENT_MACRO_SUBSTITUTION();

	//enum class TimerLayers {
	//	Timerlayer_None = 0x0000,
	//	Timerlayer_SystemTime = 0x0001,
	//	Timerlayer_MasterTime = 0x004,
	//	Timerlayer_SourceTime = 0x008,
	//	Timerlayer_IOTime = 0x0010,
	//	Timerlayer_RenderingTime = Timerlayer_SystemTime | Timerlayer_MasterTime | Timerlayer_SourceTime | Timerlayer_IOTime,

	//	//ï°çáíËã`
	//	DTL_SyMaSo = Timerlayer_SystemTime | Timerlayer_MasterTime | Timerlayer_SourceTime,
	//	DTL_SySoIo = Timerlayer_SystemTime | Timerlayer_SourceTime | Timerlayer_IOTime,
	//	DTL_SyMa = Timerlayer_SystemTime | Timerlayer_MasterTime,
	//	DTL_SySo = Timerlayer_SystemTime | Timerlayer_SourceTime,
	//	DTL_MaSo = Timerlayer_MasterTime | Timerlayer_SourceTime,
	//	DTL_SoIo = Timerlayer_SourceTime | Timerlayer_IOTime
	//};

	int32_t TimeToFrames(const AudioFormat _format, const int32_t _time) {
		return static_cast<int32_t>(lround(_format.samplingFreq().value() * (_time / 1000.0)));
	}

	int32_t FramesToTime(const AudioFormat _format, const int32_t _frames) {
		return static_cast<int32_t>(lround((_frames * 1000.0) / _format.samplingFreq().value()));
	}
}

#endif // !_AUDIODEFINE_

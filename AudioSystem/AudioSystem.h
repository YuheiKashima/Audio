#ifndef _AUDIOSYSTEM_
#define _AUDIOSYSTEM_

#include "EndPointBase.h"

#define _DEBUGSINWAVE false
#define _DEBUGNOARG true

namespace AS {
	class AudioSystem;
	using UPAudioSystem = std::unique_ptr<AudioSystem>;
	using SPAudioSystem = std::shared_ptr<AudioSystem>;
	using WPAudioSystem = std::weak_ptr<AudioSystem>;

	class AudioSystem {
	public:
		AudioSystem();
		~AudioSystem();

		template <class ENDPOINT>
		uint32_t EnumerateDevices(const EEndPointMode _mode, DeviceList& _destList);
		void LunchDevice(LunchInfo& _info);
		void SetupDevice(const EEndPointMode _mode, SetupInfo& _info);
		void Start(const EEndPointMode _mode, StartInfo& _info);
		void Stop(const EEndPointMode _mode);

		std::shared_ptr<MasterTrack> CreateMasterTrack();
		std::shared_ptr<SourceTrack> CreateSourceTrack(std::weak_ptr<MasterTrack> _connectMaster, const uint32_t _bufferTime);
		std::pair<std::shared_ptr<SourceTrack>, std::shared_ptr<EffectManager>> CreateSourceTrackWithEffect(std::weak_ptr<MasterTrack> _connectMaster, const uint32_t _bufferTime, const EEffectTiming _effectTiming);
		std::shared_ptr<SourceTrack> CreateSourceTrackIndepend(const uint32_t _bufferTime);
		std::shared_ptr<EffectManager> CreateEffectManager(std::weak_ptr<SourceTrack> _source);
	private:
		void RenderThread(std::weak_ptr<MasterTrack> _master);
		std::thread m_RenderThread;
		std::unique_ptr<EndPointBase> m_upRenderEndPoint;
		bool m_bRenderLoop = false;

		void CaptureThread(std::weak_ptr<SourceTrack> _record);
		std::thread m_CaptureThread;
		std::unique_ptr<EndPointBase> m_upCaptureEndPoint;
		bool m_bCaptureLoop = false;

#if _DEBUGSINWAVE
		static float m_stestSin_a;
		static float m_stestSin_f0;
		static uint32_t m_stestSinWave;
		void testSin(LineBuffer<float>& _buf, const AudioFormat _format);
#endif
	};

	template<class ENDPOINT>
	uint32_t AudioSystem::EnumerateDevices(const EEndPointMode _mode, DeviceList& _destList) {
		static_assert(std::is_base_of<EndPointBase, ENDPOINT>::value, "The specified type is not a derived type of EndPointBase");
		std::unique_ptr<EndPointBase> base(new ENDPOINT);
		return base->EnumrareDevices(_mode, _destList);
	}
}
#endif
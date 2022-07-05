#ifndef _SOURCETRACK_
#define _SOURCETRACK_
#include "TrackBase.h"
#include "WaveBase.h"
#include "EffectManager.h"

namespace AS {
	enum class EPlayState {
		AS_PLAYSTATE_NONE = 0,	//バッファ非生成状態
		AS_PLAYSTATE_UNBIND,	//非バインド状態
		AS_PLAYSTATE_PLAY,		//再生中
		AS_PLAYSTATE_PAUSE,		//一時停止中
		AS_PLAYSTATE_STOP 		//停止中
	};

	enum class EEffectTiming {
		AS_EFFECTTIMING_IO,			//IOスレッドでエフェクト処理
		AS_EFFECTTIMING_SENDBUFFER	//レンダリングスレッドで処理
	};

	struct PlayOption {
		uint32_t playPoint = 0;
		uint32_t loopCount = 0;
		PlayOption() {}
		PlayOption(uint32_t _playPoint, uint32_t _loopCount) :playPoint(_playPoint), loopCount(_loopCount) {}
		PlayOption(uint32_t _loopCount) :loopCount(_loopCount), playPoint(0) {}
	};

	class SourceTrack : public TrackBase {
		template<typename T>
		friend class StdPtrHelper;
		friend class AudioSystem;
	public:

		void Bind(std::weak_ptr<WaveBase> _wave);
		void Play(PlayOption& _option);
		void PlayShot();
		void Pause();
		void Stop();
		EPlayState GetState() { return m_PlayState; }
		bool IsBinding() { return !m_Wave.expired(); }
		void SetEndingCallback(std::function<void(void)> _stopCall) { m_EndCallback = _stopCall; }
	private:
		enum class ETrackNum {
			NONE = -1,
			PRIMARY,
			SECONDRY,
			DOUBLE_MAX
		};

		SourceTrack(AudioFormat _format, uint32_t _createFrames);
		SourceTrack(AudioFormat _format, uint32_t _createFrames, EEffectTiming _effectTiming, std::weak_ptr<EffectManager> _effectManager);
		~SourceTrack();
		static std::shared_ptr<SourceTrack> CreateInstance(AudioFormat& _format, uint32_t _createFrames) {
			return StdPtrHelper<SourceTrack>::make_shared(_format, _createFrames);
		}
		static std::shared_ptr<SourceTrack> CreateInstance(AudioFormat _format, uint32_t _createFrames, EEffectTiming _effectTiming, std::weak_ptr<EffectManager> _effectManager) {
			return StdPtrHelper<SourceTrack>::make_shared(_format, _createFrames, _effectTiming, _effectManager);
		}

		EEffectTiming m_EffectTiming = EEffectTiming::AS_EFFECTTIMING_SENDBUFFER;
		EPlayState m_PlayState = EPlayState::AS_PLAYSTATE_NONE;
		std::array<Track, static_cast<size_t>(ETrackNum::DOUBLE_MAX)> m_Tracks;
		uint32_t m_UseTrack = 0;
		uint32_t m_Cuesor = 0;
		uint32_t m_Loop = 0;
		std::weak_ptr<WaveBase> m_Wave;

		std::weak_ptr<EffectManager> m_wpEffectManager;

		std::function<void(void)> m_EndCallback;

		void TaskProcess(TrackRequest& _request) override;

		void CreateBuffer(AudioFormat _format, uint32_t _createFrames);
		size_t GetBuffer(LineBuffer<float>& _dest, uint32_t _frames)override;
		void PreLoad();
		void Load(Track& _dest, size_t loadFrames);
		size_t ConnectTrack(const std::weak_ptr<TrackBase> _child) override { return 0; }
	};
}
#endif
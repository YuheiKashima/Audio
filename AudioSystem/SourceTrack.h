#ifndef _SOURCETRACK_
#define _SOURCETRACK_
#include "TrackBase.h"
#include "WaveBase.h"
#include "EffectManager.h"

namespace AS {
	enum class EPlayState {
		AS_PLAYSTATE_NONE = 0,		//バッファ非生成状態
		AS_PLAYSTATE_UNBIND,		//非バインド状態
		AS_PLAYSTATE_STOP, 			//停止中
		AS_PLAYSTATE_PAUSE,			//一時停止中
		AS_PLAYSTATE_PLAY,			//再生中
		AS_PLAYSTATE_OUT,			//停止準備(コールバックあり)
		AS_PLAYSTATE_OUT_NOCALLBACK,//停止準備(コールバック無し)
		AS_PLAYSTATE_MAX
	};

	enum class EEffectTiming {
		AS_EFFECTTIMING_IO,			//IOスレッドでエフェクト処理
		AS_EFFECTTIMING_SENDBUFFER	//レンダリングスレッドで処理
	};

	struct PlayOption {
		//再生開始地点(ms)
		uint32_t playPoint = 0;
		//ループ数設定
		uint32_t loopCount = 0;
		//事前バッファ要求時間(時間分のバッファが満たされた時再生を開始する)(ms)
		uint32_t preLoadTime = 0;
		PlayOption() {}
		PlayOption(uint32_t _playPoint, uint32_t _loopCount) :playPoint(_playPoint), loopCount(_loopCount), preLoadTime(0) {}
		PlayOption(uint32_t _loopCount) :loopCount(_loopCount), playPoint(0), preLoadTime(0) {}
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
		std::string GetStateStr() { return m_sPlayStateStr[static_cast<size_t>(GetState())]; }
		bool IsBinding() { return !m_Wave.expired(); }
		void SetEndingCallback(std::function<void(void)> _stopCall) { m_EndCallback = _stopCall; }
	private:
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
		EPlayState m_TempState = EPlayState::AS_PLAYSTATE_NONE;

		Track m_Track;

		uint32_t m_Loop = 0;
		std::weak_ptr<WaveBase> m_Wave;

		std::weak_ptr<EffectManager> m_wpEffectManager;

		std::function<void(void)> m_EndCallback;

		static const float m_sOutLimitDB;
		static const std::array < std::string, static_cast<size_t>(EPlayState::AS_PLAYSTATE_MAX)> m_sPlayStateStr;

		void TaskProcess(TrackRequest& _request) override;

		void CreateBuffer(AudioFormat _format, uint32_t _createFrames);
		size_t GetBuffer(LineBuffer<float>& _dest, uint32_t _frames)override;
		size_t Load(LineBuffer<float>& _dest, size_t loadFrames, bool& _isEnd);
		size_t ConnectTrack(const std::weak_ptr<TrackBase> _child) override { return 0; }
	};
}
#endif
#pragma once
#include "TrackBase.h"

namespace AS {
	struct RecordOption {
	};

	class RecordTrack : public TrackBase {
		template<typename T>
		friend class StdPtrHelper;
		friend class AudioSystem;
	public:
		void Recode();
	private:
		static std::shared_ptr<RecordTrack> CreateInstance(AudioFormat& _format, int32_t _createFrames) {
			return StdPtrHelper<RecordTrack>::make_shared(_format, _createFrames);
		}

		size_t GetBuffer(LineBuffer<float>& _track, const int32_t _frames) override;
		size_t WriteBuffer(LineBuffer<float>& _dest);
		size_t ConnectTrack(const std::weak_ptr<TrackBase> _child) override { return 0; }

		Track m_Track;
	};
}

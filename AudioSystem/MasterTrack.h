#ifndef _MASTERTRACK_
#define _MASTERTRACK_

#include "SourceTrack.h"

namespace AS {
	class MasterTrack : public TrackBase {
		template<typename T>
		friend class StdPtrHelper;
		friend class AudioSystem;
	public:
		size_t GetChildrenSize() { return m_Children.size(); }
	private:
		MasterTrack(AudioFormat _format, uint32_t _createFrames);
		~MasterTrack();

		static std::shared_ptr<MasterTrack> CreateInstance(AudioFormat _format, uint32_t _createFrames) {
			return StdPtrHelper<MasterTrack>::make_shared(_format, _createFrames);
		}

		size_t GetBuffer(LineBuffer<float>& _dest, uint32_t _frames)override;
		size_t ConnectTrack(const std::weak_ptr<TrackBase> _child)override;

		std::vector<std::weak_ptr<TrackBase>> m_Children;

#if MEASUREMENT_MASTER
		boostMeasurement m_DebMeasurement;
#endif
	};
}
#endif
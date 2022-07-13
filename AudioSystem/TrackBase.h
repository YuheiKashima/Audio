#ifndef _TRACKBASE_
#define _TRACKBASE_

#include "AudioDefines.h"

namespace AS {
	class TrackBase :public std::enable_shared_from_this<TrackBase> {
	public:
		struct Track {
#if USE_CIRCULAR
			std::vector<boost::circular_buffer<float>> circular;
#else
			LineBuffer<float> buffer;
			size_t fillingBuffer = 0;
#endif
			std::shared_mutex mutex;
			bool is_End = false;
		};

		class TrackRequest {
		public:
			TrackRequest(TrackBase& _track, Track& _taskTrack, const size_t _order) :requestTrack(_track), taskTrack(_taskTrack), orderFrames(_order) {};
			~TrackRequest() {};

			TrackBase& requestTrack;
			Track& taskTrack;
			size_t orderFrames;
		};

		TrackBase(AudioFormat _format, std::string _trackType);
		virtual ~TrackBase();

		float Volume(float _vol);
		float Volume() { return m_Volume; }
		AudioFormat GetFormat() { return m_Format; }
		std::string GetTrackType() { return m_TrackType; }
		uint32_t GetInstanceID() { return m_InstanceID; }

		virtual size_t GetBuffer(LineBuffer<float>& _track, const uint32_t frames) = 0;
		virtual size_t ConnectTrack(const std::weak_ptr<TrackBase> _child) = 0;
	protected:
		void RegisterTask(TrackRequest& _request);

		virtual void TaskProcess(TrackRequest& _request);
		const uint32_t GetInstancesSize() { return m_sTaskInstances; }

		const AudioFormat m_Format;
		float m_Volume = 1.0f;
		const uint32_t m_InstanceID = 0;
		const std::string m_TrackType;
	private:
		static std::thread m_sTaskThread;
		static std::queue<TrackRequest> m_sTaskQueue;
		static std::condition_variable m_sTaskVariable;
		static uint32_t m_sTaskInstances;

		static void TaskThread();
	};
};

#endif
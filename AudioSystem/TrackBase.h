#ifndef _TRACKBASE_
#define _TRACKBASE_

#include "AudioDefines.h"

namespace AS {
	class TrackBase :public std::enable_shared_from_this<TrackBase> {
	public:
		struct Track {
			std::vector<boost::circular_buffer<float>> circular;
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
		int32_t GetInstanceID() { return m_InstanceID; }

		virtual size_t GetBuffer(LineBuffer<float>& _track, const int32_t frames) = 0;
		virtual size_t ConnectTrack(const std::weak_ptr<TrackBase> _child) = 0;
		virtual std::string OutputCPUMeasure();
	protected:
		const int32_t GetInstancesSize() { return m_sTaskInstances; }
		void RegisterTask(TrackRequest& _request);
		virtual void TaskProcess(TrackRequest& _request);

		const AudioFormat m_Format;
		const int32_t m_InstanceID = 0;
		const std::string m_TrackType;

		float m_Volume = 1.0f;
	private:
		static void TaskThread();

		static std::thread m_sTaskThread;
		static std::queue<TrackRequest> m_sTaskQueue;
		static std::condition_variable m_sTaskVariable;
		static int32_t m_sTaskInstances;
	};
};

#endif
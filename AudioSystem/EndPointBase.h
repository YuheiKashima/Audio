#ifndef _ENDPOINTBASE_
#define _ENDPOINTBASE_

#include "AudioDefines.h"
#include "MasterTrack.h"

namespace AS {
	enum class EEndPointMode {
		AS_ENDPOINTMODE_NONE = 0,
		AS_ENDPOINTMODE_RENDER,
		AS_ENDPOINTMODE_CAPTURE,
		AS_ENDPOINTMODE_MAX
	};

	enum class EEndPointState {
		AS_ENDPOINTSTATE_NONE = 0,
		AS_ENDPOINTSTATE_LAUNCHED,
		AS_ENDPOINTSTATE_READY,
		AS_ENDPOINTSTATE_RENDERING,
		AS_ENDPOINTSTATE_CAPTURING,
		AS_ENDPOINTSTATE_MAX
	};

	struct DeviceInfo {
		DeviceInfo() {}
		DeviceInfo(std::string _deviceName, std::string _apiName, EEndPointMode _endPointMode = EEndPointMode::AS_ENDPOINTMODE_RENDER) :deviceName(_deviceName), apiName(_apiName), endpointMode(_endPointMode) {}
		std::string deviceName{};
		std::string apiName{};
		EEndPointMode endpointMode = EEndPointMode::AS_ENDPOINTMODE_NONE;
	};
	using DeviceList = std::vector<DeviceInfo>;

	struct LaunchInfo {
		LaunchInfo() {}
		LaunchInfo(DeviceInfo& _device, AudioFormat& _format)
			:LaunchDevice(_device),
			LaunchFormat(_format) {}
		DeviceInfo LaunchDevice;
		AudioFormat LaunchFormat;
	};

	struct SetupInfo {
	};

	struct StartInfo {
		StartInfo() {}
		StartInfo(std::weak_ptr<MasterTrack> _master) :startMaster(_master) {}
		std::weak_ptr<MasterTrack> startMaster;
	};

	struct FramesInfo {
		int32_t maxFrameSize = 0;
		int32_t paddingFrameSize = 0;
	};

	class EndPointBase {
	public:
		EndPointBase() {}
		virtual ~EndPointBase() {}

		virtual int32_t EnumrareDevices(const EEndPointMode _mode, DeviceList& _destList) = 0;
		virtual void LaunchDevice(LaunchInfo& _info) = 0;
		virtual void SetupDevice(SetupInfo& _info) = 0;
		virtual void Start(StartInfo& _info) = 0;
		virtual void Stop() = 0;

		virtual bool WaitForProcess() = 0;
		virtual void GetFrames(FramesInfo& _destInfo) = 0;
		virtual int32_t Process(LineBuffer<float>& _output, int32_t& _frames) = 0;
		AudioFormat GetFormat() { return m_Format; }
		EEndPointState GetState() { return m_EndpointState; }
		DeviceInfo GetLaunchedDevicceInfo() { return m_DeviceInfo; }

	protected:
		EEndPointState m_EndpointState = EEndPointState::AS_ENDPOINTSTATE_NONE;
		AudioFormat m_Format;
		DeviceInfo m_DeviceInfo;
	};
}
#endif
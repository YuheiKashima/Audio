#ifndef _WASAPI_
#define _WASAPI_

#ifdef _WIN32

#define USE_COINITIALIZEEX false

#include "EndPointBase.h"
#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <wrl.h>

#pragma comment(lib,"Avrt.lib")
#pragma comment(lib,"winmm.lib")

namespace AS {
	using Microsoft::WRL::ComPtr;

	struct WasapiLaunchInfo :public LaunchInfo {
		WasapiLaunchInfo() {}
		WasapiLaunchInfo(DeviceInfo _device, AudioFormat _format, AUDCLNT_SHAREMODE _mode, AudioFormat* _paltFormat)
			:LaunchInfo(_device, _format),
			shareMode(_mode),
			pAltFormat(_paltFormat) {}
		//WasapiMode(共有モード:AUDCLNT_SHAREMODE_SHARED 排他モード:AUDCLNT_SHAREMODE_EXCLUSIVE)
		AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_SHARED;
		//AltanateFormat(Output)
		AudioFormat* pAltFormat = nullptr;
	};

	struct WasapiSetupInfo :public SetupInfo {
		WasapiSetupInfo() {}
		WasapiSetupInfo(uint32_t _periodTime, DWORD _streamFlag)
			:periodTime(_periodTime),
			streamFlags(_streamFlag) {}
		//Period time(ms)(0 = use device default period)
		uint32_t periodTime = 0;
		//Stream flags(AUDCLNT_STREAMFLAGS_~)
		DWORD streamFlags = 0;
	};

	struct WasapiStartInfo :public StartInfo {
		WasapiStartInfo() {}
		WasapiStartInfo(DWORD _timeOutTime) :timeoutTime(_timeOutTime) {}
		WasapiStartInfo(std::weak_ptr<MasterTrack> _master, DWORD _timeOutTime)
			:StartInfo(_master),
			timeoutTime(_timeOutTime) {}
		//event timeout time(millisecond)(sharemode->exclusive&streamflags->AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
		DWORD timeoutTime = 0;
	};

	class Wasapi final : public EndPointBase {
	public:
		friend class AudioSystem;
		Wasapi();
		~Wasapi();

		static std::string GetAPIName() { return m_APIName; }
		uint32_t EnumrareDevices(const EEndPointMode _mode, DeviceList& _destList) override;
		void LaunchDevice(LaunchInfo& _info) override;
		void SetupDevice(SetupInfo& _info) override;
		void Start(StartInfo& _info) override;
		void Stop() override;

		bool WaitForProcess() override;
		void GetFrames(FramesInfo& _destInfo) override;
		uint32_t Process(LineBuffer<float>& _output, uint32_t& _frames) override;

#ifndef _DEBUG
	private:
#endif

		uint32_t CreateDeviceMap(const EEndPointMode _mode, std::map<std::string, IMMDevice*>& _destMap);
		DeviceInfo GetDeviceInfo(IMMDevice* _pDevice, EEndPointMode _mode);
		IMMDevice* FindDeviceFromMap(const std::map<std::string, IMMDevice*>& _devMap, const std::string& _find);
		HRESULT CheckFormat(ComPtr<IAudioClient> _pClient, const AudioFormat _checkFormat, const AUDCLNT_SHAREMODE _shareMode, AudioFormat& _destFormat);
		WAVEFORMATEXTENSIBLE AudioFormatToWFX(const AudioFormat _info);
		AudioFormat WFXToAudioFormat(const WAVEFORMATEXTENSIBLE _wfx);
		HRESULT InitializeClient(ComPtr<IAudioClient> _pClient, const AUDCLNT_SHAREMODE _shareMode, const DWORD _streamFlags, const REFERENCE_TIME _initPeriod, const AudioFormat _initFormat, REFERENCE_TIME& _destFixPeriod);
		void SetupHandle(ComPtr<IAudioClient> _pClient, const DWORD _streamFlag, HANDLE& _destHandle);
		uint32_t RenderProcess(LineBuffer<float>& _output, uint32_t& _frames);
		uint32_t CaptureProcess(LineBuffer<float>& _input, uint32_t& _frames);
	private:
		static	CLSID	m_sCLSID_MMDeviceEnumerator;
		static	IID		m_sIID_IMMDeviceEnumerator;
		static	IID		m_sIID_IAudioClient;
		static	IID		m_sIID_IAudioClock;
		static	IID		m_sIID_IAudioRenderClient;
		static	IID		m_sIID_IAudioCaptureClient;
		static std::map<HRESULT, std::string> m_sErrorDetails;

		static const std::string m_APIName;

		std::map<std::string, IMMDevice*> m_DeviceMap;
		ComPtr<IMMDevice> m_pDevice = nullptr;
		ComPtr<IAudioClient> m_pClient;
		ComPtr<IAudioRenderClient> m_pRenderClient;
		HANDLE m_EventHandle = nullptr;

		AUDCLNT_SHAREMODE m_ShareMode = static_cast<AUDCLNT_SHAREMODE>(-1);
		EEndPointMode m_EndPointMode = EEndPointMode::AS_ENDPOINTMODE_NONE;
		//Periodtime(ns)
		REFERENCE_TIME m_PeriodTime = 0;
		//Minperiodtime(ns)
		//REFERENCE_TIME m_MinPeriodTime = 0;
		//Waittime(ms) use sharemode
		REFERENCE_TIME m_WaitTime = 0;
		uint32_t m_MaxFrameSize = 0;
		uint32_t m_TimeOutTime = 0;
	};
}
#endif
#endif
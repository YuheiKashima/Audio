#include "Wasapi.h"

CLSID	AS::Wasapi::m_sCLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
IID		AS::Wasapi::m_sIID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
IID		AS::Wasapi::m_sIID_IAudioClient = __uuidof(IAudioClient);
IID		AS::Wasapi::m_sIID_IAudioClock = __uuidof(IAudioClock);
IID		AS::Wasapi::m_sIID_IAudioRenderClient = __uuidof(IAudioRenderClient);
IID		AS::Wasapi::m_sIID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const	std::string AS::Wasapi::m_APIName = "Wasapi";
std::map<HRESULT, std::string>		AS::Wasapi::m_sErrorDetails{
	{S_OK,"S_OK"},
	{S_FALSE,"S_FALSE"},
	{E_NOTIMPL,"指定された機能が実装されていません。"},
	{E_NOINTERFACE,"要求された COM インターフェイスは使用できません。"},
	{E_POINTER,"無効なポインターが使用されました。"},
	{E_ABORT,"操作が中止されました。"},
	{E_FAIL,"一般的な障害が発生しました。"},
	{REGDB_E_CLASSNOTREG,"必要な COM オブジェクトが登録されていません。"},
	{CO_E_NOTINITIALIZED,"CoInitialize が呼び出されていません。"},
	{ERROR_INVALID_FUNCTION,"無効な関数が呼び出されました。"},
	{ERROR_FILE_NOT_FOUND,"指定されたファイルが見つかりませんでした。"},
	{ERROR_PATH_NOT_FOUND,"指定されたパスが見つかりませんでした。"},
	{E_ACCESSDENIED,"リソースへのアクセスが拒否されました。"},
	{E_HANDLE,"無効なオブジェクト ハンドルが使用されました。"},
	{E_OUTOFMEMORY,"割り当て可能なメモリーがありません。"},
	{E_INVALIDARG,"無効な引数が検出されました。"},
	{ERROR_GEN_FAILURE,"システムに接続されたデバイスが機能していません。"},
	{ERROR_MOD_NOT_FOUND,"必要な DLL または EXE が見つかりませんでした。"},
	{ERROR_MAX_THRDS_REACHED,"システムの最大スレッド数に達しました。"},
	{ERROR_ALREADY_EXISTS,"既に存在しているため、ファイルを作成できませんでした。"},
	{ERROR_NO_SYSTEM_RESOURCES,"必要なシステム リソースが使い果たされています。"},
	{RPC_S_UNKNOWN_IF,"そのインターフェイスは認識されません。"},
	{RPC_S_CALL_FAILED,"リモート プロシージャ コールに失敗しました。"},
	{RPC_E_CHANGED_MODE,"CoInitializeEx への以前の呼び出しでは、このスレッドの同時実行モデルをマルチスレッドアパートメント（MTA）として指定しました。これは、ニュートラルスレッドのアパートからシングルスレッドのアパートへの変更が発生したことも示している可能性があります。"},
	{ERROR_PARTIAL_COPY,"ReadProcessMemory 要求または WriteProcessMemory 要求の一部だけを完了しました。"},
	{AUDCLNT_E_NOT_INITIALIZED,"Not Initualized"},
	{AUDCLNT_E_ALREADY_INITIALIZED,"Already Initialized"},
	{AUDCLNT_E_WRONG_ENDPOINT_TYPE,"Wrong Endpoint Type"},
	{AUDCLNT_E_DEVICE_INVALIDATED,"Device Invalidated"},
	{AUDCLNT_E_NOT_STOPPED,"Not Stopped"},
	{AUDCLNT_E_BUFFER_TOO_LARGE,"Buffer Too Large"},
	{AUDCLNT_E_OUT_OF_ORDER,"Out of Order"},
	{AUDCLNT_E_UNSUPPORTED_FORMAT,"Unsupported Format"},
	{AUDCLNT_E_INVALID_SIZE,"Invalid Size"},
	{AUDCLNT_E_DEVICE_IN_USE,"Device in Use"},
	{AUDCLNT_E_BUFFER_OPERATION_PENDING,"Buffer Operation Pending"},
	{AUDCLNT_E_THREAD_NOT_REGISTERED,"Thread Not Registered"},
	{AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED,"Not Allowed Exclusive Mode"},
	{AUDCLNT_E_ENDPOINT_CREATE_FAILED,"Endpoint Create Failed"},
	{AUDCLNT_E_SERVICE_NOT_RUNNING,"Service Not Running"},
	{AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED,"Eventhandle Not Expected"},
	{AUDCLNT_E_EXCLUSIVE_MODE_ONLY,"Exclusive Mode Only"},
	{AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL,"Bufduration Period Not Equal"},
	{AUDCLNT_E_EVENTHANDLE_NOT_SET,"Eventhandle Not Set"},
	{AUDCLNT_E_INCORRECT_BUFFER_SIZE,"Incorrect Buffer Size"},
	{AUDCLNT_E_BUFFER_SIZE_ERROR,"Buffer Size Error"},
	{AUDCLNT_E_CPUUSAGE_EXCEEDED,"Cpuusage Exceeded"},
	{AUDCLNT_E_BUFFER_ERROR,"Buffer Error"},
	{AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED,"Buffer Size Not Aligned"},
	{AUDCLNT_E_INVALID_DEVICE_PERIOD,"Invalid Device Period"},
	{AUDCLNT_E_INVALID_STREAM_FLAG,"Invalid Stream Flag"},
	{AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE,"Endpoint Offload Not Capable"},
	{AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES,"Out of Offload Resources"},
	{AUDCLNT_E_OFFLOAD_MODE_ONLY,"Offload Mode Only"},
	{AUDCLNT_E_NONOFFLOAD_MODE_ONLY,"Non Offload Mode Only"},
	{AUDCLNT_E_RESOURCES_INVALIDATED,"Resources Incalodated"},
	{AUDCLNT_E_RAW_MODE_UNSUPPORTED,"Raw Mode Unsupported"}
};

AS::Wasapi::Wasapi() {
	HRESULT res = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(res == S_OK || res == S_FALSE);
}

AS::Wasapi::~Wasapi() {
	if (m_EndpointState == EEndPointState::AS_ENDPOINTSTATE_RENDERING) {
		Stop();
	}

	if (m_EventHandle) {
		CloseHandle(m_EventHandle);
		m_EventHandle = nullptr;
	}
	CoUninitialize();
}

uint32_t AS::Wasapi::CreateDeviceMap(const EEndPointMode _mode, std::map<std::string, IMMDevice*>& _destMap) {
	HRESULT res;
	ComPtr<IMMDeviceEnumerator> pDevEnum = nullptr;
	ComPtr<IMMDeviceCollection> pDevCol = nullptr;
	uint32_t devCount = 0;
	std::stringstream strstr;

	if (!m_DeviceMap.empty())m_DeviceMap.clear();

	//Create enumerator
	res = CoCreateInstance(
		m_sCLSID_MMDeviceEnumerator,
		nullptr,
		CLSCTX_ALL,
		m_sIID_IMMDeviceEnumerator,
		(void**)&pDevEnum
	);
	assert(res == S_OK);
	///todo:CheckError

	///todo:なんか例外処理入れたがいいかも
	EDataFlow dataflow =
		_mode == EEndPointMode::AS_ENDPOINTMODE_RENDER ? EDataFlow::eRender :
		_mode == EEndPointMode::AS_ENDPOINTMODE_CAPTURE ? EDataFlow::eCapture : (EDataFlow)-1;

	//Get default endpoint
	IMMDevice* pDefDev = nullptr;
	res = pDevEnum->GetDefaultAudioEndpoint(dataflow, ERole::eConsole, &pDefDev);
	assert(res == S_OK);
	///todo:CheckError
	_destMap.insert(std::make_pair(GetDeviceInfo(pDefDev, _mode).deviceName + DefaultDeviceStr, pDefDev));

	//Enumerate audio endpoints
	res = pDevEnum->EnumAudioEndpoints(dataflow, DEVICE_STATE_ACTIVE, &pDevCol);
	assert(res == S_OK);
	///todo:CheckError
	res = pDevCol->GetCount(&devCount);
	assert(res == S_OK);
	///todo:CheckError
	if (devCount != 0) {
		for (uint32_t i = 0; i < devCount; ++i) {
			IMMDevice* pDev = nullptr;

			//Item device
			pDevCol->Item(i, &pDev);
			DeviceInfo info = GetDeviceInfo(pDev, _mode);
			_destMap.insert(std::make_pair(info.deviceName, pDev));
		}
	}
	return devCount;
}

AS::DeviceInfo AS::Wasapi::GetDeviceInfo(IMMDevice* _pDevice, EEndPointMode _mode) {
	IPropertyStore* pPropStore = nullptr;
	PROPVARIANT proVar{};

	//Get dev prop
	_pDevice->OpenPropertyStore(STGM_READ, &pPropStore);
	///todo:CheckError

	PropVariantInit(&proVar);
	//get prop
	pPropStore->GetValue(PKEY_Device_DeviceDesc, &proVar);
	DeviceInfo dest(Convert_Wstr_to_Str(std::wstring(proVar.bstrVal)), m_APIName, _mode);
	PropVariantClear(&proVar);

	return dest;
}

IMMDevice* AS::Wasapi::FindDeviceFromMap(const std::map<std::string, IMMDevice*>& _devMap, const std::string& _find) {
	///mapと名称比較forをして、一致したらデバイスを取り出してbreak,
	///全て一致しなかった場合保険処理としてデフォルトデバイスで起動
	IMMDevice* pDefaultDev = nullptr;
	for (const auto& item : _devMap) {
		if (_find == item.first) {
			//見つかったら即return
			return item.second;
		}
		if (item.first.find(DefaultDeviceStr, 0) != std::wstring::npos) {
			pDefaultDev = item.second;
		}
	}

	//見つからなかったらデフォルトデバイスを返す
	return pDefaultDev;
}

HRESULT AS::Wasapi::CheckFormat(ComPtr<IAudioClient> _pClient, const AudioFormat _checkFormat, const AUDCLNT_SHAREMODE _shareMode, AudioFormat& _destFormat) {
	HRESULT res;
	WAVEFORMATEXTENSIBLE wfx = AudioFormatToWFX(_checkFormat), * pAltFormat = nullptr;
	std::stringstream strstr;
	strstr << "\t" << "FormatCheck\t\t:";

	bool output = false;
	res = _pClient->IsFormatSupported(_shareMode, (WAVEFORMATEX*)&wfx, (WAVEFORMATEX**)&pAltFormat);
	switch (res) {
	case S_OK:
		_destFormat = _checkFormat;
		output = true;
		strstr << "Available" << std::endl;
		break;
	case S_FALSE:
		//近似代替フォーマットを返す
		_destFormat = WFXToAudioFormat(*pAltFormat);
		output = true;
		strstr << "Unavailable(output alternate format)" << std::endl;
		break;
	case AUDCLNT_E_UNSUPPORTED_FORMAT:
		//共有モードのみ使用可能
		if (_shareMode == AUDCLNT_SHAREMODE_SHARED) {
			_destFormat = _checkFormat;
			output = true;
			strstr << "Available(Only available in shared mode)" << std::endl;
		}
		else {
			//排他モードはだめでーす
			strstr << "Unavailable" << std::endl;
		}
		break;
	}

	if (output) {
		strstr << "\t\t" << "Channel\t\t:" << _destFormat.channnels << std::endl;
		strstr << "\t\t" << "SamplingRate\t:" << _destFormat.samplingRate << std::endl;
		strstr << "\t\t" << "BitDepth\t:" << _destFormat.bitDepth << std::endl;
	}

	Log::Logging(myLib::Log::ASLOG_INFO, strstr.str(), false);

	return res;
}

WAVEFORMATEXTENSIBLE AS::Wasapi::AudioFormatToWFX(const AudioFormat _info) {
	WAVEFORMATEXTENSIBLE wfx{};

	wfx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
	wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	wfx.Format.nChannels = _info.channnels;
	wfx.Format.nSamplesPerSec = _info.samplingRate;
	wfx.Format.wBitsPerSample = _info.bitDepth;
	wfx.Format.nBlockAlign = (_info.bitDepth / BitsPerByte) * _info.channnels;
	wfx.Format.nAvgBytesPerSec = _info.samplingRate * wfx.Format.nBlockAlign;
	wfx.Samples.wValidBitsPerSample = _info.bitDepth;
	wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	return wfx;
}

AS::AudioFormat AS::Wasapi::WFXToAudioFormat(const WAVEFORMATEXTENSIBLE _wfx) {
	return AudioFormat(_wfx.Format.nSamplesPerSec, _wfx.Format.wBitsPerSample, _wfx.Format.nChannels);
}

HRESULT AS::Wasapi::InitializeClient(ComPtr<IAudioClient> _pClient, const AUDCLNT_SHAREMODE _shareMode, const DWORD _streamFlags, const REFERENCE_TIME _initPeriod, const AudioFormat _initFormat, REFERENCE_TIME& _destFixPeriod) {
	HRESULT res;

	//クライアント起動
	auto initFormat = AudioFormatToWFX(_initFormat);
	res = _pClient->Initialize(_shareMode, _streamFlags, _initPeriod, _initPeriod, (WAVEFORMATEX*)&initFormat, nullptr);
	if (res == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
		//要求されたデバイスバッファアライメントがダメなのでデバイスにあったアライメントを返す
		//fix buffer align
		uint32_t deviceFrame = 0;
		_pClient->GetBufferSize(&deviceFrame);
		_destFixPeriod =
			static_cast<REFERENCE_TIME>((10000.0 * 1000 / deviceFrame * _initFormat.samplingRate) + 0.5);
		//(REFERENCE_TIME)((ms->ns * s->ms / (frames/sec) * frames) + 0.5)
		//	* (frames/sec) / frames にすると(frames/sec)>framesの時丸め処理が入る可能性がある
	}
	return res;
}

void AS::Wasapi::SetupHandle(ComPtr<IAudioClient> _pClient, const DWORD _streamFlag, HANDLE& _destHandle) {
	HRESULT res;
	//ハンドルコールバック設定
	if (_streamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) {
		_destHandle = CreateEvent(nullptr, false, false, nullptr);
		assert(_destHandle != nullptr);

		res = m_pClient->SetEventHandle(_destHandle);
		assert(res == S_OK);
	}
	return;
}

uint32_t AS::Wasapi::RenderProcess(LineBuffer<float>& _output, uint32_t& _frames) {
	HRESULT res;
	byte* pBuffer = nullptr;

	FramesInfo framesInfo;
	GetFrames(framesInfo);
	uint32_t renderFrames = framesInfo.paddingFrameSize == NoPadding ? framesInfo.maxFrameSize : framesInfo.paddingFrameSize;

	if (renderFrames > 0) {
		//デバイスへのバッファを取得
		res = m_pRenderClient->GetBuffer(renderFrames, &pBuffer);
		assert(res == S_OK);
		///todo:CheckError

		memset(pBuffer, NULL, static_cast<size_t>(renderFrames * (m_Format.bitDepth / BitsPerByte) * m_Format.channnels));

		//逆正規化
		PCMNormalizer::PCM_Denormalize(_output, pBuffer, m_Format, renderFrames);

		//収集した音声データをデバイスへ送る
		res = m_pRenderClient->ReleaseBuffer(renderFrames, 0);
		assert(res == S_OK);
	}

	return renderFrames;
}

uint32_t AS::Wasapi::CaptureProcess(LineBuffer<float>& _output, uint32_t& _frames) {
	return 0;
}

uint32_t AS::Wasapi::EnumrareDevices(const EEndPointMode _mode, DeviceList& _destList) {
	auto count = CreateDeviceMap(_mode, m_DeviceMap);
	std::stringstream strstr;
	strstr << m_APIName << "->" << std::source_location::current().function_name() << std::endl;

	if (count != 0) {
		auto count = _destList.size();
		for (const auto& item : m_DeviceMap) {
			{
				strstr << "\tCount\t\t:" << count << std::endl;
				strstr << "\tDeviceName\t:" << item.first << std::endl;
			}

			_destList.push_back(DeviceInfo(item.first, m_APIName, _mode));
			++count;
		}
	}
	Log::Logging(Log::ASLOG_INFO, strstr.str(), std::source_location::current(), true);

	return count;
}

void AS::Wasapi::LunchDevice(LunchInfo& _info) {
	if (m_EndpointState != EEndPointState::AS_ENDPOINTSTATE_NONE)return;
	///todo:ステート違ったらなんか例外処理

	HRESULT res;
	std::stringstream strstr;
	strstr << m_APIName << "->" << std::source_location::current().function_name() << std::endl;
	WasapiLunchInfo& wa_info = static_cast<WasapiLunchInfo&>(_info);
	CreateDeviceMap(wa_info.lunchDevice.endpointMode, m_DeviceMap);
	m_pDevice = FindDeviceFromMap(m_DeviceMap, wa_info.lunchDevice.deviceName);
	//Activate client
	res = m_pDevice->Activate(m_sIID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&m_pClient);
	assert(res == S_OK);
	///todo:CheckError
	{
		strstr << "\tLunchedDevice\t:" << wa_info.lunchDevice.deviceName << std::endl;
		strstr << "\t" << "LunchOrderFormat" << std::endl;
		strstr << "\t\t" << "Channel\t\t:" << wa_info.lunchFormat.channnels << std::endl;
		strstr << "\t\t" << "SamplingRate\t:" << wa_info.lunchFormat.samplingRate << std::endl;
		strstr << "\t\t" << "BitDepth\t:" << wa_info.lunchFormat.bitDepth << std::endl;
		Log::Logging(Log::ASLOG_INFO, strstr.str(), std::source_location::current(), true);
	}

	res = CheckFormat(m_pClient, wa_info.lunchFormat, wa_info.shareMode, m_Format);
	assert(res == S_OK);
	if (res != S_OK) {
		m_pClient->Release();
		m_pClient = nullptr;
		///todo:例外処理
	}

	m_ShareMode = wa_info.shareMode;
	m_EndPointMode = wa_info.lunchDevice.endpointMode;
	m_EndpointState = EEndPointState::AS_ENDPOINTSTATE_LUNCHED;
}

void AS::Wasapi::SetupDevice(SetupInfo& _info) {
	if (m_EndpointState != EEndPointState::AS_ENDPOINTSTATE_LUNCHED)return;
	///todo:ステート違ったらなんか例外処理

	std::stringstream strstr;
	strstr << m_APIName << "->" << std::source_location::current().function_name() << std::endl;
	HRESULT res;
	WasapiSetupInfo& wa_info = static_cast<WasapiSetupInfo&>(_info);
	REFERENCE_TIME period = 0, minPeriod = 0, wait = 0, fix = 0;
	if (wa_info.periodTime != 0) {
		wait = (static_cast<REFERENCE_TIME>(wa_info.periodTime) / 2);
		period = wa_info.periodTime * 10000.0;//ms->ns
	}
	else {
		//Get default device period
		res = m_pClient->GetDevicePeriod(&period, &minPeriod);
		///todo:CheckError
		wait = (period / 10000.0) / 2;//ns->ms
	}

	res = InitializeClient(m_pClient, m_ShareMode, wa_info.streamFlags, period, m_Format, fix);
	assert(res == S_OK || res == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED);
	if (res == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
		//アライメントがダメな場合ここで訂正されたアライメントで再起動
		m_pClient->Release();
		m_pDevice->Activate(m_sIID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&m_pClient);
		res = InitializeClient(m_pClient, m_ShareMode, wa_info.streamFlags, fix, m_Format, fix);
		assert(res == S_OK);

		period = fix;
		wait = fix / 10000LL;
	}
	m_PeriodTime = period;
	m_WaitTime = wait;

	SetupHandle(m_pClient, wa_info.streamFlags, m_EventHandle);

	byte* pBuffer = nullptr;
	res = m_pClient->GetService(m_sIID_IAudioRenderClient, (void**)&m_pRenderClient);
	assert(res == S_OK);

	res = m_pClient->GetBufferSize(&m_MaxFrameSize);
	assert(res == S_OK);

	uint32_t oneFrameSize = m_Format.channnels * (m_Format.bitDepth / BitsPerByte);

	//Clear initial buffer
	res = m_pRenderClient->GetBuffer(m_MaxFrameSize, &pBuffer);
	assert(res == S_OK);
	memset(pBuffer, NULL, static_cast<size_t>(oneFrameSize * m_MaxFrameSize));
	res = m_pRenderClient->ReleaseBuffer(m_MaxFrameSize, 0);
	assert(res == S_OK);

	m_EndpointState = EEndPointState::AS_ENDPOINTSTATE_READY;
}

void AS::Wasapi::Start(StartInfo& _info) {
	if (m_EndpointState != EEndPointState::AS_ENDPOINTSTATE_READY) {
		return;
	}

	WasapiStartInfo& wa_info = static_cast<WasapiStartInfo&>(_info);

	//クライアント始動！
	HRESULT res = m_pClient->Start();
	assert(res == S_OK);

	m_TimeOutTime = wa_info.timeoutTime;

	m_EndpointState = EEndPointState::AS_ENDPOINTSTATE_RENDERING;

	return;
}

void AS::Wasapi::Stop() {
	if (m_EndpointState != EEndPointState::AS_ENDPOINTSTATE_RENDERING) {
		return;
	}

	//クライアント終了！
	HRESULT res = m_pClient->Stop();
	assert(res == S_OK);

	m_EndpointState = EEndPointState::AS_ENDPOINTSTATE_READY;

	return;
}

bool AS::Wasapi::WaitForProcess() {
	if (m_EndpointState != EEndPointState::AS_ENDPOINTSTATE_RENDERING) {
		return false;
	}
	if (m_EventHandle) {
		//Event driven
		DWORD retval = WaitForSingleObject(m_EventHandle, m_TimeOutTime);
		assert(retval == WAIT_OBJECT_0);
	}
	else {
		std::this_thread::sleep_for(std::chrono::milliseconds(m_WaitTime));
	}
	return true;
}

void AS::Wasapi::GetFrames(FramesInfo& _destInfo) {
	if (m_EndpointState == EEndPointState::AS_ENDPOINTSTATE_NONE || m_EndpointState == EEndPointState::AS_ENDPOINTSTATE_MAX) {
		return;
	}

	HRESULT res;
	uint32_t frames = 0;
	uint32_t padding = 0;

	//res = m_pClient->GetBufferSize(&frames);
	//assert(res == S_OK);
	//getPadding frame
	res = m_pClient->GetCurrentPadding(&padding);
	assert(res == S_OK);

	_destInfo.maxFrameSize = m_MaxFrameSize;
	_destInfo.paddingFrameSize = m_MaxFrameSize - padding;

	return;
}

uint32_t AS::Wasapi::Process(LineBuffer<float>& _output, uint32_t& _frames) {
	return m_EndpointState == EEndPointState::AS_ENDPOINTSTATE_RENDERING ? RenderProcess(_output, _frames) : CaptureProcess(_output, _frames);
}
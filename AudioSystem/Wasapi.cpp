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
	{E_NOTIMPL,"�w�肳�ꂽ�@�\����������Ă��܂���B"},
	{E_NOINTERFACE,"�v�����ꂽ COM �C���^�[�t�F�C�X�͎g�p�ł��܂���B"},
	{E_POINTER,"�����ȃ|�C���^�[���g�p����܂����B"},
	{E_ABORT,"���삪���~����܂����B"},
	{E_FAIL,"��ʓI�ȏ�Q���������܂����B"},
	{REGDB_E_CLASSNOTREG,"�K�v�� COM �I�u�W�F�N�g���o�^����Ă��܂���B"},
	{CO_E_NOTINITIALIZED,"CoInitialize ���Ăяo����Ă��܂���B"},
	{ERROR_INVALID_FUNCTION,"�����Ȋ֐����Ăяo����܂����B"},
	{ERROR_FILE_NOT_FOUND,"�w�肳�ꂽ�t�@�C����������܂���ł����B"},
	{ERROR_PATH_NOT_FOUND,"�w�肳�ꂽ�p�X��������܂���ł����B"},
	{E_ACCESSDENIED,"���\�[�X�ւ̃A�N�Z�X�����ۂ���܂����B"},
	{E_HANDLE,"�����ȃI�u�W�F�N�g �n���h�����g�p����܂����B"},
	{E_OUTOFMEMORY,"���蓖�ĉ\�ȃ������[������܂���B"},
	{E_INVALIDARG,"�����Ȉ��������o����܂����B"},
	{ERROR_GEN_FAILURE,"�V�X�e���ɐڑ����ꂽ�f�o�C�X���@�\���Ă��܂���B"},
	{ERROR_MOD_NOT_FOUND,"�K�v�� DLL �܂��� EXE ��������܂���ł����B"},
	{ERROR_MAX_THRDS_REACHED,"�V�X�e���̍ő�X���b�h���ɒB���܂����B"},
	{ERROR_ALREADY_EXISTS,"���ɑ��݂��Ă��邽�߁A�t�@�C�����쐬�ł��܂���ł����B"},
	{ERROR_NO_SYSTEM_RESOURCES,"�K�v�ȃV�X�e�� ���\�[�X���g���ʂ�����Ă��܂��B"},
	{RPC_S_UNKNOWN_IF,"���̃C���^�[�t�F�C�X�͔F������܂���B"},
	{RPC_S_CALL_FAILED,"�����[�g �v���V�[�W�� �R�[���Ɏ��s���܂����B"},
	{RPC_E_CHANGED_MODE,"CoInitializeEx �ւ̈ȑO�̌Ăяo���ł́A���̃X���b�h�̓������s���f�����}���`�X���b�h�A�p�[�g�����g�iMTA�j�Ƃ��Ďw�肵�܂����B����́A�j���[�g�����X���b�h�̃A�p�[�g����V���O���X���b�h�̃A�p�[�g�ւ̕ύX�������������Ƃ������Ă���\��������܂��B"},
	{ERROR_PARTIAL_COPY,"ReadProcessMemory �v���܂��� WriteProcessMemory �v���̈ꕔ�������������܂����B"},
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

	///todo:�Ȃ񂩗�O�������ꂽ����������
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
	///map�Ɩ��̔�rfor�����āA��v������f�o�C�X�����o����break,
	///�S�Ĉ�v���Ȃ������ꍇ�ی������Ƃ��ăf�t�H���g�f�o�C�X�ŋN��
	IMMDevice* pDefaultDev = nullptr;
	for (const auto& item : _devMap) {
		if (_find == item.first) {
			//���������瑦return
			return item.second;
		}
		if (item.first.find(DefaultDeviceStr, 0) != std::wstring::npos) {
			pDefaultDev = item.second;
		}
	}

	//������Ȃ�������f�t�H���g�f�o�C�X��Ԃ�
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
		//�ߎ���փt�H�[�}�b�g��Ԃ�
		_destFormat = WFXToAudioFormat(*pAltFormat);
		output = true;
		strstr << "Unavailable(output alternate format)" << std::endl;
		break;
	case AUDCLNT_E_UNSUPPORTED_FORMAT:
		//���L���[�h�̂ݎg�p�\
		if (_shareMode == AUDCLNT_SHAREMODE_SHARED) {
			_destFormat = _checkFormat;
			output = true;
			strstr << "Available(Only available in shared mode)" << std::endl;
		}
		else {
			//�r�����[�h�͂��߂Ł[��
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

	//�N���C�A���g�N��
	auto initFormat = AudioFormatToWFX(_initFormat);
	res = _pClient->Initialize(_shareMode, _streamFlags, _initPeriod, _initPeriod, (WAVEFORMATEX*)&initFormat, nullptr);
	if (res == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
		//�v�����ꂽ�f�o�C�X�o�b�t�@�A���C�����g���_���Ȃ̂Ńf�o�C�X�ɂ������A���C�����g��Ԃ�
		//fix buffer align
		uint32_t deviceFrame = 0;
		_pClient->GetBufferSize(&deviceFrame);
		_destFixPeriod =
			static_cast<REFERENCE_TIME>((10000.0 * 1000 / deviceFrame * _initFormat.samplingRate) + 0.5);
		//(REFERENCE_TIME)((ms->ns * s->ms / (frames/sec) * frames) + 0.5)
		//	* (frames/sec) / frames �ɂ����(frames/sec)>frames�̎��ۂߏ���������\��������
	}
	return res;
}

void AS::Wasapi::SetupHandle(ComPtr<IAudioClient> _pClient, const DWORD _streamFlag, HANDLE& _destHandle) {
	HRESULT res;
	//�n���h���R�[���o�b�N�ݒ�
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
		//�f�o�C�X�ւ̃o�b�t�@���擾
		res = m_pRenderClient->GetBuffer(renderFrames, &pBuffer);
		assert(res == S_OK);
		///todo:CheckError

		memset(pBuffer, NULL, static_cast<size_t>(renderFrames * (m_Format.bitDepth / BitsPerByte) * m_Format.channnels));

		//�t���K��
		PCMNormalizer::PCM_Denormalize(_output, pBuffer, m_Format, renderFrames);

		//���W���������f�[�^���f�o�C�X�֑���
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
	///todo:�X�e�[�g�������Ȃ񂩗�O����

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
		///todo:��O����
	}

	m_ShareMode = wa_info.shareMode;
	m_EndPointMode = wa_info.lunchDevice.endpointMode;
	m_EndpointState = EEndPointState::AS_ENDPOINTSTATE_LUNCHED;
}

void AS::Wasapi::SetupDevice(SetupInfo& _info) {
	if (m_EndpointState != EEndPointState::AS_ENDPOINTSTATE_LUNCHED)return;
	///todo:�X�e�[�g�������Ȃ񂩗�O����

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
		//�A���C�����g���_���ȏꍇ�����Œ������ꂽ�A���C�����g�ōċN��
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

	//�N���C�A���g�n���I
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

	//�N���C�A���g�I���I
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
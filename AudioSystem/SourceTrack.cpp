#include "SourceTrack.h"

AS::SourceTrack::SourceTrack(AudioFormat _format, uint32_t _createFrames) :TrackBase(_format, "SourceTrack"), m_PlayState(EPlayState::AS_PLAYSTATE_UNBIND) {
	CreateBuffer(_format, _createFrames);
}

AS::SourceTrack::SourceTrack(AudioFormat _format, uint32_t _createFrames, EEffectTiming _effectTiming, std::weak_ptr<EffectManager> _effectManager) : TrackBase(_format, "SourceTrack") {
	CreateBuffer(_format, _createFrames);

	std::stringstream strstr;
	if (!_effectManager.expired()) {
		m_wpEffectManager = _effectManager;
		m_EffectTiming = _effectTiming;
		strstr << "Connect EffectManager" << std::endl;
	}

	Log::Logging(Log::ASLOG_INFO, strstr.str(), false);
}

AS::SourceTrack::~SourceTrack() {
}

void AS::SourceTrack::TaskProcess(TrackRequest& _request) {
	Load(_request.taskTrack, _request.orderFrames);

	//�G�t�F�N�g����(IO�������ɐݒ肵�Ă���Ƃ�)
	if (m_EffectTiming == EEffectTiming::AS_EFFECTTIMING_IO && !m_wpEffectManager.expired()) {
		if (auto effect = m_wpEffectManager.lock())
			effect->Execute(_request.taskTrack.buffer, _request.taskTrack.fillingBuffer);
	}
	else if (m_wpEffectManager.expired()) {
		m_wpEffectManager.reset();
	}
}

void AS::SourceTrack::CreateBuffer(AudioFormat _format, uint32_t _createFrames) {
	for (auto& track : m_Tracks) {
		track.buffer.resize(m_Format.channnels, _createFrames);
	}

	std::stringstream strstr;
	strstr << "Track\t\t:" << m_InstanceID << std::endl;
	strstr << "Type\t\t:" << m_TrackType << std::endl;
	strstr << "TrackFormat<" << std::endl;
	strstr << "\t" << "Channel\t\t:" << m_Format.channnels << std::endl;
	strstr << "\t" << "SamplingRate\t:" << m_Format.samplingRate << std::endl;
	strstr << "\t" << "BitDepth\t:" << m_Format.bitDepth << std::endl;
	strstr << ">" << std::endl;
	strstr << "CreateFrameSize\t:" << _createFrames << std::endl;

	m_PlayState = EPlayState::AS_PLAYSTATE_UNBIND;
	Log::Logging(Log::ASLOG_INFO, strstr.str());
}

size_t AS::SourceTrack::GetBuffer(LineBuffer<float>& _dest, uint32_t _frames) {
	if (m_PlayState != EPlayState::AS_PLAYSTATE_PLAY)return 0;

	auto& primary = m_Tracks.at(m_UseTrack);

	//�g�p�t���[�����Z�o
	size_t sendFrames = 0, remainFrames = 0, maxFrames = std::min(static_cast<size_t>(primary.fillingBuffer), primary.buffer.sizeX());
	bool is_End = false;
	if (m_Cuesor + _frames < maxFrames) {
		sendFrames = _frames;
	}
	else {
		sendFrames = maxFrames - m_Cuesor;
		remainFrames = primary.is_End ? 0 : _frames - sendFrames;
		is_End = primary.is_End && !remainFrames ? true : false;
	}

	{
		//�N���e�B�J���Z�N�V����(�v���C�}���o�b�t�@)
		//�v���C�}���o�b�t�@���瑗�o
		std::lock_guard lock(primary.mutex);
		for (uint32_t c = 0; c < m_Format.channnels; ++c) {
			std::memcpy(&_dest[c].front(), &primary.buffer[c][m_Cuesor], sizeof(float) * sendFrames);
		}
		m_Cuesor += (uint32_t)sendFrames;
	}

	if (is_End) {
		if (m_EndCallback)m_EndCallback();
		Stop();
	}
	else if (remainFrames > 0) {
		//�c�o�b�t�@���Z�J���_���o�b�t�@���瑗�o
		auto& secondry = m_Tracks.at((m_UseTrack + 1) % static_cast<uint32_t>(ETrackNum::DOUBLE_MAX));

		TrackRequest request(*this, primary, primary.buffer.sizeX());
		RegisterTask(request);

		//�N���e�B�J���Z�N�V����(�Z�J���_���o�b�t�@)
		std::lock_guard lock(secondry.mutex);
		for (uint32_t c = 0; c < m_Format.channnels; ++c) {
			std::memcpy(&_dest[c][sendFrames], &secondry.buffer[c].front(), sizeof(float) * remainFrames);
		}
		m_Cuesor = (uint32_t)remainFrames;
		//�g�p�o�b�t�@�؂�ւ�
		m_UseTrack = (m_UseTrack + 1) % static_cast<uint32_t>(ETrackNum::DOUBLE_MAX);
	}

	//�G�t�F�N�g����(�o�b�t�@���o�������ɐݒ肵�Ă���Ƃ�)
	if (m_EffectTiming == EEffectTiming::AS_EFFECTTIMING_SENDBUFFER && !m_wpEffectManager.expired()) {
		if (auto effect = m_wpEffectManager.lock())
			effect->Execute(_dest, sendFrames + remainFrames);
	}
	else if (m_wpEffectManager.expired()) {
		m_wpEffectManager.reset();
	}

	//���ʒ���
	_dest.avx_mul(m_Volume);
	//���ʂ�1.0���瘨�����Ă����ƉE���Ƀm�C�Y����������H
	//�E�F�[�u�o���h���ϑ�����Ƃ���Ȋ����������B
	//���ʉ��Z�̕ύX��v����
	//->����Ȃ��ƂȂ������A�G�t�F�N�g���ꂽ�璴�m�C�Y

	return sendFrames + remainFrames;
}

//�v�����[�h(���O�o�b�t�@������)
void AS::SourceTrack::PreLoad() {
	m_Cuesor = 0;
	m_UseTrack = static_cast<uint32_t>(ETrackNum::PRIMARY);

	for (auto& track : m_Tracks) {
		track.buffer.zeroclear();
		track.fillingBuffer = 0;
		track.is_End = false;
	}

	auto& pri = m_Tracks.at(static_cast<size_t>(ETrackNum::PRIMARY));
	auto& sec = m_Tracks.at(static_cast<size_t>(ETrackNum::SECONDRY));

	{
		TrackRequest req(*this, pri, pri.buffer.sizeX());
		RegisterTask(req);
	}
	{
		TrackRequest req(*this, sec, sec.buffer.sizeX());
		RegisterTask(req);
	}
}

//Wave���烍�[�h
void AS::SourceTrack::Load(Track& _dest, size_t _loadFrames) {
	if (auto wav = m_Wave.lock()) {
		std::lock_guard lock(_dest.mutex);
		bool is_end = false, loop = m_Loop > 0 ? true : false;
		//�o�b�t�@�Q�b�`���I�I
		_dest.fillingBuffer = wav->GetBuffer(_dest.buffer, (uint32_t)_loadFrames, loop, is_end);

		//�I������
		if (is_end) {
			if (m_Loop > 0) {
				_dest.is_End = false;
				--m_Loop;
			}
			else if (m_Loop <= 0) {
				_dest.is_End = true;
			}
		}
	}
	else {
		Stop();
	}
}

void AS::SourceTrack::Bind(std::weak_ptr<WaveBase> _wave) {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_NONE) {
		return;
	}

	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY ||
		m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE) {
		this->Stop();
	}

	if (m_PlayState == EPlayState::AS_PLAYSTATE_STOP ||
		m_PlayState == EPlayState::AS_PLAYSTATE_UNBIND) {
		m_UseTrack = static_cast<uint32_t>(ETrackNum::PRIMARY);
		m_Wave = _wave;
		PreLoad();
		m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
	}
}

void AS::SourceTrack::Play(PlayOption& _option) {
	m_Loop = _option.loopCount;

	if (m_PlayState == EPlayState::AS_PLAYSTATE_STOP || m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE)
		m_PlayState = EPlayState::AS_PLAYSTATE_PLAY;
}

void AS::SourceTrack::PlayShot() {
	PlayOption shot(0);
	this->Play(shot);
}

void AS::SourceTrack::Pause() {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY)
		m_PlayState = EPlayState::AS_PLAYSTATE_PAUSE;
}

void AS::SourceTrack::Stop() {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY || m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE) {
		m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
		if (auto wav = m_Wave.lock()) {
			wav->Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, 0);
		}
		PreLoad();
	}
}
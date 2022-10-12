#ifndef _ASQTDEFINE_
#define _ASQTDEFINE_

#include <memory>
#include <vector>

#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

#include <AudioSystem.h>
#include <Wasapi.h>
#include <WavFile.h>
#include <OggFile.h>
#include <Reverb.h>
#include <Compressor.h>
#include <Equalizer.h>

#pragma comment(lib,"AudioSystem.lib")

struct SourcePlayer {
	//�\�[�X�g���b�N
	std::shared_ptr<AS::SourceTrack> source;
	//�����t�@�C��
	std::shared_ptr<AS::WaveBase> wav;
	//�G�t�F�N�g�Ǘ�
	std::shared_ptr<AS::EffectManager> effect;
	//���o�[�u�G�t�F�N�g
	std::weak_ptr<AS::Reverb> reverb;
	//�R���v���b�T�G�t�F�N�g
	std::weak_ptr<AS::Compressor> compressor;
	//EQ
	std::weak_ptr <AS::Equalizer> equalizer;
};
#endif
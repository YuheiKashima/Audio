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
	//ソーストラック
	std::shared_ptr<AS::SourceTrack> source;
	//音源ファイル
	std::shared_ptr<AS::WaveBase> wav;
	//エフェクト管理
	std::shared_ptr<AS::EffectManager> effect;
	//リバーブエフェクト
	std::weak_ptr<AS::Reverb> reverb;
	//コンプレッサエフェクト
	std::weak_ptr<AS::Compressor> compressor;
	//EQ
	std::weak_ptr <AS::Equalizer> equalizer;
};
#endif
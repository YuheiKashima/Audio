#include <iostream>
#include "Wasapi.h"
#include "AudioSystem.h"
#include "InstantInput.h"
#include <math.h>

#include "WavFile.h"
#include "Reverb.h"
#include "Compressor.h"
#include <random>

#define ENABLEEFFECT false

void StopCallBack() {
	std::cout << "Called Stop Callback Function" << std::endl;
}

using namespace AS;
namespace Render {
	class Test {
	public:
		void Main() {
			Init();
			PlayTest();
			Exit();
		}
	private:
		struct player {
			//ソーストラック
			std::shared_ptr<SourceTrack> source;
			//音源ファイル
			std::shared_ptr<WavFile> wav;
			//エフェクト管理
			std::shared_ptr<EffectManager> effect;
			//リバーブエフェクト
			std::weak_ptr<Reverb> reverb;
			//コンプレッサエフェクト
			std::weak_ptr<Compressor> compressor;
		};
		//入力
		InstantInput input;
		//メインシステム
		AudioSystem system;
		//サウンドデバイス一覧
		DeviceList list;
		//マスタトラック
		std::shared_ptr<MasterTrack> spMaster;
		std::vector<player> archive;

#ifdef _DEBUG
		std::vector<std::string> album{
			"../Media/Somehow_441.wav",
		};
#else
		std::vector<std::string> album{
			"Media/Somehow_441.wav",
		};
#endif
		void Init();
		void Exit() {
			system.Stop(EEndPointMode::AS_ENDPOINTMODE_RENDER);
		}

		void PlayTest();
		player makePlayer(std::string directory);
	};

	//デバイス起動
	void Test::Init() {
		system.EnumerateDevices<Wasapi>(EEndPointMode::AS_ENDPOINTMODE_RENDER, list);

		uint32_t selectDevice = 0;
		for (uint32_t i = 0; i < list.size(); ++i) {
			std::cout << i << " : " << list[i].deviceName << std::endl;
		}
		std::cout << ">" << std::flush;
		std::cin >> selectDevice;

		//フォーマット設定(2ch 16bit 48000Hz)
		AudioFormat alt;
		WasapiLunchInfo lunchInfo(list[selectDevice], AudioFormat(44100, 16, 2), AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, &alt);
		system.LunchDevice(lunchInfo);

		WasapiSetupInfo setup(0, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
		system.SetupDevice(EEndPointMode::AS_ENDPOINTMODE_RENDER, setup);

		spMaster = system.CreateMasterTrack();

		WasapiStartInfo start(spMaster, 2000);
		system.Start(EEndPointMode::AS_ENDPOINTMODE_RENDER, start);
	}

	//楽曲読み込み&エフェクト設定
	Test::player Test::makePlayer(std::string directory) {
		player play;
		play.wav = std::make_shared<WavFile>(directory, EBufferMode::WAVE_BUFFERMODE_STREAM);

#if ENABLEEFFECT
		//std::pairで受け取る
		auto [sou, eff] = system.CreateSourceTrackWithEffect(spMaster, 0, EEffectTiming::AS_EFFECTTIMING_SENDBUFFER);
		play.source = sou; play.effect = eff;
		play.source->Bind(play.wav);
		play.source->Volume(0.5f);
		play.reverb = play.effect->AddEffect<Reverb>();
		play.compressor = play.effect->AddEffect<Compressor>();

		//エフェクトパラメタ設定
		std::array<CombParam, 4> combParam;
		combParam[0] = CombParam(39.0f, 0.835603f);
		combParam[1] = CombParam(36.7f, 0.844501f);
		combParam[2] = CombParam(33.2f, 0.858223f);
		combParam[3] = CombParam(29.9f, 0.832147f);
		std::array<bool, 4> combEna{ true, true, true, true };
		std::array<float, 2> apfQ{ 5.05f ,1.65f };
		std::array<bool, 2> apfEna{ true, true };
		ReverbParam revParam(
			combParam,
			combEna,
			apfQ,
			apfEna,
			1.0f,
			0.25f);
		auto rev = play.reverb.lock();
		rev->SetEffectParam(revParam);

		CompressorParam compParam;
		compParam.threshold = 0.01f;
		compParam.ratio = 3.0f;
		compParam.gain = 1.0f;
		auto comp = play.compressor.lock();
		comp->SetEffectParam(compParam);

		play.source->SetEndingCallback(StopCallBack);
#else
		play.source = system.CreateSourceTrack(spMaster, 0);
		play.source->Bind(play.wav);
		play.source->Volume(0.5f);
#endif

		return play;
	}

	//メイン
	void Test::PlayTest() {
		for (auto& dir : album)
			archive.push_back(makePlayer(dir));

		//MainLoop
		while (true) {
			input.UpdateState();
			//Rshift + Esc プログラム終了
			if (GetKeyState(VK_RSHIFT) & 0x80 && GetKeyState(VK_ESCAPE) & 0x80) {
				break;
			}

			if (input.GetTrigger(VK_UP)) {
				for (auto p : archive) {
					float vol = p.source->Volume();
					vol += 0.1f;
					p.source->Volume(vol);
				}
				std::cout << "Vol up:" << archive[0].source->Volume() << std::endl;
			}
			else if (input.GetTrigger(VK_DOWN)) {
				for (auto p : archive) {
					float vol = p.source->Volume();
					vol -= 0.1f;
					p.source->Volume(vol);
				}
				std::cout << "Vol down:" << archive[0].source->Volume() << std::endl;
			}

			int32_t sel = -1;
			//Lshift + 1~5再生or一時停止中の楽曲を停止
			if (input.GetPress(VK_LSHIFT)) {
				if (input.GetTrigger('1')) sel = 0;
				else if (input.GetTrigger('2')) sel = 1;
				else if (input.GetTrigger('3')) sel = 2;
				else if (input.GetTrigger('4')) sel = 3;
				else if (input.GetTrigger('5')) sel = 4;

				if (sel > -1 && sel < archive.size()) {
					archive[sel].source->Stop();
					std::cout << "Track" << sel << "Stop" << std::endl;
				}
			}
			else {
				if (input.GetTrigger('1')) sel = 0;
				else if (input.GetTrigger('2')) sel = 1;
				else if (input.GetTrigger('3')) sel = 2;
				else if (input.GetTrigger('4')) sel = 3;
				else if (input.GetTrigger('5')) sel = 4;

				else if (input.GetTrigger('Q')) sel = 5;
				else if (input.GetTrigger('W')) sel = 6;
				else if (input.GetTrigger('E')) sel = 7;
				else if (input.GetTrigger('R')) sel = 8;
				else if (input.GetTrigger('T')) sel = 9;

				else if (input.GetTrigger('A'))sel = 10;
				else if (input.GetTrigger('S'))sel = 11;
				else if (input.GetTrigger('D'))sel = 12;
				else if (input.GetTrigger('F'))sel = 13;
				else if (input.GetTrigger('G'))sel = 14;

				//1~5 設定した楽曲を再生or一時停止
				PlayOption op(0);
				if (sel < archive.size() && sel >= 0 && sel <= 5) {
					std::cout << "Track" << sel << std::flush;
					auto state = archive[sel].source->GetState();
					if (state == EPlayState::AS_PLAYSTATE_STOP || state == EPlayState::AS_PLAYSTATE_PAUSE) {
						archive[sel].source->Play(op);
						std::cout << "Play" << std::endl;
					}
					else if (state == EPlayState::AS_PLAYSTATE_PLAY) {
						std::cout << "Pause" << std::endl;
						archive[sel].source->Pause();
					}
				}

#if ENABLEEFFECT
				//Q~T 1~5で設定されている楽曲のエフェクトOn or Off
				else if (sel >= 5 && sel <= 9) {
					sel -= 5;
					if (sel < archive.size()) {
						std::cout << "Track" << sel << "Effect:Reverb:" << std::flush;
						if (auto rev = archive[sel].reverb.lock())
							if (rev->GetEnable()) {
								rev->SetEnable(false);
								std::cout << "Diable" << std::endl;
							}
							else {
								rev->SetEnable(true);
								std::cout << "Enable" << std::endl;
							}
					}
				}
				//A~G 1~5で設定されている楽曲のエフェクトOn or Off
				else if (sel >= 10 && sel <= 14) {
					sel -= 10;
					if (sel < archive.size()) {
						std::cout << "Track" << sel << "Effect:Compressor:" << std::flush;
						if (auto rev = archive[sel].compressor.lock())
							if (rev->GetEnable()) {
								rev->SetEnable(false);
								std::cout << "Diable" << std::endl;
							}
							else {
								rev->SetEnable(true);
								std::cout << "Enable" << std::endl;
							}
					}
				}
#endif
			}
			input.Ready();
		}
	}

	//※試作のオマケ
#define MAX_Z 16
#define LENGTH 48000
	std::array<float, MAX_Z> z;
	std::array<float, MAX_Z> k;

	float pinkFilter(float in) {
		static float t = 0.0f;
		float q;

		q = in;
		for (int i = 0; i < MAX_Z; ++i) {
			z[i] = (q * k[i] + z[i] * (1.0 - k[i]));
			q = (q + z[i]) * 0.5f;
		}

		return (t = 0.75f * q + 0.25f * t);
	}

	void init_pink() {
		for (int i = 0; i < MAX_Z; i++)
			z[i] = 0;
		k[MAX_Z - 1] = 0.5;
		for (int i = MAX_Z - 1; i > 0; i--)
			k[i - 1] = k[i] * 0.25;
	}

	void testNoise() {
		init_pink();
		std::random_device seed_gen;
		std::mt19937_64 engine(seed_gen());

		WavFile wav("noise", AudioFormat(48000, 16, 2));
		LineBuffer<float> buf(2, LENGTH);
		float res = 0;
		for (int i = 0; i < LENGTH; ++i) {
			buf[0][i] = buf[1][i] = res = pinkFilter(engine() & 1 ? 1.0 : -1.0) * 1000.0f;
		}
		wav.Write(buf, LENGTH);
	}

	/*void testfft() {
		std::unique_ptr<OTFFT_Complex::complex_t[]> fft_comp(new OTFFT_Complex::complex_t[LENGTH]);
		testNoise();
		WavFile wav("../Media/noise.wav", EBufferMode::WAVE_BUFFERMODE_LOADALL);
		auto& linebuf = wav.GetTrack();

		for (uint32_t i = 0; i < LENGTH; ++i) {
			fft_comp[i].Re = linebuf[0][i];
		}

		OTFFT::FFT fft(LENGTH);
		fft.fwd(fft_comp.get());

		std::filesystem::create_directory("../FFT");
		std::ofstream file("../FFT/FFT_" + Chrono::GetTime_str() + ".csv", std::ios::out);
		for (uint32_t i = 0; i < LENGTH; ++i) {
			if (i == 0)file << "Real,Imag" << std::endl;
			file << fft_comp[i].Re << std::flush;
			file << "," << std::flush;
			file << fft_comp[i].Im << std::flush;
			file << std::endl;
			++i;
		}
	}*/
}

namespace Capture {
	class Test {
	public:
		void Main() {
			Init();
			Capturetest();
			Exit();
		}
	private:
		InstantInput input;
		AudioSystem system;
		DeviceList list;

		void Init();
		void Capturetest() {  }
		void Exit() {}
	};

	void Test::Init() {
		system.EnumerateDevices<Wasapi>(EEndPointMode::AS_ENDPOINTMODE_CAPTURE, list);
	}
}

//これでいける！！！
void sourceLocationTest(std::source_location _loc = std::source_location::current()) {
	std::cout << _loc.file_name() << std::endl;
	std::cout << _loc.function_name() << std::endl;
	std::cout << _loc.line() << std::endl;
	std::cout << _loc.column() << std::endl;
}

#define _RENDER_ true
int main(int argc, char* argv[]) {
#if _RENDER_
	Render::Test test;
#else
	Capture::Test test;
#endif
	test.Main();

	//boost::circular_buffer<float> cir(10);
	//for (auto i = 0; i < 5; ++i) {
	//	cir.push_back(i + 1);
	//}
	//std::cout << cir.size() << std::endl;
	//std::cout << cir.capacity() << std::endl;

	//cir.erase_begin(2);
	//std::cout << cir.front() << std::endl;

	/*AS::Wasapi render, capture;
	AudioFormat alt{};
	WasapiLunchInfo lunch{};
	lunch.lunchFormat = AudioFormat(48000, 16, 2);
	lunch.shareMode = AUDCLNT_SHAREMODE_SHARED;
	lunch.pAltFormat = &alt;

	lunch.lunchDevice = DeviceInfo("", Wasapi::GetAPIName(), EEndPointMode::AS_ENDPOINTMODE_RENDER);
	render.LunchDevice(lunch);
	lunch.lunchDevice.endpointMode = EEndPointMode::AS_ENDPOINTMODE_CAPTURE;
	capture.LunchDevice(lunch);*/

	return 0;
}
#include <iostream>
#include "Wasapi.h"
#include "AudioSystem.h"
#include "InstantInput.h"
#include <math.h>

#include "WavFile.h"
#include "OggFile.h"
#include "Reverb.h"
#include "Compressor.h"
#include <random>

#define ENABLEEFFECT true

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
			//�\�[�X�g���b�N
			std::shared_ptr<SourceTrack> source;
			//�����t�@�C��
			std::shared_ptr<OggFile> wav;
			//�G�t�F�N�g�Ǘ�
			std::shared_ptr<EffectManager> effect;
			//���o�[�u�G�t�F�N�g
			std::weak_ptr<Reverb> reverb;
			//�R���v���b�T�G�t�F�N�g
			std::weak_ptr<Compressor> compressor;
		};
		//����
		InstantInput input;
		//�T�E���h�f�o�C�X�ꗗ
		DeviceList list;
		//�}�X�^�g���b�N
		std::shared_ptr<MasterTrack> spMaster;
		std::vector<player> archive;

#ifdef _DEBUG
		std::vector<std::string> album{
			"../Media/Somehow_480.ogg",
		};
#else
		std::vector<std::string> album{
			"Media/Somehow_480.ogg",
		};
#endif
		void Init();
		void Exit() {
			AudioSystem::GetInstance().Stop(EEndPointMode::AS_ENDPOINTMODE_RENDER);
		}

		void PlayTest();
		player makePlayer(std::string directory);
	};

	//�f�o�C�X�N��
	void Test::Init() {
		AudioSystem::GetInstance().EnumerateDevices<Wasapi>(EEndPointMode::AS_ENDPOINTMODE_RENDER, list);

		uint32_t selectDevice = 0;
		for (uint32_t i = 0; i < list.size(); ++i) {
			std::cout << i << " : " << list[i].deviceName << std::endl;
		}
		std::cout << ">" << std::flush;
		std::cin >> selectDevice;

		//�t�H�[�}�b�g�ݒ�(2ch 16bit 48000Hz)
		AudioFormat alt;
		WasapiLaunchInfo LaunchInfo(list[selectDevice], AudioFormat(48000, 16, 2), AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, &alt);
		AudioSystem::GetInstance().LaunchDevice(LaunchInfo);

		WasapiSetupInfo setup(0, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
		AudioSystem::GetInstance().SetupDevice(EEndPointMode::AS_ENDPOINTMODE_RENDER, setup);

		spMaster = AudioSystem::GetInstance().CreateMasterTrack();

		WasapiStartInfo start(spMaster, 2000);
		AudioSystem::GetInstance().Start(EEndPointMode::AS_ENDPOINTMODE_RENDER, start);
	}

	//�y�ȓǂݍ���&�G�t�F�N�g�ݒ�
	Test::player Test::makePlayer(std::string directory) {
		player play;
		play.wav = std::make_shared<OggFile>(directory, EBufferMode::WAVE_BUFFERMODE_STREAM);

#if ENABLEEFFECT
		//std::pair�Ŏ󂯎��
		auto [sou, eff] = AudioSystem::GetInstance().CreateSourceTrackWithEffect(spMaster, 0, EEffectTiming::AS_EFFECTTIMING_SENDBUFFER);
		play.source = sou; play.effect = eff;
		play.source->Bind(play.wav);
		play.source->Volume(0.5f);
		play.reverb = play.effect->AddEffect<Reverb>();
		play.compressor = play.effect->AddEffect<Compressor>();

		//�G�t�F�N�g�p�����^�ݒ�
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
		play.source = AudioSystem::GetInstance().CreateSourceTrack(spMaster, 0);
		play.source->Bind(play.wav);
		play.source->Volume(0.5f);
#endif

		return play;
	}

	//���C��
	void Test::PlayTest() {
		for (auto& dir : album)
			archive.push_back(makePlayer(dir));

		//MainLoop
		while (true) {
			input.UpdateState();
			//Rshift + Esc �v���O�����I��
			if (GetKeyState(VK_RSHIFT) & 0x80 && GetKeyState(VK_ESCAPE) & 0x80) {
				break;
			}

			//����
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
			//Lshift + 1~5�Đ�or�ꎞ��~���̊y�Ȃ��~
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

				//1~5 �ݒ肵���y�Ȃ��Đ�or�ꎞ��~
				PlayOption op(LoopInfinity);
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

				if (input.GetTrigger('0')) {
					for (auto i = 0; auto & arc : archive) {
						std::cout << "Track" << i << "\tState:" << arc.source->GetStateStr() << std::endl;
					}
				}

#if ENABLEEFFECT
				//Q~T 1~5�Őݒ肳��Ă���y�Ȃ̃G�t�F�N�gOn or Off
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
				//A~G 1~5�Őݒ肳��Ă���y�Ȃ̃G�t�F�N�gOn or Off
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
			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}
	}

	//������̃I�}�P
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
		AudioSystem::GetInstance().EnumerateDevices<Wasapi>(EEndPointMode::AS_ENDPOINTMODE_CAPTURE, list);
	}
}

//����ł�����I�I�I
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

	//OggFile ogg("../Media/Somehow_480.ogg", AS::EBufferMode::WAVE_BUFFERMODE_LOADALL);

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
	WasapiLaunchInfo Launch{};
	Launch.LaunchFormat = AudioFormat(48000, 16, 2);
	Launch.shareMode = AUDCLNT_SHAREMODE_SHARED;
	Launch.pAltFormat = &alt;

	Launch.LaunchDevice = DeviceInfo("", Wasapi::GetAPIName(), EEndPointMode::AS_ENDPOINTMODE_RENDER);
	render.LaunchDevice(Launch);
	Launch.LaunchDevice.endpointMode = EEndPointMode::AS_ENDPOINTMODE_CAPTURE;
	capture.LaunchDevice(Launch);*/

	return 0;
}

// �w��T�C�Y��PCM�����o�b�t�@�𖄂߂�֐�
unsigned int getPCMBuffer(OggVorbis_File* ovf, char* buffer, int bufferSize, bool isLoop, bool* isEnd = 0) {
	if (buffer == 0) {
		if (isEnd) *isEnd = true;
		return 0;
	}

	if (isEnd) *isEnd = false;

	memset(buffer, 0, bufferSize);
	int requestSize = 4096;
	int bitstream = 0;
	int readSize = 0;
	int comSize = 0;
	bool isAdjust = false;

	if (bufferSize < requestSize) {
		requestSize = bufferSize;
		isAdjust = true;    // �����i�K
	}

	while (1) {
		readSize = ov_read(ovf, (char*)(buffer + comSize), requestSize, 0, 2, 1, &bitstream);
		if (readSize == 0) {
			// �t�@�C���G���h�ɒB����
			if (isLoop == true) {
				// ���[�v����ꍇ�ǂݍ��݈ʒu���ŏ��ɖ߂�
				ov_time_seek(ovf, 0.0);
			}
			else {
				// ���[�v���Ȃ��ꍇ�t�@�C���G���h�ɒB������I��
				if (isEnd) *isEnd = true;
				return comSize;
			}
		}

		comSize += readSize;

		if (comSize >= bufferSize) {
			// �o�b�t�@�𖄂ߐs�������̂ŏI��
			return comSize;
		}

		if (bufferSize - comSize < 4096) {
			isAdjust = true;    // �����i�K
			requestSize = bufferSize - comSize;
		}
	}

	return 0;    // �ǂ��킩��Ȃ��G���[
}
long ov_read(
	OggVorbis_File* vf,
	char* buffer,
	int             length,
	int             bigendianp,
	int             word,
	int             sgned,
	int* bitstream
);

//vf�ɂ̓I�[�v������Ogg�t�@�C�����w�肵�܂��B
//buffer�ɂ͉��f�[�^�iPCM�f�[�^�j���i�[����o�b�t�@���w�肵�܂��B�}�j���A���ɂ��ƓT�^�I�ɂ�4096�o�C�g�̃o�b�t�@���w�肷��悤�ł��B
//length�ɂ�buffer�̃T�C�Y���w�肵�܂��B
//bigendianp�ɂ͉��f�[�^�̊i�[�`���Ƃ��ă��g���G���f�B�A���i0�j���r�b�O�G���f�B�A���i1�j���w�肵�܂��BOgg Vorbis�̃��C�u������Windows�����ł͂Ȃ���Unix��Mac�Ȃǂł��g����悤�ɂȂ��Ă���̂ł��������t���O������܂��BWindows�̓��g���G���f�B�A���Ȃ̂Łuo�v���w�肳��܂��B
//word�ɂ͂�����uWORD�v�̃T�C�Y���t���O�Ŏw�肵�܂��B8bit����1�A16bit����2�ł��BWindows��WORD��16bit�Ȃ̂ł����ɂ́u2�v������܂��B
//sgned�ɂ�PCM�����̕����̗L�����t���O�w�肵�܂��B���������̏ꍇ��0�A�L���1�ł��B����PCM�͕����t���Ȃ̂Łu1�v�ł��B
//bitstream�ɂ̓X�g���[���Đ����̈ʒu���Ԃ�܂��B���͓��ɋC�ɂ��Ȃ��ŉ������B
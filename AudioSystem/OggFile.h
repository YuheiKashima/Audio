//#ifndef _OGGFILE_
//#define _OGGFILE_
//
//#include "WaveBase.h"
//#include <vorbis\codec.h>
//#include <vorbis\vorbisfile.h>
//
//#pragma comment(lib, "libogg.lib")
//#pragma comment(lib, "libvorbis_static.lib")
//#pragma comment(lib, "libvorbisfile_static.lib")
//
//namespace AS {
//	class OggFile : public WaveBase {
//		template<typename T>
//		friend class CStdPtrHelper;
//	public:
//		OggFile(std::string _directory, EBufferMode _mode) { Open(_directory, _mode); };
//		~OggFile() {  }
//	private:
//		bool Open(std::string _directory, EBufferMode _mode);
//		size_t GetStream(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd)override;
//		void SeekStream(const uint32_t _seek)override {}
//
//		OggVorbis_File m_OggFile;
//	};
//}
//#endif
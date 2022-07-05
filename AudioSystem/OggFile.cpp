//#include "OggFile.h"
//
//bool AS::OggFile::Open(std::string _directory, EBufferMode _mode) {
//	int res = ov_fopen(_directory.c_str(), &m_OggFile);
//	if (res != 0) {
//		switch (res) {
//		case OV_EREAD:       break;
//		case OV_ENOTVORBIS:  break;
//		case OV_EVERSION:    break;
//		case OV_EBADHEADER:  break;
//		case OV_EFAULT:      break;
//		default:             break;
//		}
//		return false;
//	}
//
//	vorbis_info* pOggInfo = ov_info(&m_OggFile, -1);
//
//	m_Format = AudioFormat(pOggInfo->rate, 16, pOggInfo->channels);
//}
//
//size_t AS::OggFile::GetStream(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd) {
//	return 0;
//}
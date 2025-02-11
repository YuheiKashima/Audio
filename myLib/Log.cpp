#include "Log.h"

using namespace myLib;
using namespace std;

ofstream Log::m_sLogStream;
function<void(const string)> Log::m_sLogOutputFunc;

/**
	@fn    Open
	@brief ���O�t�@�C�����쐬�A�L�^���J�n����
	@param _logFileName -Log_�쐬���O�t�@�C����_�쐬����.log
**/
void Log::Open(const string _logFileName) {
	m_sLogStream.close();

	filesystem::create_directory("Log");
	string fileName = format("Log/Log_{}_{}.log", _logFileName, TimeStamper::GetTime_str());
	ofstream ofFile(fileName, ios::out);
	if (!ofFile)throw runtime_error("Log file could not be created.");

	m_sLogStream = move(ofFile);
}
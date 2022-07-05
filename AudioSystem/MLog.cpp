#include "MLog.h"
std::ofstream myLib::Log::m_sLogStream;
uint32_t myLib::Log::m_sLevel = ELogLevel::ASLOG_NONE;
bool myLib::Log::m_sViewLogging = true;

myLib::Log::Log() {
}

myLib::Log::~Log() {
	m_sLogStream.close();
	m_sLevel = ELogLevel::ASLOG_NONE;
}

void myLib::Log::Open(bool _viewLogging, uint32_t _destlevel) {
	m_sLogStream.close();
	m_sLevel = ELogLevel::ASLOG_NONE;

	std::filesystem::create_directory("Log");
	std::ofstream file("Log/Log_" + Chrono::GetTime_str() + ".txt", std::ios::out);
	if (!file) return;

	m_sLevel = _destlevel;
	m_sLogStream = std::move(file);
	m_sViewLogging = _viewLogging;
}

void myLib::Log::Open(bool _viewLogging) {
	Open(_viewLogging, ELogLevel::ASLOG_ALL);
}

std::string myLib::Log::Logging(std::string _log, bool _viewHeader) {
	if (!m_sLogStream)return "";
	std::stringstream strstr;

	if (_viewHeader)
		strstr << "TimeStamp\t:" << Chrono::GetTime_str() << std::endl;
	strstr << _log << std::flush;

	if (m_sViewLogging) {
		std::cout << strstr.str();
	}
	m_sLogStream << strstr.str();
	return strstr.str();
}

std::string myLib::Log::Logging(Log::ELogLevel _level, std::string _log, bool _viewHeader) {
	std::stringstream output;

	if (_viewHeader) {
		output << "Level\t\t:";
		switch (_level) {
		case Log::ASLOG_TRACE:
			output << "Trace" << std::endl;
			break;
		case Log::ASLOG_DEBUG:
			output << "Debug" << std::endl;
			break;
		case Log::ASLOG_INFO:
			output << "Info" << std::endl;
			break;
		case Log::ASLOG_WARN:
			output << "Warning" << std::endl;
			break;
		case Log::ASLOG_ERROR:
			output << "Error" << std::endl;
			break;
		case Log::ASLOG_FATAL:
			output << "Fatal" << std::endl;
			break;
		}
	}

	output << _log;
	return myLib::Log::Logging(output.str(), _viewHeader);
}

std::string myLib::Log::Logging(Log::ELogLevel _level, std::string _comment, std::source_location _location, bool _viewHeader) {
	std::stringstream strstr;
	if (_viewHeader)
		strstr << "FileName\t:" << _location.file_name() << "\n" <<
		"FunctionName\t:" << _location.function_name() << "\n" <<
		"Line\t\t:" << _location.line() << std::endl;
	if (!_comment.empty())strstr << "Comment\t\t:" << _comment << std::flush;
	return myLib::Log::Logging(_level, strstr.str(), _viewHeader);
}
#include "MLog.h"
std::ofstream myLib::Log::m_sLogStream;
int32_t myLib::Log::m_sLevel = ELogLevel::ASLOG_NONE;
bool myLib::Log::m_sViewLogging = true;
std::function<void(std::string)> myLib::Log::m_sLogOutputCallback;

myLib::Log::Log() {
}

myLib::Log::~Log() {
	m_sLogStream.close();
	m_sLevel = ELogLevel::ASLOG_NONE;
}

void myLib::Log::Open(bool _viewLogging, int32_t _destlevel) {
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
		strstr << Chrono::GetTime_str();
	strstr << _log;

	if (m_sViewLogging) {
		std::cout << strstr.str() << std::endl;
	}
	if (m_sLogOutputCallback)
		m_sLogOutputCallback(strstr.str());
	m_sLogStream << strstr.str();
	m_sLogStream << std::endl;
	return strstr.str();
}

std::string myLib::Log::Logging(Log::ELogLevel _level, std::string _log, bool _viewHeader) {
	std::stringstream output;

	if (_viewHeader) {
		output << "[";
		switch (_level) {
		case Log::ASLOG_TRACE:
			output << "Trace]";
			break;
		case Log::ASLOG_DEBUG:
			output << "Debug]";
			break;
		case Log::ASLOG_INFO:
			output << "Info]";
			break;
		case Log::ASLOG_WARN:
			output << "Warning]";
			break;
		case Log::ASLOG_ERROR:
			output << "Error]";
			break;
		case Log::ASLOG_FATAL:
			output << "Fatal]";
			break;
		}
	}

	output << _log;
	return myLib::Log::Logging(output.str(), _viewHeader);
}

std::string myLib::Log::Logging(Log::ELogLevel _level, std::string _comment, std::source_location _location, bool _viewHeader) {
	std::stringstream strstr;
	if (_viewHeader) {
		auto fileDir = std::string(_location.file_name());
		auto pos = fileDir.find_last_of("\\");
		strstr << fileDir.substr(pos + 1) << "[" << _location.line() << "]" << std::endl;
	}
	if (!_comment.empty())strstr << _comment << std::endl;
	return myLib::Log::Logging(_level, strstr.str(), _viewHeader);
}
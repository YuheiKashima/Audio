#ifndef _MLOG_
#define _MLOG_

#include "MChrono.h"

using timestamp = std::chrono::system_clock::time_point;

namespace  myLib {
	class Log {
	public:
		Log();
		~Log();

		enum ELogLevel {
			ASLOG_NONE = 0x0,
			ASLOG_TRACE = 0x1,
			ASLOG_DEBUG = 0x2,
			ASLOG_INFO = 0x4,
			ASLOG_WARN = 0x8,
			ASLOG_ERROR = 0x10,
			ASLOG_FATAL = 0x20,
			ASLOG_ALL = 0x3F
		};

		static void Open(bool _viewLogging, uint32_t _destLevel);
		static void Open(bool _viewLogging);
		static std::string Logging(std::string _log, bool _viewHeader = true);
		static std::string Logging(Log::ELogLevel _level, std::string _log, bool _viewHeader = true);
		static std::string Logging(Log::ELogLevel _level, std::string _comment, std::source_location _location, bool _viewHeader = true);
		static void SetLogOutputCallback(std::function<void(std::string)> _logCallback) { m_sLogOutputCallback = _logCallback; }

		static bool is_Open() { return m_sLogStream.is_open(); }
	private:
		static std::ofstream m_sLogStream;
		static uint32_t m_sLevel;
		static bool m_sViewLogging;
		static std::function<void(std::string)> m_sLogOutputCallback;
	};
}
#endif // _CLOG_
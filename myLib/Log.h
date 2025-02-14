/**

	@file      Log.h
	@brief
	@details   ~
	@author    Yuhei Kashima
	@date      3.09.2024
	@copyright © Yuhei Kashima, 2024. All right reserved.

**/

#ifndef _LOG_
#define _LOG_

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <source_location>
#include <format>

#include "TimeStamp.h"

namespace myLib {
	/**

		@class   Log
		@brief
		@details ~ログ出力クラス

	**/
	class Log {
	public:
		Log() {}
		~Log() {
			m_sLogStream.close();
			m_sLogOutputFunc = nullptr;
		}

		enum class ELoggingLevel {
			LOGLV_NONE = 0x00,//使用しない
			LOGLV_TRACE = 0x01,
			LOGLV_DEBUG = 0x02,
			LOGLV_INFO = 0x04,
			LOGLV_WARN = 0x08,
			LOGLV_ERROR = 0x10,
			LOGLV_FATAL = 0x20,
			LOBLV_ALL = LOGLV_TRACE | LOGLV_DEBUG | LOGLV_INFO | LOGLV_WARN | LOGLV_ERROR | LOGLV_FATAL
		};

		/**
			@fn    Open
			@brief ログファイルを作成、記録を開始する
			@param _logFileName -Log_作成ログファイル名_作成日時.log
		**/
		static void Open(const std::string _logFileName);

		/**
			@fn     Logging
			@brief
			@tparam Args       - template parameter pack type
			@param  _log       -
			@param  _logFormat -
			@retval            -
		**/
		template <typename... Args>
		static constexpr std::string Logging(const std::format_string<Args...> _log, Args&&... _logFormat) {
			if (!m_sLogStream)return "";

			std::stringstream strstr;
			strstr << std::format("{}: {}", TimeStamper::GetTime_str(), std::format(_log, std::forward<Args>(_logFormat)...)) << std::endl;

			std::cout << strstr.str();

			if (m_sLogOutputFunc)
				m_sLogOutputFunc(strstr.str());

			m_sLogStream << strstr.str();

			return strstr.str();
		}

		/**
			@fn     Logging
			@brief
			@tparam Args       - template parameter pack type
			@param  _level     -
			@param  _log       -
			@param  _logFormat -
			@retval            -
		**/
		template <typename... Args>
		static constexpr std::string Logging(const ELoggingLevel _level, const std::format_string<Args...> _log, Args&&... _logFormat) {
			if (!m_sLogStream)return "";

			std::stringstream strstr;
			strstr << "[";
			switch (_level) {
			case ELoggingLevel::LOGLV_TRACE:
				strstr << "Trace]";
				break;
			case ELoggingLevel::LOGLV_DEBUG:
				strstr << "Debug]";
				break;
			case ELoggingLevel::LOGLV_INFO:
				strstr << "Info]";
				break;
			case ELoggingLevel::LOGLV_WARN:
				strstr << "Warning]";
				break;
			case ELoggingLevel::LOGLV_ERROR:
				strstr << "Error]";
				break;
			case ELoggingLevel::LOGLV_FATAL:
				strstr << "Fatal]";
				break;
			default:
				throw std::runtime_error("An ELoggingLevel outside the defined range is used.");
			}

			return Logging("{}{}", strstr.str(), std::format(_log, std::forward<Args>(_logFormat)...));
		}

		/**
			@fn     Logging
			@brief
			@tparam Args            - template parameter pack type
			@param  _level          -
			@param  _sourceLocation -
			@param  _log            -
			@param  _logFormat      -
			@retval                 -
		**/
		template <typename... Args>
		static constexpr std::string Logging(const ELoggingLevel _level, const std::source_location _sourceLocation, const std::format_string<Args...> _log, Args&&... _logFormat) {
			if (!m_sLogStream)return "";

			std::stringstream strstr;
			const std::string fileDir = std::format("{}", _sourceLocation.file_name());
			const int32_t pos = fileDir.find_last_of("\\");
			strstr << std::format(" {} [Locate {}:Line {}] ", fileDir.substr(pos + 1), _sourceLocation.function_name(), _sourceLocation.line());

			return Logging(_level, "{}{}", strstr.str(), std::format(_log, std::forward<Args>(_logFormat)...));
		}
		/**
			@fn     is_Open
			@brief
			@retval  -
		**/
		static bool is_Open() { return m_sLogStream.is_open(); }

		/**
			@fn		SetLogOutputCallback
			@brief	ログテキストをライブラリ外部に送る為のコールバック関数を設定する
			@param	_logCallbackFunc -ログコールバック関数 -void (const std::string)
		**/
		static void SetLogOutputCallback(std::function<void(const std::string)> _logCallbackFunc) { m_sLogOutputFunc = _logCallbackFunc; }
	private:
		static std::ofstream m_sLogStream;
		static std::function<void(const std::string)> m_sLogOutputFunc;
	};
}

#endif
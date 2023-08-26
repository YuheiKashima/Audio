#ifndef _MEXCEPTION_
#define _MEXCEPTION_

#include "Utilities.h"

namespace myLib {
	struct ErrorInfo {
		ErrorInfo(std::source_location _location, std::string _message)
			:location(_location),
			message(_message)
		{}
		ErrorInfo(std::string _message) :message(_message) {}
		std::source_location location;
		std::string message;
	};

	class MyExceptionBase :public std::exception {
	public:
		MyExceptionBase(const std::string& _message)
			:m_Message(_message),
			m_FileName(""),
			m_FunctionName(""),
			m_LineNumber(0),
			m_ErrorInfo(_message),
			m_IsErrorInfoExist(false)
		{}
		MyExceptionBase(const std::string& _message, const std::source_location& _sourceLocation)
			:m_Message(_message),
			m_FileName(_sourceLocation.file_name()),
			m_FunctionName(_sourceLocation.function_name()),
			m_LineNumber(_sourceLocation.line()),
			m_ErrorInfo(_sourceLocation, _message),
			m_IsErrorInfoExist(true)
		{};
		virtual ~MyExceptionBase() {}

		constexpr std::string GetFilePath() { return m_FileName; }
		constexpr std::string GetFuntionName() { return m_FunctionName; }
		constexpr int32_t GetLineNumber() { return m_LineNumber; }
		const ErrorInfo GetErrorInfo() { return m_ErrorInfo; }

		virtual char const* what()const noexcept override {
			if (m_IsErrorInfoExist) {
			}
			else {
			}
		}
	protected:
		std::string m_Message;
		std::string m_FileName;
		std::string m_FunctionName;
		int32_t m_LineNumber;
		ErrorInfo m_ErrorInfo;
		bool m_IsErrorInfoExist;
	};
}

#endif
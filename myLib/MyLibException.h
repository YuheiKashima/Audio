/**

	@file      MyLibException.h
	@brief
	@details   ~
	@author    Yuhei Kashima
	@date      4.09.2024
	@copyright © Yuhei Kashima, 2024. All right reserved.

**/
#ifndef _MYLIBEXCEPTION_
#define _MYLIBEXCEPTION_

#include <exception>

#include "Log.h"

#ifdef _WIN32

namespace myLib {
	/**

		@class   MyLibException
		@brief
		@details ~

	**/
	class MyLibException :public std::exception {
	public:

		/**
			@fn     MyLibException
			@brief  MyLibException object constructor
			@tparam Args              - template parameter pack type
			@param  _level            -
			@param  _expComment       -
			@param  _expCommentFormat -
			@param  _sourceLocation   -
		**/
		template <typename... Args>
		MyLibException(const Log::ELoggingLevel _level, const std::source_location _sourceLocation, const std::format_string<Args...> _expComment, Args&&... _expCommentFormat)
			:m_ExpLevel(_level), m_ExpLoc(_sourceLocation) {
			m_what = Log::Logging(_level, _sourceLocation, _expComment, std::forward<Args>(_expCommentFormat)...);
		}

		/**
			@fn     what
			@brief
			@retval  -
		**/
		const char* what() const throw() override {
			return m_what.c_str();
		}

	protected:
		const Log::ELoggingLevel m_ExpLevel;
		const std::source_location m_ExpLoc;
		std::string m_Comment;

		std::string m_what;
	};
};
#endif
#endif
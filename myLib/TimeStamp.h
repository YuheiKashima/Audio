#ifndef _TIMESTAMP_
#define _TIMESTAMP_

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <format>

namespace myLib {
	using timestamp = std::chrono::system_clock::time_point;
	using milliseconds = std::chrono::milliseconds;
	using microseconds = std::chrono::microseconds;
	using nanoseconds = std::chrono::nanoseconds;

	class TimeStamper {
	public:
		TimeStamper();
		~TimeStamper();

		/**
			@fn		GetTime
			@brief
			@retval
		**/
		static const timestamp GetTime();
		/**
			@fn     GetTime_str
			@brief
			@retval  -
		**/
		static const std::string GetTime_str();

		/**
			@fn     GetDuration
			@brief
			@tparam DURATION -
			@param  _start   -
			@param  _end     -
			@retval          -
		**/
		template<typename DURATION>
		static const double GetDuration(const timestamp _start, const timestamp _end) {
			if (typeid(DURATION) == typeid(milliseconds) || typeid(DURATION) == typeid(microseconds) || typeid(DURATION) == typeid(nanoseconds)) {
				return static_cast<double>(std::chrono::duration_cast<DURATION>(_end - _start).count());
			}
			else {
				return static_cast<double>(-1.0);
			}
		}
	};
};

#endif
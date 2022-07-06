#ifndef _MCHRONO_
#define _MCHRONO_

#include "Utilities.h"

using timestamp = std::chrono::system_clock::time_point;
using milliseconds = std::chrono::milliseconds;
using microseconds = std::chrono::microseconds;
using nanoseconds = std::chrono::nanoseconds;
namespace myLib {
	class Chrono {
	public:
		Chrono() {};
		~Chrono() {};

		static timestamp GetTime() {
			return std::chrono::system_clock::now();
		}

		static std::string GetTime_str() {
			auto now = std::chrono::system_clock::now();
			auto now_c = std::chrono::system_clock::to_time_t(now);
			std::stringstream ss;
			tm localTime;
			localtime_s(&localTime, &now_c);
			ss << std::put_time(&localTime, "%Y%m%d_%H%M%S");
			return ss.str();
		}

		template<typename SEC>
		static double GetDuration(timestamp _start, timestamp _end) {
			return static_cast<double>(std::chrono::duration_cast<SEC>(_end - _start).count());
		}
	};
}
#endif
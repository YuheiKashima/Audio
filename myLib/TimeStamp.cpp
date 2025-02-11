#include "TimeStamp.h"

using namespace myLib;
using namespace std;

TimeStamper::TimeStamper() {
}

TimeStamper::~TimeStamper() {
}

const timestamp TimeStamper::GetTime() {
	return chrono::system_clock::now();
}

const string TimeStamper::GetTime_str() {
	const auto now = chrono::system_clock::now();
	const auto now_c = chrono::system_clock::to_time_t(now);

	tm tm;
	stringstream strstr;
	localtime_s(&tm, &now_c);
	strstr << put_time(&tm, "%Y%m%d_%H%M%S");

	return format("{}", strstr.str());
}
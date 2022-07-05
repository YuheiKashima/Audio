#ifndef _UTITLITES_
#define _UTILITIES_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <map>
#include <chrono>
#include <cassert>
#include <functional>
#include <queue>
#include <sstream>
#include <exception>
#include <source_location>
#include <limits>
#include <filesystem>
#include <typeinfo>
#include <codecvt>
#include <locale>
#include <utility>
#include <algorithm>
#include <complex>
#include <numbers>
#include <bit>
#include <future>
#include <atomic>

//Locale
#include <locale>

#define _USE_MATH_DEFINES
#include <math.h>
//AVX
#include <immintrin.h>
#define AVX_ALIEN_RANGE 32

//Boost
//#include <boost\thread\synchronized_value.hpp>

size_t calcAlienRange(size_t _range);
std::wstring Convert_Str_to_Wstr(std::string const& _src);
std::string Convert_Wstr_to_Str(std::wstring const& _src);

//NameofLib
//#include <nameof.hpp>

#endif
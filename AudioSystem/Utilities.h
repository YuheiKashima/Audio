#ifndef _UTITLITES_
#define _UTILITIES_

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <bitset>
#include <cassert>
#include <chrono>
#include <codecvt>
#include <complex>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <numbers>
#include <numeric>
#include <optional>
#include <queue>
#include <shared_mutex>
#include <source_location>
#include <sstream>
#include <string>
#include <thread>
#include <typeinfo>
#include <utility>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

//AVX
#include <immintrin.h>
#define AVX_ALIEN_RANGE 32

//Boost
#include <boost\circular_buffer.hpp>
#include <boost\timer\timer.hpp>
#include <boost\config.hpp>

size_t calcAlienRange(size_t _range);
std::wstring Convert_Str_to_Wstr(std::string const& _src);
std::string Convert_Wstr_to_Str(std::wstring const& _src);
#endif
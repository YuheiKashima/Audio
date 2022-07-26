#include "CPUSupportChecker.h"

// Initialize static member data
const myLib::CPUSupportChecker::CPUSupportChecker_Internal myLib::CPUSupportChecker::CPU_Rep;

std::string myLib::CPUSupportChecker::OutputCPUSupports() {
	std::stringstream outstream;

	auto support_message = [&outstream](std::string isa_feature, bool is_supported) {
		outstream << isa_feature << (is_supported ? " supported" : " not supported") << std::endl;
	};

	outstream << CPUSupportChecker::Vendor() << std::endl;
	outstream << CPUSupportChecker::Brand() << std::endl;

	support_message("3DNOW", CPUSupportChecker::_3DNOW());
	support_message("3DNOWEXT", CPUSupportChecker::_3DNOWEXT());
	support_message("ABM", CPUSupportChecker::ABM());
	support_message("ADX", CPUSupportChecker::ADX());
	support_message("AES", CPUSupportChecker::AES());
	support_message("AVX", CPUSupportChecker::AVX());
	support_message("AVX2", CPUSupportChecker::AVX2());
	support_message("AVX512CD", CPUSupportChecker::AVX512CD());
	support_message("AVX512ER", CPUSupportChecker::AVX512ER());
	support_message("AVX512F", CPUSupportChecker::AVX512F());
	support_message("AVX512PF", CPUSupportChecker::AVX512PF());
	support_message("BMI1", CPUSupportChecker::BMI1());
	support_message("BMI2", CPUSupportChecker::BMI2());
	support_message("CLFSH", CPUSupportChecker::CLFSH());
	support_message("CMPXCHG16B", CPUSupportChecker::CMPXCHG16B());
	support_message("CX8", CPUSupportChecker::CX8());
	support_message("ERMS", CPUSupportChecker::ERMS());
	support_message("F16C", CPUSupportChecker::F16C());
	support_message("FMA", CPUSupportChecker::FMA());
	support_message("FSGSBASE", CPUSupportChecker::FSGSBASE());
	support_message("FXSR", CPUSupportChecker::FXSR());
	support_message("HLE", CPUSupportChecker::HLE());
	support_message("INVPCID", CPUSupportChecker::INVPCID());
	support_message("LAHF", CPUSupportChecker::LAHF());
	support_message("LZCNT", CPUSupportChecker::LZCNT());
	support_message("MMX", CPUSupportChecker::MMX());
	support_message("MMXEXT", CPUSupportChecker::MMXEXT());
	support_message("MONITOR", CPUSupportChecker::MONITOR());
	support_message("MOVBE", CPUSupportChecker::MOVBE());
	support_message("MSR", CPUSupportChecker::MSR());
	support_message("OSXSAVE", CPUSupportChecker::OSXSAVE());
	support_message("PCLMULQDQ", CPUSupportChecker::PCLMULQDQ());
	support_message("POPCNT", CPUSupportChecker::POPCNT());
	support_message("PREFETCHWT1", CPUSupportChecker::PREFETCHWT1());
	support_message("RDRAND", CPUSupportChecker::RDRAND());
	support_message("RDSEED", CPUSupportChecker::RDSEED());
	support_message("RDTSCP", CPUSupportChecker::RDTSCP());
	support_message("RTM", CPUSupportChecker::RTM());
	support_message("SEP", CPUSupportChecker::SEP());
	support_message("SHA", CPUSupportChecker::SHA());
	support_message("SSE", CPUSupportChecker::SSE());
	support_message("SSE2", CPUSupportChecker::SSE2());
	support_message("SSE3", CPUSupportChecker::SSE3());
	support_message("SSE4.1", CPUSupportChecker::SSE41());
	support_message("SSE4.2", CPUSupportChecker::SSE42());
	support_message("SSE4a", CPUSupportChecker::SSE4a());
	support_message("SSSE3", CPUSupportChecker::SSSE3());
	support_message("SYSCALL", CPUSupportChecker::SYSCALL());
	support_message("TBM", CPUSupportChecker::TBM());
	support_message("XOP", CPUSupportChecker::XOP());
	support_message("XSAVE", CPUSupportChecker::XSAVE());

	return outstream.str();
}
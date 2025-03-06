/**

	@file      ThreadTaskShared.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      3.03.2025

**/
#ifndef _THREADTASKSHARED_
#define _THREADTASKSHARED_

#include "ThreadTask.h"

namespace myLib {
	/**
		@class   ThreadTaskShared
		@brief
		@details ~
	**/
	class ThreadTaskShared :public ThreadTask {
	public:
		ThreadTaskShared() :ThreadTask(ThreadTaskType::Shared) {}
		~ThreadTaskShared() {}
	};
}

#endif
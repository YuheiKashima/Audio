/**

	@file      ThreadExclusiveTask.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      28.02.2025

**/

#ifndef _THREADTASKEXCLUSIVE_
#define _THREADTASKEXCLUSIVE_

#include "ThreadTask.h"

namespace myLib {
	/**

		@class   ThreadTaskExclusive
		@brief
		@details ~

	**/
	class ThreadTaskExclusive :public ThreadTask {
	public:
		ThreadTaskExclusive() :ThreadTask(ThreadTaskType::Exclusive) {}
		~ThreadTaskExclusive() {}
	};
}

#endif // !_THREADTASKEXCLUSIVE_

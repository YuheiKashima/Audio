/**

	@file      ThreadTask.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      28.02.2025

**/

#ifndef _THREADTASK_
#define _THREADTASK_

#include "ThreadPool.h"

namespace myLib {
	/**
		@enum  myLib::ThreadTaskType
		@brief
	**/
	enum class ThreadTaskType {
		None,
		Standard,
		Shared,
		Exclusive
	};

	/**

		@class   ThreadTask
		@brief
		@details ~

	**/
	class ThreadTask {
	public:
		ThreadTask() :m_Type(ThreadTaskType::Standard) {}
		~ThreadTask() {}

		const ThreadTaskType GetType() const { return m_Type; }

		virtual void Execute() = 0;

	protected:
		ThreadTask(ThreadTaskType type) :m_Type(type) {}

	private:
		const ThreadTaskType m_Type = ThreadTaskType::None;
	};
}

#endif // !_THREADTASK_
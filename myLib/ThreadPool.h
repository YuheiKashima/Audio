/**

	@file      ThreadPool.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      27.02.2025

**/

#ifndef _THREADPOOL_
#define _THREADPOOL_

#include "ThreadDefine.h"
#include "ThreadTaskExclusive.h"
#include "ThreadTask.h"

namespace myLib {
	/**

		@class   ThreadPool
		@brief
		@details ~
		@tparam  CntThread -

	**/
	template<size_t CntThread>
	class ThreadPool {
	public:

	private:
		class WorkThread {
		public:
			WorkThread() :m_Thread([this] {WorkThread(); }) {}
			~WorkThread() {
			}

		private:
			void WaitForInitialize() {
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_Condition.wait(lock, [this]() {return !m_pParent.has_value() :m_Index >= 0; })
			}

			void WorkThread() {
				WaitForInitialize();

				while (true) {
				}
			}

			bool m_isTermination = false;
			int32_t m_Index = -1;
			std::optional<ThreadPool*> m_pParent;
			std::thread m_Thread;
			std::mutex m_Mutex;
			std::condition_variable m_Condition;
			std::deque<std::weak_ptr<ThreadTask>> m_LocalTaskQueue;
		};

		std::array<WorkThread, CntThread> m_WorkThreads;
	};
}

#endif // !_THREADPOOL_
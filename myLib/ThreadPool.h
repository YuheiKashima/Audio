/**

	@file      ThreadPool.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      27.02.2025

**/

#ifndef _THREADPOOL_
#define _THREADPOOL_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <optional>
#include <memory>

namespace myLib {
	/**

		@class   ThreadTask
		@brief
		@details ~

	**/
	class ThreadTask {
	public:
		ThreadTask() {}
		~ThreadTask() {}

		virtual void Execute() = 0;

	protected:

	private:
	};

	/**

		@class   ThreadPool
		@brief
		@details ~
		@tparam  CntThread -

	**/
	class ThreadPool {
	public:

	private:
		class inPoolThread {
		public:
			inPoolThread();
			~inPoolThread();

			void Initialization(int32_t _index, ThreadPool* _pParent);
			void WakeUp();
			void WaitForIdle();
			void Termination();
		private:
			void WorkThreadFunc();
			void WaitForInitialization();

			bool m_isTermination = false;
			int32_t m_Index = -1;
			std::optional<ThreadPool*> m_pParent;
			std::thread m_Thread;
			std::mutex m_Mutex;
			std::condition_variable m_Condition;
			std::deque<std::weak_ptr<ThreadTask>> m_LocalTaskQueue;
		};
		std::vector<inPoolThread> m_Threads;
	};
}

#endif // !_THREADPOOL
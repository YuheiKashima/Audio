#include "ThreadPool.h"

using namespace myLib;
using namespace std;

ThreadPool::inPoolThread::inPoolThread() : m_Thread(&inPoolThread::WorkThreadFunc, this) {}

ThreadPool::inPoolThread::~inPoolThread() {
	WaitForIdle();
	Termination();
	if (m_Thread.joinable())
		m_Thread.join();
}

void ThreadPool::inPoolThread::WorkThreadFunc() {
	WaitForInitialization();

	while (true) {
		unique_lock<mutex> lock(m_Mutex);

		m_Condition.wait(lock, [this]() {return !m_LocalTaskQueue.empty() || m_isTermination; });

		if (m_isTermination)break;
	}
}

void ThreadPool::inPoolThread::WaitForInitialization() {
	unique_lock<mutex> lock(m_Mutex);
	m_Condition.wait(lock, [this]() {return m_pParent.has_value() && m_Index >= 0; });
}

void ThreadPool::inPoolThread::Initialization(int32_t _index, ThreadPool* _pParent) {
	unique_lock<mutex> lock(m_Mutex);
	m_Index = _index;
	m_pParent = _pParent;
	m_Condition.notify_all();
}

void ThreadPool::inPoolThread::WakeUp() {
	unique_lock<mutex> lock(m_Mutex);
	m_Condition.notify_all();
}

void ThreadPool::inPoolThread::WaitForIdle() {
	unique_lock<mutex> lock(m_Mutex);
	m_Condition.wait(lock, [this]() {return m_LocalTaskQueue.empty() || m_isTermination; });
}

void ThreadPool::inPoolThread::Termination() {
	unique_lock<mutex> lock(m_Mutex);
	m_isTermination = true;
	m_Condition.notify_all();
}
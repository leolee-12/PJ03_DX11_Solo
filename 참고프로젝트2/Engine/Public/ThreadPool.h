#pragma once

#include "Base.h"
#include <thread>
#include <mutex>

BEGIN(Engine)

class ENGINE_DLL CThreadPool final : public CBase
{
public:
	HRESULT Initialize(size_t ThreadCount);


	void Shutdown();
	_uint Get_ThreadNumber() { return m_ThreadCount; };

	template<typename FunctionType>
	future<typename result_of<FunctionType()>::type> EnqueueTask(FunctionType task);

private:
	CThreadPool();
	virtual ~CThreadPool() = default;
	void WorkerThread();

private:
	vector<thread> m_Workers; //워커 스레드 모음
	queue<function<void()>> m_Tasks; //작업 큐
	mutex m_QueueMutex; //큐 보호를 위한 뮤텍스
	condition_variable m_Condition; //스레드 동기화를 위한 변수
	_bool m_bStop = { false }; //스레드 풀 종료 플래그
	size_t m_ThreadCount = 1; //스레드 개수

public:
	static CThreadPool* Create();
	virtual void Free() override;
};

//여러 함수 타입을 모두 처리하기 위해 '템플릿'으로 만든다.
// 템플릿 함수의 정의는 헤더 파일에 포함되어야 합니다.
template<typename FunctionType>
future<typename result_of<FunctionType()>::type> CThreadPool::EnqueueTask(FunctionType task)
{
	using return_type = typename result_of<FunctionType()>::type;

	auto packagedTask = make_shared<packaged_task<return_type()>>(task);
	future<return_type> fut = packagedTask->get_future();

	{
		unique_lock<mutex> lock(m_QueueMutex);
		if (m_bStop)
			throw runtime_error("ThreadPool is stopped.");

		m_Tasks.emplace([packagedTask]() { (*packagedTask)(); });
	}
	m_Condition.notify_one();
	return fut;
}


END
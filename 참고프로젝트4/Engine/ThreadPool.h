#pragma once

#include "Base.h"


BEGIN(Engine)

/// <summary>
/// 쓰레드 처리를 위해 생성한 쓰레드 풀
/// 비동기 처리보다 더 체계화된 쓰레드 작업을 하려고 제작하였다.
/// 지만 그냥 넣고 싶어서 넣은거다.
/// </summary>
class ENGINE_DLL CThreadPool : public CBase
{
    INFO_CLASS(CThreadPool, CBase)

private:
    explicit CThreadPool();
    explicit CThreadPool(const CThreadPool& rhs);
    virtual ~CThreadPool();

public:
    HRESULT Initialize(_uint iNumThreads, _bool bAutoAllocateNum);

public:
    static CThreadPool* Create(_uint iNumThreads, _bool bAutoAllocateNum);
    virtual void Free() override;
    
public:
    // job 을 추가한다. 일을 큐에 등록하는 함수.
    template <class F, class... Args>
    auto EnqueueJob(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

private:
    // Worker 쓰레드
    void WorkerThread();

private:
    _uint                   m_iNumThreads = { 0 };      // 총 Worker 쓰레드의 개수.
    vector<thread>          m_WorkderThreads;           // Worker 쓰레드를 보관하는 벡터.
    queue<function<void()>> m_Jobs;                     // 할일들을 보관하는 job 큐.
    condition_variable      m_JobQ_CV;                  // 위의 job 큐를 위한 컨디션 밸류
    mutex                   m_JobQ_Mutex;               // 잡큐 뮤텍스
    _bool                   m_bStopAll = { false };     // 모든 쓰레드 강제 종료

};


template <class F, class... Args>
auto CThreadPool::EnqueueJob(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;

    if (m_bStopAll) {
        throw std::runtime_error("ThreadPool 사용 중지됨");
    }

    auto job = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> job_result_future = job->get_future();
    {
        std::lock_guard<std::mutex> lock(m_JobQ_Mutex);
        m_Jobs.push([job]() { (*job)(); });
    }

    m_JobQ_CV.notify_one();

    return job_result_future;
}

END
#include "..\Public\ThreadPool.h"


CThreadPool::CThreadPool()
{

}

HRESULT CThreadPool::Initialize(size_t ThreadCount)
{
    m_ThreadCount = ThreadCount;

    try
    {
        for (size_t i = 0; i < ThreadCount; ++i)
        {
            m_Workers.emplace_back(thread(&CThreadPool::WorkerThread, this));
        }
    }
    catch (const exception& e)
    {
        return E_FAIL;
    }
    return S_OK;
}

//각 워커 스레드가 실행하는 함수입니다. 작업 큐에서 작업을 가져와서 실행합니다.
void CThreadPool::WorkerThread()
{
    while (true)
    {
        function<void()> task;

        {
            // 작업 큐에 안전하게 접근하기 위해 뮤텍스를 잠급니다.
            unique_lock<mutex> lock(m_QueueMutex);

            //- 조건 변수 `m_Condition`을 사용하여 작업이 추가되거나 스레드풀 종료 신호(`m_bStop`)가 올 때까지 대기합니다.
            //- 조건 : `m_bStop`이 `true`이거나 `m_Tasks`에 작업이 있을 때까지 대기합니다.
            m_Condition.wait(lock, [this] { return m_bStop || !m_Tasks.empty(); });

            //스레드풀 종료 신호(m_bStop이 true)이고, 처리할 작업도 없다면 스레드를 종료합니다.
            if (m_bStop && m_Tasks.empty())
                return;

            //작업 큐에서 가장 앞에 있는 작업을 가져와 task에 저장하고, 큐에서 제거합니다.
            task = move(m_Tasks.front());
            m_Tasks.pop();
        }

        //가져온 작업을 실행합니다.
        task();
    }
}


//스레드풀을 종료하고 모든 스레드를 정리합니다.
void CThreadPool::Shutdown()
{
    {
        //작업 큐를 보호하기 위해 뮤텍스를 잠그고, 스레드풀 종료 신호를 true로 설정합니다.
        unique_lock<mutex> lock(m_QueueMutex);
        m_bStop = true;
    }
    //대기 중인 모든 스레드에게 신호를 보내서 깨웁니다. 스레드들은 m_bStop을 확인하고 종료 절차를 밟습니다.
    m_Condition.notify_all();

    for (thread& worker : m_Workers)
    {
        //모든 워커 스레드가 종료될 때까지 기다립니다.
        //`join()`은 스레드가 종료될 때까지 대기하는 함수입니다


        //작업 중인 직원 : 어떤 직원이 일을 하고 있다면, 그 직원은 일을 마칠 때까지 일을 계속합니다.
        //회사 문 닫기 : 회사가 문을 닫는다는 공지가 내려오면, 아직 일을 시작하지 않은 직원들은 바로 퇴근합니다.
        //일을 마친 직원 : 일을 마친 직원은 더 이상 새로운 일을 받지 않고 퇴근합니다.
        //모든 직원이 퇴근할 때까지 기다림 : 회사는 모든 직원이 안전하게 퇴근할 때까지 기다립니다.
        if (worker.joinable())
            worker.join();
    }
}

CThreadPool* CThreadPool::Create()
{
	CThreadPool* pInstance = new CThreadPool();

	return pInstance;
}

void CThreadPool::Free()
{
	__super::Free();
}

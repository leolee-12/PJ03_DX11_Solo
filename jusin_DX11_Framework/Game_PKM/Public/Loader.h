#pragma once
#include "Game_PKM_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Game_PKM)

class CLoader final : public CBase
{
public:
	using TaskFunc = std::function<HRESULT()>;

	struct LOAD_TASK
	{
		_wstring strDebugName = {};
		TaskFunc fn;
	};

private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	Concurrency::concurrent_queue<LOAD_TASK>* Get_TaskQueue() { return &m_TaskQueue; }

	HRESULT	Initialize(LEVEL eNextLevelID);
	_bool Is_Finished() const { return m_iCompletedCount.load() == m_iTotalCount; }
	_bool Has_Error() const { return m_bHasError.load(); }
	void Set_Error(_bool b) { m_bHasError.store(b); }
	_float Get_Progress() const { return m_iTotalCount ? static_cast<_float>(m_iCompletedCount) / m_iTotalCount : 0.f; }
	void Add_Progress() { m_iCompletedCount.fetch_add(1, memory_order_relaxed); }

	void Set_ErrorTask(const _wstring& strTaskName);
	_wstring Get_LastErrorTask() const;

#ifdef _DEBUG
public:
	void Show();
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
	LEVEL					m_eNextLevelID = { LEVEL::END };

	Concurrency::concurrent_queue<LOAD_TASK> m_TaskQueue;
	mutable std::mutex m_ErrorMutex;
	_wstring m_strLastErrorTask = {};

	_uint					m_iTotalCount = { 0 };		// 메인만 쓰므로 일반 _uint OK
	std::atomic<_uint>		m_iCompletedCount = { 0 };
	std::atomic<bool>		m_bHasError = { false };	// 실패 시그널
	std::vector<HANDLE>		m_Threads;

private:
	void Enqueue_LoadTask(TaskFunc fn, const _tchar* pDebugName = TEXT("Load Task"));

	template<typename Factory>
	void Enqueue_Prototype(_uint iLevelIndex, WNameID strProtoTag, Factory FactoryFunc, const _tchar* pDebugName = TEXT("Load Task"));
	
	void Enqueue_All(LEVEL eNextLevelID);
	HRESULT Ready_Resources_For_Logo();
	HRESULT Ready_Resources_For_GamePlay();
	HRESULT Ready_Resources_For_Battle();
	HRESULT Ready_Resources_For_Capture();
	HRESULT Ready_Resources_For_Effect();

public:
	static CLoader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);

protected:
	virtual void	Free() override;
};

NS_END

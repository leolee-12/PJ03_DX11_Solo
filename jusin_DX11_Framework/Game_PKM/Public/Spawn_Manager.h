#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "Spawn_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
class CNavigation;
NS_END

NS_BEGIN(Game_PKM)

/* -----------------------------------------------------------
   CSpawn_Manager - 사각형(SpawnRect) 등록/검증/스폰 진행 관리

	 호출 순서:
	   1) Initialize(...)         : 레벨 진입 시 1회. NavMesh 프로토타입 클론 + 내부 상태 wipe.
									(재진입 시 자동 wipe - 내부에서 Clear() 선행 호출.)
	   2) Register_SpawnRect(...) : Begin() 이전에만 유효. Begin 이후 호출은 실패(E_FAIL).
	   3) Begin()                 : 등록한 모든 사각형 검증. bSpawnOnLoad 사각형은 즉시 1회 스폰.
	   4) Update(fTimeDelta)      : 매 프레임. RespawnTimer 진행 + 조건 충족 시 스폰.

	 라이프사이클:
	   - DECLARE_SINGLETON. 앱 종료 시 CSpawn_Manager::DestroyInstance() 호출
		 (Game_API::Cleanup_StaticTables 권장). 본 호출이 Release->Free->Clear 체인으로
		 NaviClone 까지 모두 해제하므로 별도 Clear() 명시 호출은 필요하지 않다.
	   - 주의: Level_GamePlay::Free() 등 종료 경로에서 Clear() 를 GetInstance() 경유로
		 호출하지 말 것. Cleanup_StaticTables -> DestroyInstance 가 먼저 실행되어
		 m_pInstance == nullptr 인 상태에서 GetInstance() 가 새 인스턴스를 lazy-create
		 해 leak 이 발생한다.
	   - 명시적 reset 이 필요하면 Initialize() 재호출이 정답 (내부에서 Clear() 선행).
   ----------------------------------------------------------- */

class CSpawn_Manager final : public CBase
{
	DECLARE_SINGLETON(CSpawn_Manager)

private:
	CSpawn_Manager();
	virtual ~CSpawn_Manager() = default;

public:
	HRESULT Initialize(_uint iNaviProtoLevel, WNameID strNaviProtoTag);
	HRESULT Begin();
	void    Update(_float fTimeDelta);
	void    Clear();

	HRESULT Register_SpawnRect(const SPAWN_RECT_DESC& tDesc);
	HRESULT Load_From_File(const _tchar* pFilePath);

	const vector<SPAWN_RECT_RUNTIME>& Get_Runtimes() const { return m_Runtimes; }

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif

private:
	_bool   Prepare_SpawnRect(SPAWN_RECT_RUNTIME& tRuntime);
	_bool   Try_SpawnWildPokemon(SPAWN_RECT_RUNTIME& tRuntime);
	_bool   Spawn_NPC(SPAWN_RECT_RUNTIME& tRuntime);
	_bool   Check_DistanceFromPlayer(const _float3& vPos, _float fMin, _float fMax) const;
	void    Recount_AliveCounts();

private:
	CGameInstance* m_pGameInstance = { nullptr };   // weak
	CNavigation* m_pNavigationClone = { nullptr };   // owned (Safe_AddRef / Safe_Release)

	vector<SPAWN_RECT_RUNTIME>	m_Runtimes;
	_bool						m_bInitialized = { false };
	_bool						m_bBegan = { false };

#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

private:
	HRESULT Ready_DebugResources();
#endif

public:
	virtual void Free() override;
};

NS_END
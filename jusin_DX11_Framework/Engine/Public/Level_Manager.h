#pragma once
#include "Base.h"

/* -------------------------------------------------- */
// 레벨 매니저
// - 보관 중인 레벨의 반복적인 Update, Render를 수행
// - 현재 할당된 레벨의 주소를 보관
// - 원활한 레벨 교체작업 수행
// (레벨 교체 시 이전 레벨 삭제, 기존 레벨용 자원을 정리)
/* -------------------------------------------------- */

NS_BEGIN(Engine)
class CGameInstance;
class CLevel;

class CLevel_Manager final : public CBase
{
public:
	struct LEVEL_ENTRY
	{
		_int iLevelIndex = { -1 };
		CLevel* pLevel = { nullptr };
	};

private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	_int  Get_CurrentLevel() const { return m_iCurrentLevelIndex; }
	_bool Is_Level_Active(_uint iLevel) const;

	HRESULT Change_Level(_int iNewLevelIndex, class CLevel* pNewLevel);	// 스택 정리 후 새 레벨로 시작
	HRESULT Push_Level(_int iLevelIndex, class CLevel* pNewLevel);		// 새 레벨을 스택 top에 push(직전 top은 OnPause)
	HRESULT Pop_Level();												// 현재 top을 정리 후 pop(새로운 top은 OnResume)
	void	Update(_float fTimeDelta);
	HRESULT	Render();


private:
	CGameInstance*		m_pGameInstance = { nullptr };
	vector<LEVEL_ENTRY>	m_LevelStack;
	_int				m_iCurrentLevelIndex = { -1 };   // 스택 top의 인덱스 캐싱

public:
	static CLevel_Manager* Create();

protected:
	virtual void	Free() override;
};

NS_END
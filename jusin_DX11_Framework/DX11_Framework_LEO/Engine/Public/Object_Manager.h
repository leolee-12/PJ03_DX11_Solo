#pragma once
#include "Base.h"

/* -------------------------------------------------- */
// 프로토타입 매니저
// - 실제 동작할 객체(사본)를 보관(레벨별 + 레이어별)
// - 보관 중인 객체들의 갱신(Update)
// - 보관할 객체의 사본 생성 및 추가
/* -------------------------------------------------- */

NS_BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_GameObject(	_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
							_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	void	Priority_Update(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Late_Update(_float fTimeDelta);

private:
	typedef unordered_map<_wstring, class CLayer*>	LAYERS;
	LAYERS*	m_pLayers = { nullptr };
	size_t	m_iNumLevels = {};

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);


public:
	static CObject_Manager*	Create(_uint iNumLevels);

protected:
	virtual void	Free() override;

};

NS_END
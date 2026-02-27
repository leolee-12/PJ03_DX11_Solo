#pragma once

/* 게임오브젝트 원형을 복제하여 사본을 보관해주기위한 기능 */
/* 사본객체들을 보관 : 레벨별로, 각 레벨당 레이어로 구분하여 */
#include "Base.h"

NS_BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iIndex, const _wstring& strComponentTag);

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_GameObject(_uint iPrototypeLevelID, const _wstring& strPrototypeTag, _uint iLayerLevelID, const _wstring& strLayerTag, void* pArg);
	void Clear(_uint iLevelID);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
private:
	_uint		m_iNumLevels = {};
	//map<const _wstring, list<CGameObject*>>			m_GameObjects[LEVEL::END];
	//map<const _wstring, list<CGameObject*>>*		m_pGameObjects = { nullptr };
	map<const _wstring, class CLayer*>*				m_pLayers = { nullptr };
	class CGameInstance*							m_pGameInstance = { nullptr };

private:
	CLayer* Find_Layer(_uint iLevelID, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END
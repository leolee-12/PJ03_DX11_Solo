#pragma once

#include "Base.h"

/* 생성한 객체들을 보관한다. */
/* Prototype 방식으로 객체를 추가한다. */


BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	class list<class CGameObject*> Get_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	_uint GetLayerSize(_uint iLevelIndex, const wstring& strLayerTag);

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject_ToLayer(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strLayerTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Add_GameObject_ToLayer_AndGet(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg = nullptr);


	//파일 저장, 로드 관련 함수
	HRESULT Get_Prototype_Names(vector<string>* pVector);
	HRESULT Add_Object_Layers_Vector(_uint, vector<pair < string, list<CGameObject*>>>* pVector);
	HRESULT Add_Object_Layers_Vector(_uint, vector<pair < _wstring, list<CGameObject*>>>* pVector);

	//사이클 함수
	void Player_Update(_float fTimeDelta);
	void Camera_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	//삭제 관련 함수
	void Destory_Update();
	void Destory_Reserve(class CGameObject* gameObject);
	HRESULT Clear_Resources(_uint iLevelIndex);

private:
	map<const wstring, class CGameObject*>		m_Prototypes;

private:
	_uint										m_iNumLevels = { 0 };
	map<const wstring, class CLayer*>* m_pLayers = { nullptr };
	list<class CGameObject*>					m_DestoryObjects;

public:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END
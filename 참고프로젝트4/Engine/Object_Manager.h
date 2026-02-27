#pragma once

#include "Base.h"
#include "Layer.h"
/* 생성한 원형 객체를 모아놓는다. (프로토타입) */
/* 원형객체를 복제하여 생성한 사본객체를 레이어로 구분하여 모아놓는다. */
/* 보관하고 있는 사본객체들의 Tick관련 함수를 반복적으로 호출해준다.  */



BEGIN(Engine)

class CComponent;

class ENGINE_DLL CObject_Manager final : public CBase
{
	INFO_CLASS(CObject_Manager, CBase)

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	void Priority_Tick(_cref_time fTimeDelta);
	void Tick(_cref_time fTimeDelta);
	void Late_Tick(_cref_time fTimeDelta);
	void Before_Render(_cref_time fTimeDelta);
	void Clear(_uint iLevelIndex);

	HRESULT Add_Prototype(const wstring& strPrototypeTag,  shared_ptr<class CGameObject> pPrototype);

	template<class T>
	HRESULT Add_CloneObject(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strPrototypeTag, void* pArg, shared_ptr<T>* ppOut = nullptr);

	HRESULT Add_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, shared_ptr<CGameObject> pObject);
	shared_ptr<CGameObject> CloneObject(const wstring& strPrototypeTag, void* pArg);

	shared_ptr<CGameObject> Get_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, _uint iIndex);
	shared_ptr<CComponent> Get_Component(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag);

private:
	_uint			m_iNumLevels = { 0 };

private:
	map<const wstring, class shared_ptr<CGameObject>>				m_Prototypes;
	map<const LAYERTYPE, class CLayer*>*				m_pLayers = { nullptr } ;
	typedef map<const LAYERTYPE, class CLayer*>		LAYERS;

public:
	map<const wstring,  shared_ptr<class CGameObject>>* Get_PrototypeList() { return &m_Prototypes; }
	list<shared_ptr<class CGameObject>>*				Get_ObjectList(_uint iLevelIndex, const LAYERTYPE& strLayerTag);

private:
	class shared_ptr<CGameObject> Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const LAYERTYPE& strLayerTag);
public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};


template<class T>
HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strPrototypeTag, void* pArg, shared_ptr<T>* ppOut)
{
	/* 원형을 찾고. */
	shared_ptr<CGameObject>		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		RETURN_EFAIL;

	/* 원형을 복제하여 실제 게임내에 사용할 사본 객체를 생성해낸다.  */
	shared_ptr<CGameObject>		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		RETURN_EFAIL;

	pGameObject->Set_PrototypeTag(strPrototypeTag);

	/* 만들어낸 사본객체를 추가해야할 레이어를 찾자. */
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 해당 이름을 가진 레이어가 없었다. */
	/* 이 이름을 가진 레이어에 최초로 추가하고 있는 상황이다. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			RETURN_EFAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	/* 이미 이름을 가진 레이어가 있었어. */
	else
		pLayer->Add_GameObject(pGameObject);

	if (ppOut != nullptr)
		*ppOut = static_pointer_cast<T>(pGameObject);

	return S_OK;
}

END
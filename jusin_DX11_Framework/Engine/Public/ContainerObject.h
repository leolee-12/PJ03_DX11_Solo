#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
protected:
	CContainerObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	WNameMap<class CPartObject*> m_PartObjects;

protected:
	HRESULT Add_PartObject(_uint iPrototypeLevelIndex, const WNameID strPrototypeTag,
		const WNameID strPartTag, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;

protected:
	virtual void Free();
};

NS_END
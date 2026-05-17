#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	struct PARTOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	const _float4x4* m_pParentMatrix = {};
	_float4x4 m_CombinedWorldMatrix = {};

protected:
	void Compute_CombinedWorldMatrix(_fmatrix ChildMatrix);

public:
	virtual CGameObject* Clone(void* pArg) = 0;

protected:
	virtual void Free();
};

NS_END
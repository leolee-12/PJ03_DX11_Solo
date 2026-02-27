#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTool_Collider final : public CGameObject
{
	INFO_CLASS(CTool_Collider, CGameObject)

public:
	typedef struct tagColliderDesc :public GAMEOBJECT_DESC
	{
		_float3 vPos;
	}COLLDESC;

private:
	CTool_Collider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTool_Collider(const CTool_Collider& rhs);
	virtual ~CTool_Collider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(void* pArg);

public:
	static shared_ptr<CTool_Collider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;
};

END
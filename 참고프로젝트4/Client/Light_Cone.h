#pragma once
#include "GameObject.h"
class CLight_Cone : public CGameObject
{
private:
	CLight_Cone(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CLight_Cone(const CGameObject& rhs);
	virtual ~CLight_Cone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
public:
	GETSET_EX2(float4, vColor, LightConeColorSet, GET, SET)

public:
	static shared_ptr<CLight_Cone> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;
};


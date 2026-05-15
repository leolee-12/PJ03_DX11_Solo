#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	struct CAMERA_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vEye, vAt;
		_float fFovy, fNear, fFar;
	};

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	void Set_FollowTarget(class CTransform* pTarget);
	void Set_FollowOffset(const _float3& v) { m_vFollowOffset = v; }
	void Set_LookOffset(const _float3& v) { m_vLookOffset = v; }
	_bool Is_Following() const { return m_bFollow; }
	void Set_Following(_bool b) { m_bFollow = b; }
	void Toggle_Following() { m_bFollow = !m_bFollow; }

	virtual void Set_ControlEnabled(_bool) {};
	virtual _bool Is_ControlEnabled() const { return false; }
	virtual _string Get_TypeName() const { return "Camera"; }
	const _float* Get_FarZPtr() const { return &m_fFar; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void On_ViewportResize(_float2 vNewSize);

private:
	_float m_fFovy, m_fAspect, m_fNear, m_fFar;
	_float4x4 m_ProjMatrix = {};
	_bool m_bProjDirty = {};
	class CPipeLine* m_pPipeLine = { nullptr };

	_bool m_bFollow = { false };
	CTransform* m_pFollowTarget = { nullptr };
	_float3 m_vFollowOffset = { 0.f, 5.f, -7.f };
	_float3 m_vLookOffset = { 0.f, 1.5f, 0.f };
	_float m_fFollowLerp = 10.f;

private:
	void Update_PipeLine();

public:
	virtual CGameObject* Clone(void* pArg) = 0;

protected:
	virtual void Free() override;
};

NS_END
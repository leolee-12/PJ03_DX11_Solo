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
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float m_fFovy, m_fAspect, m_fNear, m_fFar;
	_float4x4 m_ProjMatrix = {};
	_bool m_bProjDirty = {};
	class CPipeLine* m_pPipeLine = { nullptr };

private:
	void Update_PipeLine();

public:
	virtual CGameObject* Clone(void* pArg) = 0;

protected:
	virtual void Free() override;
};

NS_END
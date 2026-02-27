#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFrieza_Metal final : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
		const _float4x4* pSocketBoneMatrix;

		_float3	vScale;
		_float4	vPosition;

		_int* pLookDirection;
	}Frieza_Metal_DESC;

private:
	CFrieza_Metal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFrieza_Metal(const CFrieza_Metal& Prototype);
	virtual ~CFrieza_Metal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Player_Update(_float fTimeDelta) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };

	const _float4x4*	m_pParentMatrix = { nullptr };
	const _float4x4*	m_pSocketMatrix = { nullptr };

	_float4x4			m_WorldMatrix = {};
	CTransform*			m_pLookAtTransformCom = { nullptr };

	_int*				m_pLookDirection = { nullptr };

	_uint		m_iTestRGIndex = { 9 };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFrieza_Metal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
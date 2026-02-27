#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Layer.h"
#include "Effect.h"
#include "Map_Manager.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CModel;
END

BEGIN(Client)

class CVolcano_Destructive final : public CGameObject
{
public:
	enum Space_TextureType { SPACE_DIFFUSE, SPACE_STAR_1, SPACE_STAR_2, SPACE_END };

private:
	CVolcano_Destructive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVolcano_Destructive(const CVolcano_Destructive& Prototype);
	virtual ~CVolcano_Destructive() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Priority_Render(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	_float3 m_vFragmentMoveDir[15];
	_float4 m_vFragmentPosition[15];
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom_Diffuse = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CModel* m_pBRModelCom = { nullptr };
	CModel* m_pFragmentModelCom[15] = { nullptr,nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,nullptr,nullptr,nullptr,nullptr };

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTransform* m_pEffectTransform = { nullptr };
	_float					m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

	_uint m_iRGIndex = { 0 };
private:
	const _float m_fFastTimeLimit = { 0.3f };
	_float m_fAccTime = { 0.f };
	_bool m_isFastSwitch = { true };
	_float m_fSpeed = { 0.3f };

	_bool m_isBrakeSwitch = { false };
	_float m_fBrakeSwitchTime = { 0.f };
	_float4x4 m_Result4x4;
	CEffect_Layer* m_pEffectLayer = { nullptr };

	_bool m_isStart = { false };
	_bool m_isRight = { true };
public:
	void Start_Volcano_DestructiveFinish(_bool isRight = true);
	void IsDone_Active_Init();
	_float4x4 m_IdentityMatrix;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CVolcano_Destructive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
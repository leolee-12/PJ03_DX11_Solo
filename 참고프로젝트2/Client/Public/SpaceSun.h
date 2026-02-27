#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Map_Manager.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSpaceSun final : public CGameObject
{
public:
	enum Space_TextureType { SPACE_DIFFUSE,SPACE_STAR_1, SPACE_STAR_2, SPACE_END};

private:
	CSpaceSun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpaceSun(const CSpaceSun& Prototype);
	virtual ~CSpaceSun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom_Light = { nullptr };
	CModel*					m_pModelCom = { nullptr };

	CTexture* m_pTextureCom_Rainbow = { nullptr };
	CModel* m_pModelCom_Rainbow = { nullptr };
	CTransform* m_pTransformCom_Rainbow = { nullptr };
	_int m_iIndex = { 1 };
private:
	_float2 m_fSpriteAnimCount = { 16.f,1.f };
	_float2 m_fSpriteSize = { 0.f,0.f };
	_float2 m_fSpriteCurPos = { 0.f,0.f };
	_float m_fAccTime = { 0.3f };
	_float m_fOneTime = { 0.f };
	_bool m_isPlus = { true };
	_bool m_isMaintain = { false };
private:
	_bool m_isRainbowSwitch = { false };
	_float	m_fRainbowTime = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSpaceSun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Define.h"

BEGIN(Engine)

class CShader;
class CModel;
class CTexture;

END

BEGIN(Client)

class CCharaSelect_Model final : public CGameObject
{
public:
	typedef struct
	{
		CUI_Define::PLAYER_ID ePlayerID = {};
		_uint iNumModel = { 0 };
	}SELECT_MODEL;

private:
	CCharaSelect_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharaSelect_Model(const CCharaSelect_Model& Prototype);
	virtual ~CCharaSelect_Model() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom[4] = {nullptr};
	CTexture* m_pDiffTexture[4] = {nullptr};
	CTexture* m_pLimTexture[4] = {nullptr};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Default_Position(_uint iNumModel);

	void Default_PlayerSetting();
	HRESULT Add_Light(_float4 vDirection, _float4 vDiffuse, _float4 vAmbient, _float4 vSpecular, string strName);

private:
	CUI_Define::PLAYER_ID m_ePlayerID = {};
	_wstring m_strModelTag = {};
	_int m_iLookDirection = { 1 };

	_uint m_iShaderIndex = { 0 };
	_float m_fRenderTimer = { 0.f };

	_bool m_bRender = { TRUE };
	_bool m_bLight = { FALSE };

public:
	static CCharaSelect_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
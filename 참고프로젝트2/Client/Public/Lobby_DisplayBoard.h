#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLobby_DisplayBoard final : public CGameObject
{
public:
	enum LOBBY_FONTID { STORY , PRACTICE , FONT_END};
	enum LOBBY_INOUT  { LOBBY_IN , LOBBY_OUT, INOUT_END };

public:
	typedef struct : public GAMEOBJECT_DESC
	{
		LOBBY_FONTID eFontID = {};
		LOBBY_INOUT eInOutID = {};
	}DISPLAY_DESC;

private:
	CLobby_DisplayBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_DisplayBoard(const CLobby_DisplayBoard& Prototype);
	virtual ~CLobby_DisplayBoard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pFontTextureCom = { nullptr };

private:
	_float m_fTexcorrdValue = { 0.f };
	_float m_fAlphaValue = { 0.f };

	_vector vDiffColor = {};
	_vector vOutLineColor = {};

	LOBBY_FONTID m_eFontID = { FONT_END };
	LOBBY_INOUT m_eInOutID = { INOUT_END };

	_wstring strMeshTag = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Set_DefaultSetting();

public:
	static CLobby_DisplayBoard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
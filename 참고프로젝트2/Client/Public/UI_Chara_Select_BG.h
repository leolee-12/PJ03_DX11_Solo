#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Chara_Select_BG final : public CUIObject
{
private:
	CUI_Chara_Select_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Chara_Select_BG(const CUI_Chara_Select_BG& Prototype);
	virtual ~CUI_Chara_Select_BG() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_float m_fMarkTimer = { 0.f };

	CTexture* m_pBGTexture = { nullptr };
	CTexture* m_pDustTexutre = { nullptr };

public:
	static CUI_Chara_Select_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
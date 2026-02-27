#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_VS_BG final : public CUIObject
{
private:
	CUI_VS_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_BG(const CUI_VS_BG& Prototype);
	virtual ~CUI_VS_BG() = default;

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
	CTexture* m_pTextureLine = { nullptr };
	CTexture* m_pTextureColor = { nullptr };
	CTexture* m_pTextureCircle[4] = {nullptr,nullptr ,nullptr ,nullptr };

	_float m_fMaskMoveValue = { 0.f };

public:
	static CUI_VS_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
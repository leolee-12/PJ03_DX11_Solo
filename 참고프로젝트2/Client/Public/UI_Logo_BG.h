#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Logo_BG final : public CUIObject
{
private:
	CUI_Logo_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Logo_BG(const CUI_Logo_BG& Prototype);
	virtual ~CUI_Logo_BG() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CTexture* m_pVideoTextureCom = { nullptr };

private:
	_float m_fVideoSprite = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_Logo_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
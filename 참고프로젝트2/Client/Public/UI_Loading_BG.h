#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Loading_BG final : public CUIObject
{
private:
	CUI_Loading_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_BG(const CUI_Loading_BG& Prototype);
	virtual ~CUI_Loading_BG() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pBGTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_float m_fBGAlphaValue = { 1.f };
	_bool m_bBallFinish = { FALSE };
	_bool m_IsAlphaSwich = { FALSE };
	_uint m_iTextureIndex = { 0 };

public:
	static CUI_Loading_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
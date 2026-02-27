#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_VS_CharaPanel final :public CUIObject
{
private:
	CUI_VS_CharaPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_CharaPanel(const CUI_VS_CharaPanel& Prototype);
	virtual ~CUI_VS_CharaPanel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

private:
	void InitPosition();
	

private:
	_uint m_iNumUI = { 0 };
	_uint m_iTotalNum = { 0 };
	UI_LRPOS m_eLR = { POS_END };

	CTexture* m_pBGTexture = { nullptr };
	CTexture* m_pMarkTexture = { nullptr };
	CTexture* m_pNameTexture = { nullptr };

public:
	static CUI_VS_CharaPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_CharaSelectImage final :public CUIObject
{
public:
	typedef struct :public CUIObject::UI_DESC
	{
		_uint iTextureIndex = { 0 };
		_uint iNumChoice = { 0 };
	}UI_IMAGE_DESC;

private:
	CUI_CharaSelectImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CharaSelectImage(const CUI_CharaSelectImage& Prototype);
	virtual ~CUI_CharaSelectImage() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

public:
	void InitPosition();

private:
	_uint m_iTexIndex = { 0 };
	_uint m_iNumChoice = { 0 };
	CTexture* m_pMaskTexture = { nullptr };


public:
	static CUI_CharaSelectImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
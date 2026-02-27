#pragma once

#include "UI_Input.h"

BEGIN(Engine)

class CTexture;

END


BEGIN(Client)

class CUI_InputDirIcon final :public CUI_Input
{


private:
	CUI_InputDirIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InputDirIcon(const CUI_InputDirIcon& Prototype);
	virtual ~CUI_InputDirIcon() = default;

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
	_uint m_iNumCommandList = { 0 };
	_uint m_iTextureIndex = { 0 };

	CTexture* m_pBGTexture = { nullptr };
	

public:
	static CUI_InputDirIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
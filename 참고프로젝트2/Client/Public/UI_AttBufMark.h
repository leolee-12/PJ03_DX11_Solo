#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_AttBufMark final :public CUI_BaseAttBuf
{
private:
	CUI_AttBufMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_AttBufMark(const CUI_AttBufMark& Prototype);
	virtual ~CUI_AttBufMark() = default;

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
	static CUI_AttBufMark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
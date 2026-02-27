#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_SelectLine final : public CUIObject
{
public:
	typedef struct :public CUIObject::UI_DESC
	{
		_uint iNumChoice = {};
	}UI_LINE_DESC;

private:
	CUI_SelectLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SelectLine(const CUI_SelectLine& Prototype);
	virtual ~CUI_SelectLine() = default;

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

	void InitPosition();



private:
	_uint m_iNumIndex = { 0 };

public:
	static CUI_SelectLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
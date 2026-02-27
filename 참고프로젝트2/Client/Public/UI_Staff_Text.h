#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include <queue>

BEGIN(Engine)

class CVIBuffer_Rect;
class CTexture;
class CShader;

END

BEGIN(Client)

class CUI_Staff_Text final :public CGameObject
{
public:
	typedef struct
	{
		CTransform* pStaffTransform = {};
	}STAFF_TEXT_DESC;


private:
	CUI_Staff_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Staff_Text(const CUI_Staff_Text& Prototype);
	virtual ~CUI_Staff_Text() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

public:
	_bool Get_Finish() { return m_bFinishText; }

private:
	virtual HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Add_Text();
	void TextOutput(_float fOutputSpeed, _float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	CTransform* m_pStaffTransform = { nullptr };

private:
	_bool m_bFinishText = { FALSE };

	_float m_fFontDration = { 0.f };
	_uint m_iTextIndex = { 0 };

	queue<_wstring> m_vTextArray = {};
	_wstring m_strOutputText = {};

	_float m_fOffsetPosY = { 0.f };
public:
	static CUI_Staff_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIProgressBar : public CUIObject
{
public:
	enum class UI_PROGRESS_DIR { LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM, BOTTOM_TO_TOP, END };

	struct UIPROGRESSBAR_DESC : public CUIObject::UIOBJECT_DESC
	{
		WNameID strBackTextureTag{ INVALID_TAG };
		_uint iBackTextureIndex{ INVALID_INDEX };

		WNameID strFillTextureTag{ INVALID_TAG };
		_uint iFillTextureIndex{ INVALID_INDEX };

		WNameID strShaderTag{ INVALID_TAG };
		WNameID strVIBufferTag{ INVALID_TAG };

		_uint iShaderLevel{ INVALID_INDEX };
		_uint iVIBufferLevel{ INVALID_INDEX };
		_uint iBackTextureLevel{ INVALID_INDEX };
		_uint iFillTextureLevel{ INVALID_INDEX };

		_float4 vBackColor{ g_kWhite };
		_float4 vFillColor{ g_kWhite };

		_float fFillAmount = { 1.f };
		UI_PROGRESS_DIR eDirection = { UI_PROGRESS_DIR::LEFT_TO_RIGHT };
	};

protected:
	CUIProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIProgressBar(const CUIProgressBar& Prototype);
	virtual ~CUIProgressBar() = default;

public:
	virtual _string Get_TypeName() const override { return "UIProgressBar"; }
	virtual UI_TYPE Get_UIType() const override { return UI_TYPE::PROGRESSBAR; }

	void Set_FillAmount(_float fFillAmount) { m_fFillAmount = max(0.f, min(1.f, fFillAmount)); }
	void Set_Direction(UI_PROGRESS_DIR eDirection) { m_eDirection = eDirection; }
	void Set_BackTexture(WNameID strTextureTag, _uint iTextureIndex) { m_strBackTextureTag = strTextureTag; m_iBackTextureIndex = iTextureIndex; }
	void Set_FillTexture(WNameID strTextureTag, _uint iTextureIndex) { m_strFillTextureTag = strTextureTag; m_iFillTextureIndex = iTextureIndex; }

	_float Get_FillAmount() const { return m_fFillAmount; }
	UI_PROGRESS_DIR Get_Direction() const { return m_eDirection; }


	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	class CShader* m_pShaderCom{ nullptr };
	class CTexture* m_pBackTextureCom{ nullptr };
	class CTexture* m_pFillTextureCom{ nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom{ nullptr };

	WNameID m_strBackTextureTag = { INVALID_TAG };
	_uint m_iBackTextureIndex = { INVALID_INDEX };

	WNameID m_strFillTextureTag = { INVALID_TAG };
	_uint m_iFillTextureIndex = { INVALID_INDEX };

	WNameID m_strShaderTag = { INVALID_TAG };
	WNameID m_strVIBufferTag = { INVALID_TAG };

	_uint m_iShaderLevel = { INVALID_INDEX };
	_uint m_iVIBufferLevel = { INVALID_INDEX };
	_uint m_iBackTextureLevel = { INVALID_INDEX };
	_uint m_iFillTextureLevel = { INVALID_INDEX };

	_float4 m_vBackColor = { g_kWhite };
	_float4 m_vFillColor = { g_kWhite };

	_float m_fFillAmount = { 1.f };
	UI_PROGRESS_DIR m_eDirection = { UI_PROGRESS_DIR::LEFT_TO_RIGHT };

protected:
	HRESULT Ready_Components();
	HRESULT Render_Rect(const _float4& rc, CTexture* pTexture, _uint iTextureIndex, const _float4& vColor);
	_float4 Get_FillRect() const;
	_bool Has_ValidBack() const;
	_bool Has_ValidFill() const;

public:
	static CUIProgressBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free();
};

NS_END
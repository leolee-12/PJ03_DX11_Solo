#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIImage : public CUIObject
{
public:
	struct UIIMAGE_DESC : public CUIObject::UIOBJECT_DESC
	{
		WNameID strTextureTag = { INVALID_TAG };
		WNameID strShaderTag = { INVALID_TAG };
		WNameID strVIBufferTag = { INVALID_TAG };

		_uint iShaderLevel = { INVALID_INDEX };
		_uint iVIBufferLevel = { INVALID_INDEX };
		_uint iTextureLevel = { INVALID_INDEX };
		
		_uint iTextureIndex = { INVALID_INDEX };
		_uint iShaderPass = { 0 };
		_float4 vColor = { g_kWhite };

		_bool bSpriteAnimEnabled = { false };
		_float fSpriteFrameDuration = { 0.f };
		_bool bSpriteAnimLoop = { true };

		vector<UI_SHARED_TEXTURE_BINDING_DESC> SharedTextureBindings;
	};

protected:
	CUIImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIImage(const CUIImage& Prototype);
	virtual ~CUIImage() = default;

public:
	virtual _string Get_TypeName() const override { return "UIImage"; }
	virtual UI_TYPE Get_UIType() const override { return UI_TYPE::IMAGE; }

	void Set_Texture(WNameID strTextureTag, _uint iTextureIndex) { m_strTextureTag = strTextureTag; m_iTextureIndex = iTextureIndex; }
	void Set_Color(const _float4& vColor) { m_vColor = vColor; }
	void Set_SharedTextureBindings(const vector<UI_SHARED_TEXTURE_BINDING_DESC>& Bindings) { m_SharedTextureBindings = Bindings; }

	// Sprite anim
	void Set_SpriteAnim(_bool bEnabled, _float fFrameDuration, _bool bLoop = true);
	virtual void Set_SpriteTickAllowed(_bool bAllowed) override { m_bSpriteTickAllowed = bAllowed; }
	/* sprite 프레임 인덱스 / 누적시간을 0 으로 강제 리셋.
   시퀀스 두 번째 재생 시 첫 재생의 끝 프레임에 머무는 현상 방지. */
	void Reset_SpriteAnim();

	const vector<UI_SHARED_TEXTURE_BINDING_DESC>& Get_SharedTextureBindings() const { return m_SharedTextureBindings; }
	WNameID Get_TextureTag() const { return m_strTextureTag; }
	_uint Get_TextureIndex() const { return m_iTextureIndex; }
	const _float4& Get_Color() const { return m_vColor; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual _bool Can_Apply_Tween_Target(UI_TWEEN_TARGET eTarget) const;
	virtual HRESULT Apply_Tween_Target(UI_TWEEN_TARGET eTarget, _float fValue);

protected:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	WNameID m_strTextureTag = { INVALID_TAG };
	WNameID m_strShaderTag = { INVALID_TAG };
	WNameID m_strVIBufferTag = { INVALID_TAG };

	_uint m_iTextureLevel = { INVALID_INDEX };
	_uint m_iShaderLevel = { INVALID_INDEX };
	_uint m_iVIBufferLevel = { INVALID_INDEX };

	_uint m_iTextureIndex = { INVALID_INDEX };
	_uint m_iShaderPass = { 0 };
	_float4 m_vColor = { g_kWhite };

	_bool m_bSpriteAnimEnabled = { false };
	_float m_fSpriteFrameDuration = { 0.f };
	_bool m_bSpriteAnimLoop = { true };
	_uint  m_iSpriteFrameCount = { 0 };
	_float m_fSpriteAccumTime = { 0.f };
	_bool  m_bSpriteTickAllowed = { true };

	vector<UI_SHARED_TEXTURE_BINDING_DESC> m_SharedTextureBindings;

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	_bool Has_ValidData() const;

	void Refresh_SpriteAnimState();

public:
	static CUIImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free();
};

NS_END
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CShader_Texture final : public CGameObject
{
public:/* 클라로 파싱할때 절대 안넘김*/
	enum SPRITE_BUTTON_TYPE { SB_PLAY, SB_PAUSE,SB_RESET};
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_wstring prototypeKey;
		const _tchar* filePath;
		_int iID;

		_int iShaderTab_ID;
	}SHADER_TEXTURE_DESC;
	typedef struct
	{
		_bool isOn = { false };
		_float2* vDirection = { nullptr };
		_float* fSpeed = { nullptr };
	}Shade_MoveTex;
	typedef struct
	{
		_bool isOn = { false };
		_float2* fSpriteSizeNumber = { nullptr };
		_float* fSpeed = { nullptr };
		_float2 fSpriteSize = { 0.f,0.f };
		_float2 fSpriteCurPos = { 0.f,0.f };
		_float fAccTime = { 0.f };
	}Shade_Sprite;
private:
	CShader_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader_Texture(const CShader_Texture& Prototype);
	virtual ~CShader_Texture() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Push_InputTextures(ID3D11ShaderResourceView* pSRV, _int LineIndex);
	void Push_Shade_MoveTex(_float2* pDirection, _float* pSpeed);
	void Push_Shade_Sprite(_float2* fSpriteSizeNumber, _float* pSpeed);
	void Remove_InputTextures(_int LineIndex);
	void Remove_InputFunction(_int iFunctionType);

	void Set_PlusKey(_wstring Key) { m_Key = Key; }

	void Add_CloneValue(CEffect* pEffect);
	_int Update_CloneValue(CEffect* pEffect, _float fTimeDelta);
	void Delete_CloneValue(CEffect* pEffect);
	_int m_iID = { 0 };
	Shade_Sprite m_Sprite;
	Shade_MoveTex m_MoveTex;
	CTexture* m_pTextureCom = { nullptr };
private:
	unordered_map<CEffect*, Shade_Sprite> m_CloneSprites;
	unordered_map<CEffect*, Shade_MoveTex> m_CloneMoveTexs;

private:
	_bool m_isDiffuse = { false };
	_bool m_isAlpha = { false };
	map<string, ID3D11ShaderResourceView*> m_InputTextures;
	//vector<ID3D11ShaderResourceView*> m_InputTextures;
private:
	_bool m_isTex = { false };
private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_wstring m_Key;
	_float m_fTime = { 0.f };
public:/* 텍스쿠드 곱해주기 */
	_float2 m_vMultiple_Texcoord = { 1.f,1.f };
private:
	_float					m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

public:	/* 클라로 파싱할때 절대 안넘김*/
	void Sprite_ButtonClick(_int iClickButton);
	void Sprite_Loop(_bool isLoop) { m_isLoop = isLoop; }
private: /* 클라로 파싱할때 절대 안넘김*/
	_bool isSpritePlay = { false };
	_bool m_isLoop = { false };
private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

	HRESULT Bind_CloneShaderResources(CEffect* pEffect);

public:
	static CShader_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual void Free() override;

	// CGameObject을(를) 통해 상속됨
	CGameObject* Clone(void* pArg) override;
};

END
#include "stdafx.h"
#include "..\Public\Shader_Texture.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "Imgui_Manager.h"
#include "IMGUI_Shader_Tab.h"

CShader_Texture::CShader_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CShader_Texture::CShader_Texture(const CShader_Texture& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CShader_Texture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShader_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_fSizeX = 512;
	m_fSizeY = 512;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_isTex = true;
	CImgui_Manager* pImGui_Manager = CImgui_Manager::Get_Instance();

	pImGui_Manager->Access_Shader_Tab(static_cast<SHADER_TEXTURE_DESC*>(pArg)->iShaderTab_ID)->Push_ShaderTexture(this);
	_float2 fTextureSize = m_pTextureCom->Get_TextureSize();

	_float fAspectRatio = fTextureSize.x / fTextureSize.y;
	_float fWinAspectRatio = (_float)g_iWinSizeX / (_float)g_iWinSizeY;

	if (fTextureSize.x > g_iWinSizeX || fTextureSize.y > g_iWinSizeY)
	{
		if (fTextureSize.x / g_iWinSizeX > fTextureSize.y / g_iWinSizeY)
		{
			fTextureSize.x = g_iWinSizeX;
			fTextureSize.y = g_iWinSizeX / fAspectRatio;
		}
		else
		{
			fTextureSize.y = g_iWinSizeY;
			fTextureSize.x = g_iWinSizeY * fAspectRatio;
		}
	}

	m_pTransformCom->Set_Scaled(fTextureSize.x, fTextureSize.y, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(fTextureSize.x *0.5f - m_fX, m_fY - fTextureSize.y * 0.5f, 0.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION,
	//	XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CShader_Texture::Camera_Update(_float fTimeDelta)
{
	
}

void CShader_Texture::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_Sprite.isOn == true && isSpritePlay == true)
	{
		m_Sprite.fAccTime += fTimeDelta;

		if (m_Sprite.fAccTime > (1.0f / (*m_Sprite.fSpeed)))
		{
			m_Sprite.fAccTime = 0.f;
			m_Sprite.fSpriteCurPos.x++;

			if (m_Sprite.fSpriteCurPos.x == m_Sprite.fSpriteSizeNumber->x)
			{
				m_Sprite.fSpriteCurPos.x = 0.f;
				m_Sprite.fSpriteCurPos.y++;
			}
		}

		if (m_Sprite.fSpriteCurPos.y == m_Sprite.fSpriteSizeNumber->y)
		{
			if (m_isLoop == true)
			{
				m_Sprite.fSpriteCurPos.y = 0.f;
				m_Sprite.fSpriteCurPos.x = 0.f;
			}
			else
			{
				m_Sprite.fSpriteCurPos.y = m_Sprite.fSpriteSizeNumber->y - 1;
				m_Sprite.fSpriteCurPos.x = m_Sprite.fSpriteSizeNumber->x - 1;
			}
		}
	}

}

void CShader_Texture::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CShader_Texture::Render(_float fTimeDelta)
{
	if (FAILED(m_pRenderInstance->Begin_MRT(m_Key)))
		return E_FAIL;
	  
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

void CShader_Texture::Push_InputTextures(ID3D11ShaderResourceView* pSRV, _int LineIndex)
{
	if (LineIndex == CIMGUI_Shader_Tab::END_DIFFUSE)
	{
		m_isDiffuse = true;
		m_InputTextures["Diffuse"] = pSRV;
	}
	else if (LineIndex == CIMGUI_Shader_Tab::END_ALPHA)
	{
		m_isAlpha = true;
		m_InputTextures["Alpha"] = pSRV;
	}
}
void CShader_Texture::Remove_InputTextures(_int LineIndex)
{
	if (LineIndex == CIMGUI_Shader_Tab::END_DIFFUSE)
	{
		m_isDiffuse = false; 
		m_InputTextures.erase("Diffuse");
	}
	else if (LineIndex == CIMGUI_Shader_Tab::END_ALPHA)
	{
		m_isAlpha = false;
		m_InputTextures.erase("Alpha");
	}
}

void CShader_Texture::Remove_InputFunction(_int iFunctionType)
{
	if (iFunctionType == CIMGUI_Shader_Tab::FUNCTION_TEXMOVE)
	{
		m_MoveTex.isOn = false;
		m_MoveTex.fSpeed = nullptr;
		m_MoveTex.vDirection = nullptr;
	}
	else if (iFunctionType == CIMGUI_Shader_Tab::FUNCTION_SPRITE)
	{
		m_Sprite.isOn = false;
		m_Sprite.fSpriteSizeNumber = nullptr;
		m_Sprite.fSpeed = nullptr;

		m_Sprite.fSpriteSize	= {0.f,0.f};
		m_Sprite.fSpriteCurPos	= {0.f,0.f};
		m_Sprite.fAccTime = 0.f;
	}
}

void CShader_Texture::Add_CloneValue(CEffect* pEffect)
{
	Shade_Sprite Sprite = m_Sprite;
	Shade_MoveTex MoveTex = m_MoveTex;

	Sprite.fSpriteCurPos.x = 0;
	Sprite.fSpriteCurPos.y = 0;
	Sprite.fAccTime = 0.f;
	m_isLoop = false;
	
	m_CloneSprites.emplace(pEffect, Sprite);
	m_CloneMoveTexs.emplace(pEffect, MoveTex);
}

_int CShader_Texture::Update_CloneValue(CEffect* pEffect, _float fTimeDelta)
{
	_int iReturnCheck = { 0 };
	auto& it = m_CloneSprites.find(pEffect);

	if (it == m_CloneSprites.end())
		return -1; // Error

	m_fTime += fTimeDelta;

	if (it->second.isOn == true)
	{
		it->second.fAccTime += fTimeDelta;

		if (it->second.fAccTime > (1.0f / (*it->second.fSpeed)))
		{
			it->second.fAccTime = 0.f;
			it->second.fSpriteCurPos.x++;

			if (it->second.fSpriteCurPos.x == it->second.fSpriteSizeNumber->x)
			{
				it->second.fSpriteCurPos.x = 0.f;
				it->second.fSpriteCurPos.y++;
			}
		}

		if (it->second.fSpriteCurPos.y == it->second.fSpriteSizeNumber->y)
		{
			if (pEffect->m_bIsShaderLoop == true)
			{
				it->second.fSpriteCurPos.y = 0.f;
				it->second.fSpriteCurPos.x = 0.f;
			}
			else
			{

				it->second.fSpriteCurPos.y = it->second.fSpriteSizeNumber->y - 1;
				it->second.fSpriteCurPos.x = it->second.fSpriteSizeNumber->x - 1;

				/* Sprite Animation End */
				iReturnCheck = 1;
			}
		}
	}

	if (FAILED(m_pRenderInstance->Begin_MRT(m_Key)))
		return E_FAIL;

	if (FAILED(Bind_CloneShaderResources(pEffect)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderInstance->End_MRT()))
		return E_FAIL;

	return iReturnCheck; //Basic
}

void CShader_Texture::Delete_CloneValue(CEffect* pEffect)
{
	m_CloneMoveTexs.erase(pEffect);
	m_CloneSprites.erase(pEffect);
}


void CShader_Texture::Push_Shade_MoveTex(_float2* pDirection, _float* pSpeed)
{
	m_MoveTex.isOn = true;
	m_MoveTex.vDirection = pDirection;
	m_MoveTex.fSpeed = pSpeed;
}

void CShader_Texture::Push_Shade_Sprite(_float2* fSpriteSizeNumber, _float* pSpeed)
{
	m_Sprite.isOn = true;
	m_Sprite.fSpriteSizeNumber = fSpriteSizeNumber;
	m_Sprite.fSpeed = pSpeed;

	_float2 textureSize = m_pTextureCom->Get_TextureSize();
	m_Sprite.fSpriteSize = _float2(1.0f / m_Sprite.fSpriteSizeNumber->x, 1.0f / m_Sprite.fSpriteSizeNumber->y);

	m_Sprite.fSpriteCurPos = _float2(0.f, 0.f);
	m_Sprite.fAccTime = { 0.f };

	/*if (m_pRenderInstance->Add_ClientRenderTargetToMRT(m_Key, m_Key, fTextureSize.x, fTextureSize.y, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(1.f, 1.f, 1.f, 0.f)))
	{
		int a = 10;
	}
	else
	{
		int a = 10;
	}

	_float2 fSize = m_pTextureCom->Get_TextureSize();
	_float fDiff;

	if (fSize.x > g_iWinSizeX)
	{
		fDiff = 1920 - fSize.x;
		fSize.x += fDiff;
		fSize.y += fDiff;
	}

	if (fSize.y > g_iWinSizeY)
	{
		fDiff = 1080 - fSize.y;
		fSize.x += fDiff;
		fSize.y += fDiff;
	}

	m_pTransformCom->Set_Scaled(fSize.x, fSize.y, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(fSize.x * 0.5f - m_fX, m_fY - fSize.y * 0.5f, 0.f, 1.f));*/
}

void CShader_Texture::Sprite_ButtonClick(_int iClickButton)
{
	if (iClickButton == SB_PLAY)
	{
		isSpritePlay = true;
	}
	else if (iClickButton == SB_PAUSE)
	{
		isSpritePlay = false;
	}
	else if (iClickButton == SB_RESET)
	{
		m_Sprite.fSpriteCurPos.y = 0.f;
		m_Sprite.fSpriteCurPos.x = 0.f;
	}
}

HRESULT CShader_Texture::Ready_Components(void* pArg)
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxShaderRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	m_Key = static_cast<SHADER_TEXTURE_DESC*>(pArg)->prototypeKey;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_Key,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader_Texture::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fTime, sizeof(float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("isBindTexture", &m_isTex, sizeof(bool))))
		return E_FAIL;

	if (m_isTex == true)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("isBindTexture", &m_isTex, sizeof(bool))))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_RawValue("isAlpha", &m_isAlpha, sizeof(bool))))
			return E_FAIL;
		if(m_isAlpha == true)
			m_pShaderCom->Bind_ShaderResourceView("g_AlphaTexture", m_InputTextures["Alpha"]);


		if (FAILED(m_pShaderCom->Bind_RawValue("isDiffuse", &m_isDiffuse, sizeof(bool))))
			return E_FAIL;
		if (m_isDiffuse == true)
			m_pShaderCom->Bind_ShaderResourceView("g_DiffuseTexture", m_InputTextures["Diffuse"]);
		
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMultiple_Texcoord", &m_vMultiple_Texcoord, sizeof(_float2))))
		return E_FAIL;
	

	if (FAILED(m_pShaderCom->Bind_RawValue("isMoveTex", &m_MoveTex.isOn, sizeof(bool))))
		return E_FAIL;

	if (m_MoveTex.isOn == true)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDirection", m_MoveTex.vDirection, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", m_MoveTex.fSpeed, sizeof(float))))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_isSprite", &m_Sprite.isOn, sizeof(bool))))
		return E_FAIL;
	
	if (m_Sprite.isOn == true)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSize", &m_Sprite.fSpriteSize, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteCurPos", &m_Sprite.fSpriteCurPos, sizeof(_float2))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader_Texture::Bind_CloneShaderResources(CEffect* pEffect)
{
	auto& Sprite_iter = m_CloneSprites.find(pEffect);
	auto& Move_iter = m_CloneMoveTexs.find(pEffect);
	m_isTex = true;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fTime, sizeof(float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("isBindTexture", &m_isTex, sizeof(bool))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("isBindTexture", &m_isTex, sizeof(bool))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("isAlpha", &m_isAlpha, sizeof(bool))))
		return E_FAIL;

	if (m_isAlpha == true)
		m_pShaderCom->Bind_ShaderResourceView("g_AlphaTexture", m_InputTextures["Alpha"]);


	if (FAILED(m_pShaderCom->Bind_RawValue("isDiffuse", &m_isDiffuse, sizeof(bool))))
		return E_FAIL;

	if (m_isDiffuse == true)
		m_pShaderCom->Bind_ShaderResourceView("g_DiffuseTexture", m_InputTextures["Diffuse"]);

	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMultiple_Texcoord", &m_vMultiple_Texcoord, sizeof(_float2))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("isMoveTex", &Move_iter->second.isOn, sizeof(bool))))
		return E_FAIL;

	if (Move_iter->second.isOn == true)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDirection", Move_iter->second.vDirection, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", Move_iter->second.fSpeed, sizeof(float))))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_isSprite", &Sprite_iter->second.isOn, sizeof(bool))))
		return E_FAIL;

	if (Sprite_iter->second.isOn == true)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSize", &Sprite_iter->second.fSpriteSize, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteCurPos", &Sprite_iter->second.fSpriteCurPos, sizeof(_float2))))
			return E_FAIL;
	}

	return S_OK;
}

CShader_Texture* CShader_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShader_Texture* pInstance = new CShader_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShader_Texture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CShader_Texture::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}

CGameObject* CShader_Texture::Clone(void* pArg)
{
	CShader_Texture* pInstance = new CShader_Texture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShader_Texture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
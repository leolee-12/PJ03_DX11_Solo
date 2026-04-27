#include "SharedTexture_Manager.h"
#include "Texture.h"
#include "Shader.h"

NS_BEGIN(Game_PKM)

namespace
{
	struct SharedTexEntry { SHARED_TEXTURE_TYPE e; const _char* s; };

	constexpr SharedTexEntry kSharedTexTable[] = {
			{ SHARED_TEXTURE_TYPE::MASK,            "MASK"            },
			{ SHARED_TEXTURE_TYPE::NOISE,           "NOISE"           },
			{ SHARED_TEXTURE_TYPE::GRADIENT,        "GRADIENT"        },
			{ SHARED_TEXTURE_TYPE::HIGHLIGHT,       "HIGHLIGHT"       },
	};
}

const _char* To_String(SHARED_TEXTURE_TYPE eType)
{
	for (const auto& entry : kSharedTexTable)
		if (entry.e == eType) return entry.s;
	return "END";
}

SHARED_TEXTURE_TYPE SHARED_TEXTURE_From_String(const _char* psz)
{
	if (nullptr == psz) return SHARED_TEXTURE_TYPE::END;
	for (const auto& entry : kSharedTexTable)
		if (0 == std::strcmp(entry.s, psz)) return entry.e;
	return SHARED_TEXTURE_TYPE::END;
}

IMPLEMENT_SINGLETON(CSharedTexture_Manager)

CSharedTexture_Manager::CSharedTexture_Manager()
{
}

HRESULT CSharedTexture_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

HRESULT CSharedTexture_Manager::Register_TextureGroup(SHARED_TEXTURE_TYPE eType, const _tchar* pszFilePath, _uint iNumTextures)
{
	if (eType >= SHARED_TEXTURE_TYPE::END) return E_FAIL;
	Safe_Release(m_SharedTextures[ETOUI(eType)]);

	CTexture* pTex = CTexture::Create(m_pDevice, m_pContext, pszFilePath, iNumTextures);
	if (!pTex) return E_FAIL;

	m_SharedTextures[ETOUI(eType)] = pTex;
	return S_OK;
}

HRESULT CSharedTexture_Manager::Bind_Texture(CShader* pShader, SHARED_TEXTURE_TYPE eType, const _string& strShaderVarName, _uint iTextureIndex)
{
	if (nullptr == pShader
		|| eType >= SHARED_TEXTURE_TYPE::END
		|| strShaderVarName.empty())
		return E_FAIL;

	CTexture* pTexture = m_SharedTextures[ETOUI(eType)];
	if (nullptr == pTexture)
		return E_FAIL;

	return pTexture->Bind_ShaderResource(pShader, strShaderVarName.c_str(), iTextureIndex);
}

HRESULT CSharedTexture_Manager::Bind_SharedTextures(CShader* pShader, const vector<UI_SHARED_TEXTURE_BINDING_DESC>& Bindings)
{
	for (const auto& binding : Bindings)
	{
		const SHARED_TEXTURE_TYPE eType = SHARED_TEXTURE_From_String(binding.strSharedTexName.c_str());
		if (SHARED_TEXTURE_TYPE::END == eType)
		{
#ifdef _DEBUG
			OutputDebugStringA(("[SharedTexture] Unknown group: '" + binding.strSharedTexName + "'\n").c_str());
#endif
			return E_FAIL;
		}

		if (nullptr == m_SharedTextures[ETOUI(eType)])
		{
#ifdef _DEBUG
			OutputDebugStringA(("[SharedTexture] Group not registered: '" + binding.strSharedTexName + "'\n").c_str());
#endif
			return E_FAIL;
		}

		if (FAILED(Bind_Texture(pShader, eType, binding.strShaderVarName, binding.iTextureIndex)))
		{
#ifdef _DEBUG
			OutputDebugStringA(("[SharedTexture] Bind failed: group='"	+ binding.strSharedTexName
																		+ "', shaderVar='" + binding.strShaderVarName
																		+ "', index=" + std::to_string(binding.iTextureIndex) + "'\n").c_str());
#endif
			return E_FAIL;
		}
	}

	return S_OK;
}

_bool CSharedTexture_Manager::Has_TextureGroup(SHARED_TEXTURE_TYPE eType) const
{
	if (eType >= SHARED_TEXTURE_TYPE::END)
		return false;
	return nullptr != m_SharedTextures[ETOUI(eType)];
}

void CSharedTexture_Manager::Free()
{
	__super::Free();

	for (auto& pTex : m_SharedTextures)
		Safe_Release(pTex);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

NS_END
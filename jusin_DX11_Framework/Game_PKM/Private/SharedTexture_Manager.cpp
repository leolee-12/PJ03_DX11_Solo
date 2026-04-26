#include "SharedTexture_Manager.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Shader.h"

NS_BEGIN(Game_PKM)

namespace
{
    struct SharedTexEntry { SHARED_TEXTURE e; const _char* s; };

    constexpr SharedTexEntry kSharedTexTable[] = {
            { SHARED_TEXTURE::MASK,            "MASK"            },
            { SHARED_TEXTURE::NOISE,           "NOISE"           },
            { SHARED_TEXTURE::GRADIENT,        "GRADIENT"        },
            { SHARED_TEXTURE::HIGHLIGHT,       "HIGHLIGHT"       },
            { SHARED_TEXTURE::OPACITY,         "OPACITY"         },
            { SHARED_TEXTURE::SHADOW_GRADIENT, "SHADOW_GRADIENT" },
    };
}

const _char* To_String(SHARED_TEXTURE eType)
{
    for (const auto& entry : kSharedTexTable)
        if (entry.e == eType) return entry.s;
    return "END";
}

SHARED_TEXTURE SHARED_TEXTURE_From_String(const _char* psz)
{
    if (nullptr == psz) return SHARED_TEXTURE::END;
    for (const auto& entry : kSharedTexTable)
        if (0 == std::strcmp(entry.s, psz)) return entry.e;
    return SHARED_TEXTURE::END;
}

IMPLEMENT_SINGLETON(CSharedTexture_Manager)

CSharedTexture_Manager::CSharedTexture_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSharedTexture_Manager::Initialize()
{
    // 그룹 등록은 외부(Ready_SharedTextures)가 Register_TextureGroup() 으로 수행한다.
    return S_OK;
}

HRESULT CSharedTexture_Manager::Register_TextureGroup(SHARED_TEXTURE eType, _uint iLevel, WNameID
    strTextureProtoTag)
{
    if (eType >= SHARED_TEXTURE::END)
        return E_FAIL;

    const _uint iIdx = ETOUI(eType);

    Safe_Release(m_TextureGroups[iIdx]);

    CTexture* pTexture = static_cast<CTexture*>(
        m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iLevel, strTextureProtoTag));

    if (nullptr == pTexture)
        return E_FAIL;

    m_TextureGroups[iIdx] = pTexture;
    return S_OK;
}

HRESULT CSharedTexture_Manager::Bind_Texture(CShader* pShader, SHARED_TEXTURE eType, const _string&
    strShaderVarName, _uint iTextureIndex)
{
    if (nullptr == pShader)
        return E_FAIL;

    if (eType >= SHARED_TEXTURE::END)
        return E_FAIL;

    CTexture* pTexture = m_TextureGroups[ETOUI(eType)];
    if (nullptr == pTexture)
        return E_FAIL;

    if (strShaderVarName.empty())
        return E_FAIL;

    return pTexture->Bind_ShaderResource(pShader, strShaderVarName.c_str(), iTextureIndex);
}

HRESULT CSharedTexture_Manager::Bind_SharedTextures(CShader* pShader, const
    vector<UI_SHARED_TEXTURE_BINDING_DESC>& Bindings)
{
    for (const auto& binding : Bindings)
    {
        const SHARED_TEXTURE eType = SHARED_TEXTURE_From_String(binding.strSharedTexName.c_str());
            if (SHARED_TEXTURE::END == eType)
                return E_FAIL;

        if (FAILED(Bind_Texture(pShader, eType, binding.strShaderVarName, binding.iTextureIndex)))
            return E_FAIL;
    }

    return S_OK;
}

_bool CSharedTexture_Manager::Has_TextureGroup(SHARED_TEXTURE eType) const
{
    if (eType >= SHARED_TEXTURE::END)
        return false;
    return nullptr != m_TextureGroups[ETOUI(eType)];
}

void CSharedTexture_Manager::Free()
{
    __super::Free();

    for (auto& pTex : m_TextureGroups)
        Safe_Release(pTex);

    Safe_Release(m_pGameInstance);
}

NS_END
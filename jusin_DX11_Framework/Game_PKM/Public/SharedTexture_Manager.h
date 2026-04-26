#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "SharedTextureBinder.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Game_PKM)

enum class SHARED_TEXTURE : _uint
{
    MASK,
    NOISE,
    GRADIENT,
    HIGHLIGHT,
    OPACITY,
    SHADOW_GRADIENT,
    END
};

const _char* To_String(SHARED_TEXTURE eType);
SHARED_TEXTURE SHARED_TEXTURE_From_String(const _char* psz);

class CSharedTexture_Manager : public CBase, public ISharedTextureBinder
{
	DECLARE_SINGLETON(CSharedTexture_Manager)

private:
	CSharedTexture_Manager();
	virtual ~CSharedTexture_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Register_TextureGroup(SHARED_TEXTURE eType, _uint iLevel, WNameID strTextureProtoTag);
    HRESULT Bind_Texture(CShader* pShader, SHARED_TEXTURE eType, const _string& strShaderVarName, _uint iTextureIndex);
    virtual HRESULT Bind_SharedTextures(CShader* pShader, const vector<UI_SHARED_TEXTURE_BINDING_DESC>& Bindings) override;
    _bool Has_TextureGroup(SHARED_TEXTURE eType) const;

private:
    CGameInstance* m_pGameInstance = { nullptr };
    array<CTexture*, ETOUI(SHARED_TEXTURE::END)> m_TextureGroups{};

private:
	virtual void Free() override;
};

NS_END
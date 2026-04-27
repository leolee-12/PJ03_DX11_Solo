#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "SharedTextureBinder.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
NS_END

NS_BEGIN(Game_PKM)

enum class SHARED_TEXTURE_TYPE	: _uint { MASK, NOISE, GRADIENT, HIGHLIGHT, END };
enum class SHARED_MASK			: _uint { MASK1, END };
enum class SHARED_NOISE			: _uint { TITLE, END };
enum class SHARED_GRADIENT		: _uint { ALPHA, END };
enum class SHARED_HIGHLIGHT		: _uint { HIGHLIGHT1, END };

const _char* To_String(SHARED_TEXTURE_TYPE eType);
SHARED_TEXTURE_TYPE SHARED_TEXTURE_From_String(const _char* psz);

class CSharedTexture_Manager : public CBase, public ISharedTextureBinder
{
	DECLARE_SINGLETON(CSharedTexture_Manager)

private:
	CSharedTexture_Manager();
	virtual ~CSharedTexture_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Register_TextureGroup(SHARED_TEXTURE_TYPE eType, const _tchar* pszFilePath, _uint iNumTextures = 1);
	HRESULT Bind_Texture(CShader* pShader, SHARED_TEXTURE_TYPE eType, const _string& strShaderVarName, _uint iTextureIndex);
	virtual HRESULT Bind_SharedTextures(CShader* pShader, const vector<UI_SHARED_TEXTURE_BINDING_DESC>& Bindings) override;
	_bool Has_TextureGroup(SHARED_TEXTURE_TYPE eType) const;

private:
	ID3D11Device* m_pDevice = {nullptr};
	ID3D11DeviceContext* m_pContext = {nullptr};
	array<CTexture*, ETOUI(SHARED_TEXTURE_TYPE::END)> m_SharedTextures{};

private:
	virtual void Free() override;
};

NS_END
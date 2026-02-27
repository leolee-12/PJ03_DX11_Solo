#pragma once
#include "Component.h"

#ifdef _DEBUG
#ifndef TEXTURE_COMP_DEBUG
#define TEXTURE_COMP_DEBUG 0
#endif  
#endif // _DEBUG




BEGIN(Engine)

class CTexture_Manager;

/// <summary>
/// 텍스처를 저장하는 컴포넌트.
/// 텍스처를 로드할 때 텍스처 매니저가 있다면
/// 텍스처를 등록해준다.
/// 현재 텍스처를 불러오는 기능 자체는 없어 텍스처 매니저에 의존하지만 추후
/// 컴포넌트 내부에서 기능을 제작할 예정이고, 매니저에 전역적으로 등록 시키는 방식으로 갈 예정이다.
/// 
/// 내부에 등록해두고 사용되는 컴포넌트이다.
/// 외부에서 텍스처에 대한 정보를 얻고 싶다면 포함한 객체에서 함수를 정의하여 얻도록 제작한다.
/// </summary>
class ENGINE_DLL CTextureComponent : public CComponent
{
	DERIVED_CLASS(CComponent, CTextureComponent)

public:

protected:
	explicit CTextureComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CTextureComponent(const CTextureComponent& rhs);
	virtual ~CTextureComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	static shared_ptr<CTextureComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr);

protected:
	virtual void	Free();


public:
	// 단일 SRV를 얻는 함수 [완]
	ID3D11ShaderResourceView*	Get_ShaderResourseView(const _uint iIndex);
	// 텍스처 배열을 받아 해당 배열에 SRV를 넘겨주는 함수 [검증중]
	HRESULT						Get_ShaderResourceViews(ID3D11ShaderResourceView** Arr, const _uint iNumTextures);

public:
	// 텍스처를 바인드한다. 동시에 텍스처 매니저에 등록도 한다. [완]
	HRESULT Ready_Texture(const wstring& strFilePath, const _uint iNumTextures = 1, _bool bUseMainPath = true);
	// 텍스처 정리하기 [완]
	HRESULT Release_Texture();

public:
	// 이펙트에 텍스처 바인드 [완]
	HRESULT Bind_SRVToEffect(class CShader* pShader, const _char* pTextureSymbolName, const _uint iIndex);

private:
	// 텍스처 매니저에 연결하는 내부 함수 [완]
	HRESULT Link_TextureManager();


private:
	// S접두어는 심볼이라는 뜻
	using SShaderResourceViews = vector<ComPtr<ID3D11ShaderResourceView>>;
	CTexture_Manager*		m_pTexture_Manager = { nullptr };	// 전역 텍스처 리소스 관리에 쓰인다.

	_uint					m_iNumTextures = 0;
	SShaderResourceViews	m_SRVs = { nullptr };		// 실제 렌더링 되는 텍스처의 주소이다.
};

END
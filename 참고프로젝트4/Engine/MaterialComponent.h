#pragma once

#include "Component.h"
#include "TextureComponent.h"



BEGIN(Engine)

/// <summary>
/// 머터리얼을 관리할 수 있는 컴포넌트
/// 모델로부터 머터리얼 정보를 로드하여 정해진 머터리얼의 텍스처 정보를 여기에 저장한다.
/// </summary>
class ENGINE_DLL CMaterialComponent final : public CComponent
{
	INFO_CLASS(CMaterialComponent, CComponent)
protected:
	explicit CMaterialComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CMaterialComponent(const CMaterialComponent& rhs);
	virtual ~CMaterialComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	static shared_ptr<CMaterialComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;

protected:
	virtual void	Free() override;

public:
	const static _uint	g_iNumTextures = TEXTURETYPE_MAX;

public:
	// 모델에 바인딩 된 텍스처를 준비하는 함수이다.
	HRESULT Ready_Materials_ByModel(const wstring& strModelKey, _uint iMesh_MatIndex);

	// 커스텀으로 머터리얼에 텍스처를 바인딩하는 함수이다. 텍스처경로, 로드하여 넣게될 머터리얼 인덱스, 로드할 텍스쳐의 양을 설정 가능하다.
	// 이펙트 담당자가 쓰면 좋다
	HRESULT Ready_CustomSingleMaterial(_uint iMatIndex, const wstring& strTexturePath, _uint iNumTextures);

	// 해당 인덱스의 텍스처로부터 SRV를 얻어오는 함수
	ID3D11ShaderResourceView* Find_SRV(_uint iMatIndex, _uint iTexIndex);
	ID3D11ShaderResourceView* Find_SRV(TEXTURETYPE eIndex, _uint iTexIndex) { return Find_SRV(ECast(eIndex), iTexIndex); }

	// 텍스처를 셰이더에 바인드, 외부로부터 셰이더를 받아 지정한 텍스처를 GPU에 바인드한다.
	HRESULT Bind_SRVToShader(class CShader* pShader, const _char* pConstantName, _uint iMatIndex, _uint iTexIndex,_bool bCustom = false);

	// 여러 텍스쳐를 바인드할 때 사용
	HRESULT Bind_SRVToShaders(class CShader* pShader, const _char* pConstantName, _uint iTexNum);

	ID3D11ShaderResourceView* Find_DefaultSRV(_uint iMatIndex);

private:
	array< shared_ptr<class CTextureComponent>, g_iNumTextures>	m_arrTextureComps = {};
	
};

END
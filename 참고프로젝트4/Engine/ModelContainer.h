#pragma once

#include "Base.h"

#include "MeshContainer.h"
#include "BoneContainer.h"
#include "BoneAnimContainer.h"
#include "MaterialContainer.h"

BEGIN(Engine)

/// <summary>
/// 어떤 모델에 대한 그룹
/// </summary>
class ENGINE_DLL FModelData final : public CBase
{
	DERIVED_CLASS(CBase, FModelData)
public:
	enum TYPE { TYPE_MODEL, TYPE_ANIM_MODEL };

private:
	explicit FModelData() = default;
	explicit FModelData(const FModelData& rhs) = delete;
	virtual ~FModelData() = default;

public:
	HRESULT Initialize();

public:
	static FModelData* Create(const _bool bLoaded);

private:
	virtual void Free() override;

public:
	FMeshData*		Find_Mesh(const _uint iIndex);
	FMeshData*		Find_Mesh(const wstring& strMesh, const _uint iRangeIndex);

	FMaterialData*	Find_Material(const _uint iIndex);
	FMaterialData*	Find_Material(const wstring& strMaterial);

	CBoneGroup*		Find_BoneGroup();

	FBoneAnimData*	Find_BoneAnim(const _uint iIndex);
	FBoneAnimData*	Find_BoneAnim(const wstring& strBoneAnim);

public:
	_uint_64				iHashID = { 0 };					// 해쉬 아이디, 삭제시 매니저에서 해시저장소에 접근
	TYPE					eModelType = { TYPE_MODEL };
	class CMeshGroup*		pMeshGroup = { nullptr };			// 메쉬를 모아놓은 그룹
	class CMaterialGroup*	pMaterialGroup = { nullptr };		// 재질 정보 그룹
	class CBoneGroup*		pBoneGroup = { nullptr };			// 뼈 정보 그룹
	class CBoneAnimGroup*	pAnimGroup = { nullptr };			// 애니메이션 그룹


public:
	HRESULT Create_VTF_Texture();
	HRESULT Bind_ShaderResources(class CShader* pShader);
	HRESULT Bind_VTFBuffers();
	HRESULT Render_ToVTF(class CShader* pShader);

private:
	ComPtr<ID3D11Texture2D>				m_pVertexTexture = { nullptr };	// VTF용 텍스처
	ComPtr<ID3D11ShaderResourceView>	m_pVT_SRV = { nullptr };		// VTF 셰이더 리소스
	ComPtr<ID3D11RenderTargetView>		m_pVT_RTV = { nullptr };		// VTF 렌더타겟

	_uint								m_iStride = { 0 };
	D3D11_PRIMITIVE_TOPOLOGY			m_eVTF_Topology = { D3D11_PRIMITIVE_TOPOLOGY_POINTLIST };	// VTF는 이거 고정
	
};

END
#pragma once

#include "Component.h"
#include "Shader.h"

BEGIN(Engine)

/// <summary>
/// CommonModelComp에 쓸 수 있도록 만들어진 버퍼 컴포넌트
/// </summary>
class ENGINE_DLL CMeshComp : public CComponent
{
	DERIVED_CLASS(CComponent, CMeshComp)

protected:
	explicit CMeshComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CMeshComp(const CMeshComp& rhs);
	virtual ~CMeshComp() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* Arg = nullptr) override;

public:
	static shared_ptr<CMeshComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr);

protected:
	virtual void	Free() override;

public:
	_bool	IsMeshLoaded() const { return (m_pMeshData != nullptr); }
	_uint	Get_MeshMaterialIndex() const;
	HRESULT Load_Mesh(const wstring& strModelFilePath, _uint iIndex);
	// 이펙트에 뼈 행렬 전달 (상수 버퍼 버전)
	HRESULT	Bind_BoneMatricesToShader(CShader* pShader, const _char* pConstantName, const class CBoneGroup& BoneGroup);
	HRESULT Bind_VIBuffer();
	HRESULT Render_VIBuffer();
	HRESULT Bind_VTFBuffer();
	HRESULT Render_VTFBuffer();
	_bool Intersect_MousePos(SPACETYPE eSpacetype, _float3* pOut, _matrix matWorld, _float* pLengthOut);

private:
	class FMeshData* m_pMeshData = { nullptr };

public:
	GETSET_EX1(FMeshData*, m_pMeshData, MeshData,GET);

};

END
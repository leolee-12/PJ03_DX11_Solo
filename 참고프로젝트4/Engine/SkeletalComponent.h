#pragma once

#include "Component.h"

BEGIN(Engine)

enum class EModelGroupIndex : _uint;
class CBoneGroup;

/// <summary>
/// 스켈레톤을 관리할 수 있는 컴포넌트
/// 하나의 인스턴스화 된 스켈레톤 그룹을 가진다.
/// 이 컴포넌트는 메쉬 컴포넌트의 부품으로 취급된다.
/// 뼈는 복제하여 사용한다.
/// </summary>
class ENGINE_DLL CSkeletalComponent final : public CComponent
{
	DERIVED_CLASS(CComponent, CSkeletalComponent)
	
protected:
	explicit CSkeletalComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CSkeletalComponent(const CSkeletalComponent& rhs);
	virtual ~CSkeletalComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* Arg = nullptr) override;
	
public:
	static shared_ptr<CSkeletalComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) override;

protected:
	virtual void	Free() override;

public:
	// 뼈를 로드한다.
	HRESULT		Load_Skeletal(const wstring& strModelFilePath);

	// 최종 트랜스폼의 주소를 저장한 벡터를 내보내, 버퍼에 전달 할 수 있도록 해준다.
	CBoneGroup* Get_BoneGroup();
	void		Invalidate_BoneTransforms();
	_float4x4	Get_BoneTransformFloat4x4(_uint iIndex);
	_matrix		Get_BoneTransformMatrix(_uint iIndex);

	_matrix Get_BoneTransformMatrix(const wstring& strBoneName);

private:	// 뼈 정보
	class CBoneGroup* m_pBoneGroup = { nullptr };		// 모든 뼈에 대한 정보를 가지고 있는 객체
	
};

END
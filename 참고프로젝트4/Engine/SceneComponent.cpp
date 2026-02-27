#include "SceneComponent.h"

#include "GameInstance.h"

CSceneComponent::CSceneComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Base(pDevice, pContext)
{
	// 필수 컴포넌트 내부 생성
	NULL_CHECK(m_pTransformComp = CTransform::Create(pDevice, pContext));
}

CSceneComponent::CSceneComponent(const CSceneComponent& rhs)
	: Base(rhs)
{
	m_pTransformComp = static_pointer_cast<CTransform>(rhs.m_pTransformComp->Clone(nullptr));
}

HRESULT CSceneComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSceneComponent::Initialize(void* Arg)
{
	return S_OK;
}

void CSceneComponent::Priority_Tick(const _float& fTimeDelta)
{
}

void CSceneComponent::Tick(const _float& fTimeDelta)
{
	// 여기에 자동 함수 추가

}

void CSceneComponent::Late_Tick(const _float& fTimeDelta)
{
}

void CSceneComponent::Free()
{
	__super::Free();
}

CSceneComponent* CSceneComponent::Get_FirstChildSceneComp()
{
	if (m_vecChildSceneComp.empty())
		return nullptr;

	return m_vecChildSceneComp.front();
}

CSceneComponent* CSceneComponent::Get_ChildSceneComp(_uint iIndex)
{
	if (m_vecChildSceneComp.empty())
		return nullptr;

	if (iIndex < 0 || iIndex >= Cast<_uint>(m_vecChildSceneComp.size()))
		return nullptr;

	return m_vecChildSceneComp[iIndex];
}

void CSceneComponent::Add_ChildSceneComp(CSceneComponent* const pComp)
{
	if (nullptr == pComp)
		return;

	m_vecChildSceneComp.push_back(pComp);
}

_bool CSceneComponent::Insert_ChildSceneComp(_uint iIndex, CSceneComponent* const pComp)
{
	if (nullptr == pComp)
		return false;

	if (iIndex < 0 || iIndex >= Cast<_uint>(m_vecChildSceneComp.size()))
		return false;

	auto iter = m_vecChildSceneComp.begin() + iIndex;
	m_vecChildSceneComp.insert(iter, pComp);

	return true;
}

_matrix CSceneComponent::Calculate_TransformMatrixFromParent()
{
	_matrix matCalc = Transform().Get_WorldMatrix();

	if (m_pParentSceneComp)
		matCalc *= Calculate_TransformMatrixFromParent();
	else if (!Get_Owner().expired())
		matCalc *= Get_Owner().lock()->Get_TransformCom().lock()->Get_WorldMatrix();
	
	return matCalc;
}

_float4x4 CSceneComponent::Calculate_TransformFloat4x4FromParent()
{
	_matrix matCalc = Calculate_TransformMatrixFromParent();

	_float4x4 matResult = {};
	XMStoreFloat4x4(&matResult, matCalc);

	return matResult;
}

_matrix CSceneComponent::Calculate_TransformMatrixFromParentNoSelf()
{
	_matrix matCalc = XMMatrixIdentity();

	_float3 vPos = { 0.f,0.f,0.f };
	_float4 vRot = { 0.f,0.f,0.f,1.f };
	// 임시로 만들어서 사용함

	if (m_pParentSceneComp)
		matCalc *= Calculate_TransformMatrixFromParent();
	else if (!Get_Owner().expired())
		matCalc *= Get_Owner().lock()->Get_TransformCom().lock()->Get_WorldMatrix();

	return matCalc;
}

_float4x4 CSceneComponent::Calculate_TransformFloat4x4FromParentNoSelf()
{
	_matrix matCalc = Calculate_TransformMatrixFromParentNoSelf();

	_float4x4 matResult = {};
	XMStoreFloat4x4(&matResult, matCalc);

	return matResult;
}

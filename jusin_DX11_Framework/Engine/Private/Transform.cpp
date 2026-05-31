#include "Transform.h"
#include "GameInstance.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent{ Prototype }
	, m_WorldMatrix{ Prototype.m_WorldMatrix }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	auto pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fRotationPerSec = pDesc->fRotationPerSec;
	m_fSpeedPerSec = pDesc->fSpeedPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstName)
{
	return pShader->Bind_Matrix(pConstName, &m_WorldMatrix);
}

HRESULT CTransform::Bind_ShaderResourceWIT(CShader* pShader, const _char* pConstName)
{
	_float4x4 WITMatrix;
	XMStoreFloat4x4(&WITMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix))));
	return pShader->Bind_Matrix(pConstName, &WITMatrix);
}

HRESULT XM_CALLCONV CTransform::Bind_ShaderResourceCombinedWIT(CShader* pShader, const _char* pConstName, _fmatrix CombinedMatrix)
{
	_float4x4 WITMatrix;
	XMStoreFloat4x4(&WITMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, CombinedMatrix)));
	return pShader->Bind_Matrix(pConstName, &WITMatrix);
}

void CTransform::ScaleTo(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fScaleY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * fScaleX);
	Set_State(STATE::UP, Get_State(STATE::UP) * fScaleY);
	Set_State(STATE::LOOK, Get_State(STATE::LOOK) * fScaleZ);
}

void XM_CALLCONV CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3 vScaled = Get_Scaled();

	_vector vRight	= XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector vUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector vLook	= XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fRotationX, _float fRotationY, _float fRotationZ)
{
	_float3 vScaled = Get_Scaled();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix RotationMatrix = XMMatrixRotationQuaternion(
		XMQuaternionRotationRollPitchYaw(fRotationX, fRotationY, fRotationZ));

	Set_State(STATE::RIGHT,	XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP,	XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK,	XMVector3TransformNormal(vLook, RotationMatrix));
}

void XM_CALLCONV CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_float3 vScale = Get_Scaled();
	_vector vRight	= XMVector3Normalize(Get_State(STATE::RIGHT));
	_vector vUp		= XMVector3Normalize(Get_State(STATE::UP));
	_vector vLook	= XMVector3Normalize(Get_State(STATE::LOOK));

	_matrix matOldRot{};
	matOldRot.r[0] = vRight;
	matOldRot.r[1] = vUp;
	matOldRot.r[2] = vLook;
	matOldRot.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_vector qCur = XMQuaternionRotationMatrix(matOldRot);
	_vector qDelta = XMQuaternionRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
	_vector qNew = XMQuaternionNormalize(XMQuaternionMultiply(qCur, qDelta));

	_matrix matNewRot = XMMatrixRotationQuaternion(qNew);
	Set_State(STATE::RIGHT, matNewRot.r[0] * vScale.x);
	Set_State(STATE::UP,	matNewRot.r[1] * vScale.y);
	Set_State(STATE::LOOK,	matNewRot.r[2] * vScale.z);
}

void XM_CALLCONV CTransform::LookAt(_fvector vAt)
{
	_float3 vScaled = Get_Scaled();

	_vector vLook	= vAt - Get_State(STATE::POSITION);
	_vector vRight	= XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	if(XMVectorGetX(XMVector3Length(vRight)) < 1e-6f)
		vRight = XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), vLook);

	_vector vUp		= XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition	= Get_State(STATE::POSITION);
	_vector vLook		= Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
		vPosition = pNavigation->Compute_SlidePos(Get_State(STATE::POSITION), vPosition);

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Move_Delta(_fvector vLocalDelta, CNavigation* pNavigation, _bool bSnapY)
{
	// 1) 스케일 분리된 회전행렬로 delta -> 월드
	_vector vRight	= XMVector3Normalize(Get_State(STATE::RIGHT));
	_vector vUp		= XMVector3Normalize(Get_State(STATE::UP));
	_vector vLook	= XMVector3Normalize(Get_State(STATE::LOOK));

	_matrix matRot{};
	matRot.r[0] = vRight;
	matRot.r[1] = vUp;
	matRot.r[2] = vLook;
	matRot.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_vector vDesired = Get_State(STATE::POSITION) + XMVector3TransformNormal(vLocalDelta, matRot);

	// 2) XZ 셀 판정 (셀 전이 갱신 포함, 지상/공중 공통)
	if (nullptr != pNavigation)
		vDesired = pNavigation->Compute_SlidePos(Get_State(STATE::POSITION), vDesired);

	// 3) Y 처리 (셀 기반)
	if (bSnapY && nullptr != pNavigation)
		Set_State(STATE::POSITION, pNavigation->Compute_Height(vDesired));	// 셀의 Y값 사용
	else
		Set_State(STATE::POSITION, vDesired);	// 루트모션의 Y 이동량 사용
}

void XM_CALLCONV CTransform::Chase(_fvector vGoal, _float fTimeDelta, _float fLimit, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vDir = vGoal - vPosition;
	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance >= fLimit)
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
		vPosition = pNavigation->Compute_SlidePos(Get_State(STATE::POSITION), vPosition);

	Set_State(STATE::POSITION, vPosition);
}

void XM_CALLCONV CTransform::Face_Direction(_fvector vCurLook, _fvector vTargetDir, _float fTimeDelta)
{
	if (XMVectorGetX(XMVector3LengthSq(vTargetDir)) < 1e-6f)
		return;

	_vector vTgtLook = XMVector3Normalize(XMVectorSet(XMVectorGetX(vTargetDir), 0.f, XMVectorGetZ(vTargetDir), 0.f));

	// 이미 정렬된 상태면 조기 종료
	_float fDot = XMVectorGetX(XMVector3Dot(vCurLook, vTgtLook));
	if (fDot > 0.9999f)
		return;

	_float t = 1.f - expf(-m_fRotationPerSec * fTimeDelta);
	_vector vNewLook = XMVector3Normalize(XMVectorLerp(vCurLook, vTgtLook, t));
	
	_float3 vScale = Get_Scaled();
	_vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vNewRight = XMVector3Cross(vWorldUp, vNewLook);
	if (XMVectorGetX(XMVector3LengthSq(vNewRight)) < 1e-6f)
		return;

	vNewRight = XMVector3Normalize(vNewRight);
	_vector vNewUp = XMVector3Normalize(XMVector3Cross(vNewLook, vNewRight));

	Set_State(STATE::RIGHT, vNewRight * vScale.x);
	Set_State(STATE::UP, vNewUp * vScale.y);
	Set_State(STATE::LOOK, vNewLook * vScale.z);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}

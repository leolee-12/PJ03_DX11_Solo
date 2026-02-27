#include "Transform.h"
#include "Shader.h"
#include "CommonModelComp.h"
#include "AnimationComponent.h"
#include "GameObject.h"
#include "PhysX_Controller.h"


IMPLEMENT_CLONE(CTransform, CComponent)
IMPLEMENT_CREATE(CTransform)

CTransform::CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}


void CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	if (fScaleX == 0 || fScaleY == 0 || fScaleZ == 0)
		return;

	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fScaleZ);

	if (isnan(m_WorldMatrix.m[0][0]))
	{
		int a = 0;
	}
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

void CTransform::Move_To_AnimationPos(weak_ptr<class CCommonModelComp> pModel, _float fTimeDelta)
{
	//Set_State(STATE_POSITION, Get_State(STATE_POSITION) + vPos);

	//_matrix WorldMatrix = XMLoadFloat3(vPos);
	//XMStoreFloat3(&m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][0] + vPos);

	if (m_bMove_AnimationPosition)
	{
		_vector vRootPosition = pModel.lock()->Get_RootTransDelta_Vector();
		_vector vPosition = Get_State(STATE_POSITION);
		_vector vAddPosition = XMVectorZero();
		_float3 vPrevAnimationPosition = m_vAnimationPosition;

		if (!pModel.lock()->IsAnimation_Transition() && !pModel.lock()->IsAnimation_Finished())
		{
			_vector vLook = Get_State(STATE_LOOK);
			_vector vRight = Get_State(STATE_RIGHT);
			_vector vUp = Get_State(STATE_UP);

			_vector vMoveX = XMVectorGetX(vRootPosition) * XMVector3Normalize(vRight);
			_vector vMoveZ = XMVectorGetZ(vRootPosition) * XMVector3Normalize(vLook);
			_vector vMoveY = XMVectorGetY(vRootPosition) * XMVector3Normalize(vUp);

			vAddPosition = (vMoveX + vMoveZ + vMoveY);
		}

		Set_State(STATE_POSITION, vPosition + vAddPosition);
		if (m_pPhysX_ControllerCom.lock())
		{
			PxControllerFilters filters;
			m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
		}
	}	
}

//_bool CTransform::Check_Navigation(_vector vPosition, _float3* vLine)
//{
//	CNavigation* pNavigation = m_pOwner.lock()->Get_NavigationCom();
//	if (pNavigation != nullptr)
//	{
//		if (false == pNavigation->isMove(vPosition, vLine))
//			return false;		
//	}
//	return true;
//}
//
//_bool CTransform::Adjust_HeightOnLand(_vector& vPosition)
//{
//	CNavigation* pNavigation = m_pOwner.lock()->Get_NavigationCom();
//	if (pNavigation != nullptr)
//	{
//		_float fHeight = pNavigation->Get_HeightOnNavigation(vPosition);
//		if (fabs(XMVectorGetY(vPosition) - fHeight) <= 0.1f || XMVectorGetY(vPosition) < fHeight)
//		{
//			vPosition = XMVectorSetY(vPosition, fHeight + m_fFloatingY);
//			return (m_bIsOnGround = true);
//		}
//	}
//	return (m_bIsOnGround = false);
//}

void CTransform::Add_Position(_float fTimeDelta, _fvector vAddPosition)
{	
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vNextPosition = XMVectorAdd(vPosition, vAddPosition);
	vPosition.m128_f32[3] = 1.f;

	if (m_pPhysX_ControllerCom.lock())
	{
		PxControllerFilters filters;
		m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
	}

	Set_State(CTransform::STATE_POSITION, vNextPosition);

	return;
	
}


void CTransform::Add_Position(_fvector vAddPosition)
{
	
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vNextPosition = XMVectorAdd(vPosition, vAddPosition);

	vNextPosition = XMVectorSetW(vNextPosition, 1.f);
	if (m_pPhysX_ControllerCom.lock())
	{
		m_pPhysX_ControllerCom.lock()->Set_Position(vNextPosition);
	}
	Set_State(CTransform::STATE_POSITION, vNextPosition);
	

	return;
}

_bool CTransform::Set_Position(_float fTimeDelta, _fvector vNewPosition)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vNewPos = XMVectorSetW(vNewPosition, 1.f);

	_float3 vLine = {};

	if (m_pPhysX_ControllerCom.lock())
	{
		m_pPhysX_ControllerCom.lock()->Set_Position(vNewPos);
	}
	Set_State(CTransform::STATE_POSITION, vNewPos);

	return true;
}


void CTransform::Slide(_float fTimeDelta, _vector vPosition, _vector vLineDir, _vector vMoveDir)
{
	_vector vRotatePosition = {};

	_float fDotMoveLine = XMVectorGetX(XMVector3Dot(vMoveDir, vLineDir));

	fTimeDelta *= fabsf(fDotMoveLine);

	if (0.f < fDotMoveLine)
	{
		vRotatePosition = vPosition + XMVector3Normalize(vLineDir) * fTimeDelta * m_fSpeedPerSec * m_fAdjustSpeed * 0.5f;
	}
	else
	{
		vRotatePosition = vPosition + -XMVector3Normalize(vLineDir) * fTimeDelta * m_fSpeedPerSec * m_fAdjustSpeed * 0.5f;
	}

	_float3 vLine = {};
	XMStoreFloat3(&vLine, vLineDir);
	/*if (!Check_Navigation(vRotatePosition, &vLine))
	{
		return;
	}*/

	Set_State(CTransform::STATE_POSITION, vRotatePosition);
}

_bool CTransform::Go_Straight(_float fTimeDelta, _Out_ _float3* vAddPos)
{
	if (m_bMove)
	{
		_vector vLook = Get_State(STATE_LOOK);
		_vector vPosition = Get_State(STATE_POSITION);
		_vector vAddPosition = XMVector3Normalize(vLook) * m_fSpeedPerSec * m_fAdjustSpeed * fTimeDelta;
		
		if (m_pPhysX_ControllerCom.lock())
		{
			PxControllerFilters filters;
			m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
		}
		Set_State(STATE_POSITION, vPosition + vAddPosition);
		if (vAddPos)
			*vAddPos = vAddPosition;
	}
	return true;
}

_bool CTransform::Go_Left(_float fTimeDelta, _Out_ _float3* vAddPos)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vAddPosition = ( - XMVector3Normalize(vRight) * m_fSpeedPerSec * m_fAdjustSpeed * fTimeDelta);

	if (m_pPhysX_ControllerCom.lock())
	{
		PxControllerFilters filters;
		m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
	}
	Set_State(STATE_POSITION, vPosition + vAddPosition);
	if (vAddPos)
		*vAddPos = vAddPosition;
	return true;
}

_bool CTransform::Go_Right(_float fTimeDelta, _Out_ _float3* vAddPos)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vAddPosition = XMVector3Normalize(vRight) * m_fSpeedPerSec * m_fAdjustSpeed * fTimeDelta;

	if (m_pPhysX_ControllerCom.lock())
	{
		PxControllerFilters filters;
		m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
	}
	Set_State(STATE_POSITION, vPosition + vAddPosition);
	if (vAddPos)
		*vAddPos = vAddPosition;
	return true;
}

_bool CTransform::Go_Backward(_float fTimeDelta, _Out_ _float3* vAddPos)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vAddPosition = ( - XMVector3Normalize(vLook) * m_fSpeedPerSec * m_fAdjustSpeed * fTimeDelta);

	if (m_pPhysX_ControllerCom.lock())
	{
		PxControllerFilters filters;
		m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
	}
	Set_State(STATE_POSITION, vPosition + vAddPosition);
	if (vAddPos)
		*vAddPos = vAddPosition;
	return true;
}

void CTransform::Go_Up(_float fTimeDelta, _Out_ _float3* vAddPos)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = { 0.f,1.f,0.f,0.f };

	_vector vAddPosition = XMVector3Normalize(vUp) * m_fSpeedPerSec * m_fAdjustSpeed * fTimeDelta;

	if (m_pPhysX_ControllerCom.lock())
	{
		PxControllerFilters filters;
		m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
	}
	Set_State(STATE_POSITION, vPosition + vAddPosition);
	if (vAddPos)
		*vAddPos = vAddPosition;
}

void CTransform::Go_Down(_float fTimeDelta, _Out_ _float3* vAddPos)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = { 0.f,1.f,0.f,0.f };

	_vector vAddPosition = ( - XMVector3Normalize(vUp) * m_fSpeedPerSec * m_fAdjustSpeed * fTimeDelta );

	if (m_pPhysX_ControllerCom.lock())
	{
		PxControllerFilters filters;
		m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
	}
	Set_State(STATE_POSITION, vPosition + vAddPosition);
	if (vAddPos)
		*vAddPos = vAddPosition;
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * m_fAdjustSpeed * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScale = Get_Scaled();
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);


	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation_Reset(_fvector vAxis, _float fRadian)
{
	_float3		vScale = Get_Scaled();
	_vector	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_float3 fScale = Get_Scaled();

	_vector	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScale.x;
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScale.y;
	_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScale.z;

	_vector vRot = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);
	_matrix matRot = XMMatrixRotationQuaternion(vRot);

	Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, matRot));
	Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, matRot));
	Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, matRot));
}

void CTransform::Rotation_Quaternion(_fvector vQuaternion)
{
	_float3     vScale = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scaled().x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scaled().y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scaled().z;

	_matrix     RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	vRight = XMVector3Normalize(vRight);
	vUp = XMVector3Normalize(vUp);
	vLook = XMVector3Normalize(vLook);

	Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	Set_State(CTransform::STATE_UP, vUp * vScale.y);
	Set_State(CTransform::STATE_LOOK, vLook * vScale.z);
}

_bool CTransform::Rotate_On_BaseDir(_float fTimeDelta, _float fRadian)
{
	_float3		vScale = Get_Scaled();
	_vector vPrevLook = Get_State(STATE_LOOK);

	_matrix DestRotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	_vector vDestLook = XMVector3TransformNormal(XMLoadFloat3(&m_vBaseDir) * vScale.z, DestRotationMatrix);

	if (XMVectorGetX(XMVector3Length(vPrevLook - vDestLook)) > 0.5f)
	{
		_float		fTurnDir = XMVectorGetX(XMVector3Dot(XMVector3Cross(vPrevLook, vDestLook), XMVectorSet(0.f, 1.f, 0.f, 0.f))) > 0.f ? 1.f : -1.f;
		_vector		vRight = Get_State(STATE_RIGHT);
		_vector		vUp = Get_State(STATE_UP);

		_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTurnDir * fTimeDelta * m_fRotationPerSec * m_fAdjustSpeed);

		Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
		Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
		Set_State(STATE_LOOK, XMVector3TransformNormal(vPrevLook, RotationMatrix));
		return true;
	}
	else
	{
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDestLook)) * vScale.x;
		_vector		vUp = XMVector3Normalize(XMVector3Cross(vDestLook, vRight)) * vScale.y;

		Set_State(STATE_RIGHT, vRight);
		Set_State(STATE_UP, vUp);
		Set_State(STATE_LOOK, vDestLook);
		return false;
	}

}

_bool CTransform::Rotate_On_BaseDir(_float fTimeDelta, _float3 vDestDir)
{
	_float3	vScale = Get_Scaled();
	_vector vPrevLook = Get_State(STATE_LOOK);
	_vector vDestLook = XMVector3Normalize(XMLoadFloat3(&vDestDir)) * vScale.z;
	_vector vAxis = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vBaseDir), vDestLook));

	if (XMVector3Equal(vAxis, XMVectorZero()))
		return false;

	if (XMVectorGetX(XMVector3Length(vPrevLook - vDestLook)) > 0.5f)
	{
		_float		fTurnDir = XMVectorGetX(XMVector3Dot(XMVector3Cross(vPrevLook, vDestLook), vAxis)) > 0.f ? 1.f : -1.f;
		_vector		vRight = Get_State(STATE_RIGHT);
		_vector		vUp = Get_State(STATE_UP);

		_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fTurnDir * fTimeDelta * m_fRotationPerSec * m_fAdjustSpeed);

		Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
		Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
		Set_State(STATE_LOOK, XMVector3TransformNormal(vPrevLook, RotationMatrix));
		return true;
	}
	else
	{
		_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDestLook)) * vScale.x;
		_vector		vUp = XMVector3Normalize(XMVector3Cross(vDestLook, vRight)) * vScale.y;

		Set_State(STATE_RIGHT, vRight);
		Set_State(STATE_UP, vUp);
		Set_State(STATE_LOOK, vDestLook);
		return false;
	}
}

void CTransform::Rotate_On_BaseDir(_float fRadian)
{
	_float3		vScale = Get_Scaled();
	_vector vPrevLook = Get_State(STATE_LOOK);

	_matrix DestRotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	_vector vDestLook = XMVector3TransformNormal(XMLoadFloat3(&m_vBaseDir) * vScale.z, DestRotationMatrix);

	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDestLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vDestLook, vRight)) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vDestLook);
}



_float CTransform::Check_Look_On_BaseDir(_float fRadian)
{
	_float3		vScale = Get_Scaled();
	_vector		vPrevLook = Get_State(STATE_LOOK);

	_matrix DestRotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	_vector vDestLook = XMVector3TransformNormal(XMLoadFloat3(&m_vBaseDir) * vScale.z, DestRotationMatrix);

	vPrevLook = XMVector3Normalize(vPrevLook);
	vDestLook = XMVector3Normalize(vDestLook);

	_float fAngle;
	if (XMVectorGetY(XMVector3Cross(vPrevLook, vDestLook)) >= 0.f)
		fAngle = acos(XMVectorGetX(XMVector3Dot(vPrevLook, vDestLook)));
	else
		fAngle = XMConvertToRadians(360.f) - acos(XMVectorGetX(XMVector3Dot(vPrevLook, vDestLook)));

	return fAngle;
}

_bool CTransform::Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vDir = vTargetPos - vPosition;

	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance >= fSpare)
	{
		_vector vAddPosition = XMVector3Normalize(vDir) * m_fSpeedPerSec * m_fAdjustSpeed * fTimeDelta;
		
		//Adjust_HeightOnLand(vPosition);
		if (m_pPhysX_ControllerCom.lock())
		{
			PxControllerFilters filters;
			m_pPhysX_ControllerCom.lock()->Move(vAddPosition, 0.f, fTimeDelta, filters);
		}
		Set_State(STATE_POSITION, vPosition + vAddPosition);

		return true;
	}
	else
		return false;
}

void CTransform::Look_At(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	if (XMVector3Equal(vPosition, vTargetPos))
		return;

	_vector		vLook = XMVector3Normalize(vTargetPos - vPosition) * vScale.z;
	vLook = XMVectorSetW(vLook, 0.f);
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	vRight = XMVectorSetW(vRight, 0.f);
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;
	vUp = XMVectorSetW(vUp, 0.f);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Look_At(_fvector vTargetPos, _float fRadianSpeed)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = XMVector3Normalize(vTargetPos - vPosition);
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_vector		vOriginLook = XMVector3Normalize(Get_State(STATE_LOOK));
	_vector		vOriginRight = XMVector3Normalize(Get_State(STATE_RIGHT));
	_vector		vOriginUp = XMVector3Normalize(Get_State(STATE_UP));

	// 같은 방향 바라보면 계산 X
	if (XMVector3Equal(vLook, vOriginLook))
		return;

	_matrix		matRotation(vRight, vUp, vLook, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	_matrix		matOriginRotation(vOriginRight, vOriginUp, vOriginLook, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_vector		vQuaternion = XMQuaternionRotationMatrix(matRotation);
	_vector		vOriginQuaternion = XMQuaternionRotationMatrix(matOriginRotation);

	vQuaternion = XMQuaternionSlerp(vOriginQuaternion, vQuaternion, fRadianSpeed);

	matRotation = XMMatrixRotationQuaternion(vQuaternion);

	matRotation.r[0] *= vScale.x;
	matRotation.r[1] *= vScale.y;
	matRotation.r[2] *= vScale.z;

	Set_State(STATE_RIGHT, matRotation.r[0]);
	Set_State(STATE_UP, matRotation.r[1]);
	Set_State(STATE_LOOK, matRotation.r[2]);
}

void CTransform::Look_At_OnLand(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();
	_vector		vPrevLook = Get_State(STATE_LOOK);
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	if (XMVector3Equal(vPosition, vTargetPos))
		return;

	vLook = XMVectorSetY(vLook, 0.f);
	vLook = XMVector3Normalize(vLook) * vScale.z;
	vLook = XMVectorSetW(vLook, 0.f);

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	vRight = XMVectorSetW(vRight, 0.f);

	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Set_Look(_fvector vLookDir, _bool bUseCamera)
{
	_float3		vScale = Get_Scaled();
	_vector		vLook = {};

	if (true == bUseCamera) {
		vLook = XMVector3Normalize(vLookDir);
	}
	else if (false == bUseCamera) {
		vLook = vLookDir;
		vLook = XMVectorSetY(vLook, 0.f);
		vLook = XMVectorSetW(vLook, 0.f);
		vLook = XMVector3Normalize(vLook);
	}

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_UP, vUp * vScale.y);
	Set_State(STATE_LOOK, vLook * vScale.z);
}

void CTransform::Set_Look(_fvector vLookDir, _fvector vUpDir)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook = XMVector3Normalize(vLookDir);
	vLook = XMVectorSetW(vLook, 0.f);
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	if (!XMVector3Equal(vLook, vUpDir))
		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_UP, vUp * vScale.y);
	Set_State(STATE_LOOK, vLook * vScale.z);
}

void CTransform::Set_Look_Manual(_fvector vLookDir)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook = XMVector3Normalize(vLookDir);
	vLook = XMVectorSetW(vLook, 0.f);

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	if (!XMVector3Equal(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)))
		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_UP, vUp * vScale.y);
	Set_State(STATE_LOOK, vLook * vScale.z);
}

void CTransform::Set_Up(_fvector vDir)
{
	_float3		vScale = Get_Scaled();

	_vector		vUp = XMVector3Normalize(vDir);
	vUp = XMVectorSetW(vUp, 0.f);

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), vUp));
	_vector		vLook = XMVector3Normalize(XMVector3Cross(vUp, vRight));

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_UP, vUp * vScale.y);
	Set_State(STATE_LOOK, vLook * vScale.z);
}


HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}


void CTransform::Write_Json(json& Out_Json)
{
	Out_Json.emplace("Transform", m_WorldMatrix.m);
}

void CTransform::Load_Json(const json& In_Json)
{
	if (In_Json.find("Transform") == In_Json.end())
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_WorldMatrix.m[i][j] = In_Json["Transform"][i][j];
		}
	}
}

void CTransform::Free()
{
	__super::Free();

}
#include "Camera.h"

#include "Transform.h"
#include "GameInstance.h"

CCamera::CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
    : CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
   return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
    if (nullptr == pArg)
        RETURN_EFAIL;
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;  

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
    m_pTransformCom->Look_At(XMLoadFloat4(&pCameraDesc->vAt));

    m_fFovy = pCameraDesc->fFovy;
    m_fAspect = pCameraDesc->fAspect;
    m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;
	m_fMouseSensor = pCameraDesc->fMouseSensor;

    return S_OK;
}

void CCamera::Priority_Tick(_cref_time fTimeDelta)
{
    Chainging_Camera(fTimeDelta);

	if (m_eOffSetCategory != CCamera::OFFSET_NONE)
		Offset_Setting(fTimeDelta);

	Apply_CameraView();
}

void CCamera::Tick(_cref_time fTimeDelta)
{
}

void CCamera::Late_Tick(_cref_time fTimeDelta)
{
}

void CCamera::Before_Render(_cref_time fTimeDelta)
{
	Apply_CameraView();
}

void CCamera::Apply_CameraView()
{
	if (m_bisNowMain)
	{
		m_pGameInstance->Set_Transform(CPipeLine::TS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
		m_pGameInstance->Set_Transform(CPipeLine::TS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

		_float4 vCamPosition, vCamLook;
		XMStoreFloat4(&vCamPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat4(&vCamLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		m_pGameInstance->Set_CamPosition(vCamPosition);
		m_pGameInstance->Set_CamLook(vCamLook);
	}
}

_float4 CCamera::Get_CamLookPoint()
{
	_float4 vCamPosition;
	XMStoreFloat4(&vCamPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    _vector vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    
	_float4 vLookPosition = vCamPosition + vCamLook;
	vLookPosition.w = 1.f;

    return vLookPosition;
}

void CCamera::Chainging_Camera(_cref_time fTimeDelta) //카메라 단순교체
{
	_bool ChangingPos = false, ChangingLook = false;

	if (true == m_bisChanging)
	{
		m_fTrackPosition += fTimeDelta;

		if (m_iINTERType == (_int)CAMERA_MOTION_DESC::INTER_LERP)  // 선형
		{
			_vector			vPos = {}, vLook = {};
			_vector			vSourPos, vSourLook, vDestPos, vDestLook;

			vSourPos = XMVectorSet(m_vNowCamPos.x, m_vNowCamPos.y, m_vNowCamPos.z, 1.f);
			vDestPos = XMVectorSet(m_vChangeCamPos.x, m_vChangeCamPos.y, m_vChangeCamPos.z, 1.f);

			vSourLook = XMVectorSet(m_vNowLook.x, m_vNowLook.y, m_vNowLook.z, 0.f);
			vDestLook = XMVectorSet(m_vChangeLook.x, m_vChangeLook.y, m_vChangeLook.z, 0.f);

			if (m_fTrackPosition >= m_fDuration)
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
				m_pTransformCom->Set_Look(vDestLook, true);

				m_fTrackPosition = m_fDuration;
				ChangingLook = true;
				ChangingPos = true;
			}
			else {		

				_float fRatio = m_fTrackPosition / m_fDuration;

				vPos = XMVectorLerp(vSourPos, vDestPos, fRatio);
				vPos = XMVectorSetW(vPos, 1.f);

				vLook = XMVector3Normalize(XMVectorLerp(vSourLook, vDestLook, fRatio));
				vLook = XMVectorSetW(vLook, 0.f);

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
				m_pTransformCom->Set_Look(vLook, true);
			}
		}

		else if ((m_iINTERType == (_int)CAMERA_MOTION_DESC::INTER_CATROM)//CatRom 스플라인
			|| (m_iINTERType == (_int)CAMERA_MOTION_DESC::INTER_EASING))//EASING
		{
			ChangingLook = true;
			ChangingPos = true;
		}

		else if (m_iINTERType == (_int)CAMERA_MOTION_DESC::INTER_NONE)  // NONE
		{
			m_vNowCamPos = m_vChangeCamPos;
			m_vNowLook = m_vChangeLook;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNowCamPos);
			m_pTransformCom->Set_Look(XMLoadFloat4(&m_vNowLook), true);
			ChangingLook = true;
			ChangingPos = true;
		}

		if ((true == m_bisChanging) && (true == ChangingPos) && (true == ChangingLook))
			m_bisChanging = false;
	}
}

void CCamera::Offset_Setting(_cref_time fTimeDelta)
{
	m_pActorTransformCom = dynamic_pointer_cast<CTransform>(m_pGameInstance->Get_Component(m_pGameInstance->Get_CreateLevelIndex(), L_PLAYER, g_pTransformTag, 0));
	
	if (m_pActorTransformCom != nullptr)
	{
		if (m_eOffSetCategory == (CAMERA_OFFSET_CATEGORY::OFFSET_POS)) {
			XMStoreFloat4(&m_vNowLook, m_pActorTransformCom->Get_State(CTransform::STATE_POSITION));
			//값 담아두기만
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pActorTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vOffset));
		}
		else if (m_eOffSetCategory == (CAMERA_OFFSET_CATEGORY::OFFSET_LOOK)) {
			m_pTransformCom->Look_At(m_pActorTransformCom->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat4(&m_vNowCamPos, m_pActorTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vOffset));
			//값 담아두기만
		}
		else if (m_eOffSetCategory == (CAMERA_OFFSET_CATEGORY::OFFSET_ALL)) {
			m_pTransformCom->Look_At(m_pActorTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pActorTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vOffset));
		}
	}
}

//바뀔 녀석에서 호출한다. A카메라 -> B카메라 변경한다면 B에서 !!
void CCamera::Chainging_Camera_Set(_float4 _PreCamPos, _float4 _PreCamLook, _float _Duration, _int eInterType)
{
	if (false == m_bisChanging)
	{
		if ((eInterType == (_int)CAMERA_MOTION_DESC::INTER_LERP)  //선형 보간
			|| (eInterType == (_int)CAMERA_MOTION_DESC::INTER_EASING)) //EASING 보간
		{
			//바뀔 대상 : B의 원래 Pos/Look
			m_vChangeCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_vChangeLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			//출발 지점 : A의 Pos/Look
			m_vNowCamPos = _PreCamPos;
			m_vNowLook = _PreCamLook;
		}

		else if (eInterType == (_int)CAMERA_MOTION_DESC::INTER_NONE) //보간 없음
		{
			m_vChangeCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_vChangeLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			
			m_vNowCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_vNowLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		}

        m_bisChanging = true;
		m_fDuration = _Duration;
        m_iINTERType = eInterType;
		m_fTrackPosition = 0.f;
	}
}

void CCamera::Free()
{
    __super::Free();
}

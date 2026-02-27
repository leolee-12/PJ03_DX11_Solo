#pragma once

/* 클라이언트 개발자가 만들고자하는 카메라들의 공통적인 부모가 되기위한 클래스다. */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum CAMERA_OFFSET_CATEGORY { //Pos를 대상에 고정할지 Look을 대상에 고정할지
		OFFSET_POS, OFFSET_LOOK, OFFSET_ALL, OFFSET_NONE, OFFSET_END
	};										//일반 월드

	enum CAMERA_OFFSET_MAG { //배율 조절하는 UI 나중에 넣기
		OFFMAG_NEAR, OFFMAG_MIDDLE, OFFMAG_FAR, OFFMAG_END
	}; //   타겟이 있는 경우 활용하는 오프셋



	typedef struct tagCameraDesc : public tagGameObjectDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;
		_float		fMouseSensor = 0.0f;  // For Dynamic,Third
	}CAMERA_DESC;

	typedef struct tagCameraMotionData {
		enum INTERPOLATION_TYPE { INTER_LERP, INTER_CATROM, INTER_EASING, INTER_NONE, INTER_END };

		INTERPOLATION_TYPE	eType;
		int					eEasingType;		//Easing 사용 시의 타입
		XMFLOAT4			vCamEyePos;
		XMFLOAT4			vCamLook;
		//XMFLOAT4			vCamRotate;
		float				fDuration;			//모든 값 통합

		float				fKeyFrame;
		bool				bStarted = false;	//최초 프레임 체크용
		bool				bFollow = false;    //캐릭터 따라가는지 아닌지

		//XMFLOAT4X4			WorldMatrix;
		shared_ptr<CTransform> pActorTransformCom;

	}CAMERA_MOTION_DESC;


protected:
	CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;

public:
	void		Apply_CameraView();
	void		Set_CameraMain(_bool bMainTrueOrFalse) { m_bisNowMain = bMainTrueOrFalse; }
	_float4		Get_CamLookPoint();

public:
	void		Chainging_Camera_Set(_float4 _PreCamPos, _float4 _PreCamLook, _float _Duration, _int eInterType);
	void		Chainging_Camera(_cref_time fTimeDelta);

	void		Offset_Setting(_cref_time fTimeDelta);
	void		Setting_Offset(CAMERA_OFFSET_CATEGORY eCategory, CAMERA_OFFSET_MAG eMag) {
		m_eOffSetCategory = eCategory;
		m_eOffSetMagnification = eMag;
		m_vOffset = m_vOffsetValue[m_eOffSetMagnification];
	}
	CAMERA_OFFSET_CATEGORY Get_OffsetCategory() { return m_eOffSetCategory; }

protected:
	_float				m_fFovy = { 0.f };
	_float				m_fAspect = { 0.0f };
	_float				m_fNear = { 0.0f };
	_float				m_fFar = { 0.0f };

	/*---------Base Setting--------*/
	_int				m_iINTERType = { 0 }; //보간 방식
	_int				m_iEasingType = { 0 };
	_bool				m_bisNowMain = { false };	//지금의 메인 카메라인지
	_bool				m_bisChanging = { false };  //카메라 전환'
public:
	GETSET_EX2(_bool, m_bisChanging, IsChanging, GET, SET)
protected:
	/*--------Target Offset---------*/
	//중점
	//_float4x4				m_WorldMatrix;
	shared_ptr<CTransform>	m_pActorTransformCom; //객체

	CAMERA_OFFSET_CATEGORY  m_eOffSetCategory = { OFFSET_NONE };
	CAMERA_OFFSET_MAG		m_eOffSetMagnification = { OFFMAG_MIDDLE };

	//None이 아닐 경우
	_float3				m_vOffset = {};
	_float3				m_vOffsetValue[3] = { { 0.f, 1.5f, -2.0f } , { 0.f, 2.0f, -2.5f }, { 0.f, 3.0f, -3.5f } };

	/*--------Motion---------*/
	_float				m_fTrackPosition = { 0.f };
	_float				m_fDuration = { 5.f };

	/*--------Change 보간용---------*/
	_float4				m_vNowCamPos = { 0.f, 0.f, 0.f, 1.f };
	_float4				m_vNowLook = { 0.f, 0.f, 0.f, 0.f };

	_float4				m_vChangeCamPos = { 0.f, 0.f, 0.f, 1.f };
	_float4				m_vChangeLook = { 0.f, 0.f, 0.f, 0.f };


	_float				m_fMouseSensor = { 0.0f };
public:
	GETSET_EX1(_float, m_fFovy, Fovy, GET)
	GETSET_EX1(_float, m_fAspect, Aspect, GET)
	GETSET_EX1(_float, m_fNear, Near, GET)
	GETSET_EX1(_float, m_fFar, Far, GET)


public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Main_Camera.h"

BEGIN(Client)

class CVirtual_Camera final : public CCamera
{
public:
	enum CAMERA_MODE
	{
		CAMERA_NORMAL_MODE,
		CAMERA_FREE_MODE,
		CAMERA_MAP_MODE,
		CAMERA_CINEMATIC_MODE,
		CAMERA_MODE_END
	};

	enum CAMERA_PLAY_MODE {
		Playing,
		Paused,
		Stopped
	};

	enum InterpolationType {
		INTERPOLATION_LINEAR_MODE,
		INTERPOLATION_DAMPING_MODE,
		INTERPOLATION_SKIP_MODE,
		INTERPOLATION_END
	};

protected:
	CVirtual_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVirtual_Camera(const CVirtual_Camera& Prototype);
	virtual ~CVirtual_Camera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Add_Point(_float duration, _int type, const _float4x4* pModelFloat4x4, _float damping, _bool hasWorldFloat4x4, _int animationIndex);
	void Add_NormalPoint(_float duration, _int type, const _float4x4* pModelFloat4x4, _float damping, _bool hasWorldFloat4x4, _int animationIndex, CTransform* transform);
	void Remove_Point(_int currentIndex, _int animationIndex);
	void Move_Point(_int index, _int animationIndex);
	void Modify_Transform(_int index, _int animationIndex);
	_float AdjustT_Damping(_float t, _float damping);
	void Delete_Points(_int animationIndex);

	void ApplyCameraShake(_float fTimeDelta);
	void StartCameraShake(_float fDuration, _float fMagnitude);
	void StopCameraShake();

	void Set_CameraMode(CMain_Camera::VIRTUAL_CAMERA cameraMode);
public:
	const _char* GetTabName() const { return m_Name; };
	void Set_Player(CGameObject* pPlayer, CGameObject* pEnemy = nullptr);

	void Start_Play(_int animationIndex, _bool isImguiPlay, CGameObject* gameObject = nullptr, _bool ignoreFlip = false);
	void Pause();
	void Stop();
	void Button_Stop();

	_bool Get_IsPlay()
	{
		return m_currentPlayMode == CAMERA_PLAY_MODE::Playing;
	};

	// 원형 회전 모드 설정 함수
	void SetCirclePlay(_bool isClockwise = true, _float rotationSpeed = XM_2PI / 10.0f);

private:
	void Free_Camera(_float fTimeDelta);
	void Default_Camera(_float fTimeDelta);
	void Map_Camera(_float fTimeDelta);
	void Play(_float fTimeDelta);
	_float ComputeDistanceX(_gvector pos1, _gvector pos2);
	void Set_Camera_Position(_float averageX, _float distanceX, _float higherY, _gvector pos1, _gvector pos2);
	void Set_Camera_Direction(_float averageX, _gvector pos1, _gvector pos2);
	// 원형 회전 함수
	void CirclePlay(_float fTimeDelta, const CameraPoint& currentPoint);


	void Print_Flip_Rotation();
public:
	CAMERA_MODE m_currentMode = { CAMERA_NORMAL_MODE };
	CAMERA_PLAY_MODE m_currentPlayMode = CAMERA_PLAY_MODE::Stopped;

	_char* m_Name;

	_int m_currentPointIndex = { 0 };
	_float m_elapsedTime = { 0.f };

	_float					m_fMouseSensor = {};
	_float					m_fMoveSpeed = {};

	//쉐이킹
	_bool				m_bIsShaking = { false };       // 흔들림 활성화 여부
	_float				m_fShakeDuration = { 3.f };
	_float				m_fElapsedShakeTime = 0.0f; // 흔들림 경과 시간
	_float				m_fShakeMagnitude = { 0.2f };
	_vector				m_vShakeOffset = XMVectorZero();
	_vector				m_vBaseCameraPosition = {};

	class CGameObject* m_p1pPlayer = { nullptr };
	class CGameObject* m_p2pPlayer = { nullptr };
	class CGameObject* m_pEnemy = { nullptr };

	_uint m_iTeam = {};

	_float m_previousFOV = {};

	// 카메라 위치 스무딩을 위한 변수
	_float3 m_previousPosition = _float3(0.f, 17.f, -30.f);

	unordered_map<_int, vector<CameraPoint>> m_mapPoints;
	_int m_AnimationIndex = -1;

	//IMGUI
	_bool m_bIsImguiPlay = { false };

	_bool m_bIsIgnoreFlip = { false };
	_bool m_bIsScaleIgnore = { false };

	// 원형 회전
	_bool m_bIsCirclePlay = false;          // 원형 회전 모드 플래그
	_bool m_bIsClockwise = true;            // 시계 방향 여부
	_float m_circleElapsedTime = 0.0f;      // 원형 회전에서 경과된 시간
	_float m_circleDuration = 10.0f;        // 원형 회전의 총 지속 시간
	_float m_circleAngle = 0.0f;            // 현재 각도
	_float m_circleRadius = 0.0f;           // 원형 회전을 위한 반지름
	_float m_rotationSpeed = XM_2PI / 10.0f; // 회전 속도 (라디안/초)
public:
	/* Map_Camera Dying PlayerTeam Check */
	void Set_DyingTeam(_uint iTeamIndex, _matrix CamWorldMatrix);
	void Set_EastFinish();
	_uint m_isDyingTeam = { 0 };
	_bool m_isDestructive = { false };
	_bool m_isEastFinish = { false };

public:
	static CVirtual_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
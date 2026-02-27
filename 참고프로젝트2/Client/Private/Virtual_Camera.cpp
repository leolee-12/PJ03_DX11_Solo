#include "stdafx.h"
#include "..\Public\Virtual_Camera.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Character.h"
CVirtual_Camera::CVirtual_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CVirtual_Camera::CVirtual_Camera(const CVirtual_Camera& Prototype)
	: CCamera{ Prototype }
{

}

HRESULT CVirtual_Camera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVirtual_Camera::Initialize(void* pArg)
{
	CAMERA_DESC Desc{};

	//초기 위치
	m_vEye = _float3(0.f, 10.f, -10.f);
	//초기 각도
	m_vAt = _float3(0.f, 0.f, 0.f);
	//카메라 이름
	m_Name = *static_cast<_char**>(pArg);
	//카메라 마우스 민감도
	m_fMouseSensor = 0.1f;
	//카메라 이동속도
	Desc.fSpeedPerSec = 1.f;
	//실제 조정할 스피드
	m_fMoveSpeed = Desc.fSpeedPerSec;

	//시야각
	Desc.fFovy = XMConvertToRadians(40.0f);
	//Near
	Desc.fNear = 0.1f;
	//Far
	Desc.fFar = 10000.f;
	//카메라 회전속도
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vEye), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vAt), 1.f));

	m_p1pPlayer = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0);
	m_p2pPlayer = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 1);
	return S_OK;
}

void CVirtual_Camera::Camera_Update(_float fTimeDelta)
{
	switch (m_currentMode)
	{
	case CAMERA_FREE_MODE:
		Free_Camera(fTimeDelta);
		break;
	case CAMERA_NORMAL_MODE:
		Default_Camera(fTimeDelta);
		break;
	case CAMERA_MAP_MODE:
		Map_Camera(fTimeDelta);
		break;
	case CAMERA_CINEMATIC_MODE:
		if (m_currentPlayMode == Playing)
			Play(fTimeDelta);
		break;
	}

	if (m_bIsShaking)
		ApplyCameraShake(fTimeDelta);
}

void CVirtual_Camera::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CVirtual_Camera::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CVirtual_Camera::Render(_float fTimeDelta)
{
	return S_OK;
}

void CVirtual_Camera::Play(_float fTimeDelta)
{
	if (m_currentPlayMode != CAMERA_PLAY_MODE::Playing || m_AnimationIndex == -1)
		return; // 현재 상태가 Playing이 아니면 업데이트하지 않음

	// 현재 애니메이션의 포인트 수 확인
	if (m_currentPointIndex >= m_mapPoints[m_AnimationIndex].size())
	{
		// 모든 포인트의 Duration이 끝나면 Play 모드 종료
		Stop();
		return;
	}

	CGameObject* player = { nullptr };

	if (m_pEnemy == nullptr)
		player = m_iTeam == 1 ? m_p1pPlayer : m_p2pPlayer;
	else
		player = m_pEnemy;

	// 플레이어의 방향 가져오기 (1이면 그대로, -1이면 반전)
	CCharacter* character = static_cast<CCharacter*>(player);
	_int direction = character->Get_iDirection();

	// 현재 포인트와 다음 포인트 설정
	CameraPoint currentPoint = m_mapPoints[m_AnimationIndex][m_currentPointIndex];

	// 원형 회전 모드인지 체크
	if (m_bIsCirclePlay)
	{
		m_circleDuration = currentPoint.duration;
		CirclePlay(fTimeDelta, currentPoint);
	}
	else
	{
		CameraPoint nextPoint = {};

		if (m_currentPointIndex + 1 < m_mapPoints[m_AnimationIndex].size())
			nextPoint = m_mapPoints[m_AnimationIndex][m_currentPointIndex + 1];
		else
			nextPoint = currentPoint;

		m_elapsedTime += fTimeDelta;

		if (m_elapsedTime >= currentPoint.duration)
		{
			// 다음 포인트로 이동
			m_currentPointIndex++;
			m_elapsedTime = 0.0f;

			if (m_currentPointIndex >= m_mapPoints[m_AnimationIndex].size())
			{
				Stop();
				return;
			}

			currentPoint = m_mapPoints[m_AnimationIndex][m_currentPointIndex];
			if (m_currentPointIndex + 1 < m_mapPoints[m_AnimationIndex].size())
				nextPoint = m_mapPoints[m_AnimationIndex][m_currentPointIndex + 1];
			else
				nextPoint = currentPoint;
		}

		// 보간 비율 계산
		float t = m_elapsedTime / currentPoint.duration;

		// 보간 방식에 따른 t 값 조정
		switch (currentPoint.interpolationType)
		{
		case InterpolationType::INTERPOLATION_LINEAR_MODE:
			// t는 그대로 사용
			break;
		case InterpolationType::INTERPOLATION_DAMPING_MODE:
			t = AdjustT_Damping(t, currentPoint.damping);
			break;
		case InterpolationType::INTERPOLATION_SKIP_MODE:
			t = 1.0f;
			break;
		}

		// **1. 로컬 포지션 보간**
		_vector interpolatedPositionLocal;
		if (currentPoint.interpolationType != InterpolationType::INTERPOLATION_SKIP_MODE)
		{
			interpolatedPositionLocal = XMVectorLerp(XMLoadFloat3(&currentPoint.position), XMLoadFloat3(&nextPoint.position), t);
		}
		else
		{
			interpolatedPositionLocal = XMLoadFloat3(&nextPoint.position);
		}

		// **3. 모델의 월드 행렬 로드 (스케일링 포함)**
		_matrix modelWorldMatrix = Float4x4ToMatrix(*currentPoint.pWorldFloat4x4);

		// 좌우 반전을 카메라 단위로 따로 만들어준 녀석은 스케일링 제거해야 함
		if (direction == -1 && m_bIsIgnoreFlip) {
			// **스케일링 제거를 위한 행렬 분해**
			_vector modelScale;
			_vector modelRotationQuat;
			_vector modelTranslation;
			XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, modelWorldMatrix);

			// **스케일링이 제거된 모델의 월드 행렬 재구성**
			_matrix modelRotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
			_matrix modelTranslationMatrix = XMMatrixTranslationFromVector(modelTranslation);
			modelWorldMatrix = modelRotationMatrix * modelTranslationMatrix;
		}


		// **4. 로컬 포지션을 월드 포지션으로 변환 (스케일링 포함)**
		_vector interpolatedPositionWorld = XMVector3TransformCoord(interpolatedPositionLocal, modelWorldMatrix);

		// **2. 로컬 회전 보간 (Quaternion Slerp 사용)**
		_vector interpolatedRotationLocal;
		if (currentPoint.interpolationType != InterpolationType::INTERPOLATION_SKIP_MODE)
		{
			_vector q1 = XMLoadFloat4(&currentPoint.rotation);
			_vector q2 = XMLoadFloat4(&nextPoint.rotation);
			interpolatedRotationLocal = XMQuaternionSlerp(q1, q2, t);
		}
		else
		{
			interpolatedRotationLocal = XMLoadFloat4(&nextPoint.rotation);
		}

		//**direction에 따른 회전 조정**
		//또한 Flip을 무시하는 속성이 false여야 반전시키기
		if (direction == -1 && m_bIsIgnoreFlip == false)
		{
			// 쿼터니언의 Y 성분 반전
			interpolatedRotationLocal = XMVectorSet(
				XMVectorGetX(interpolatedRotationLocal),
				-XMVectorGetY(interpolatedRotationLocal),
				XMVectorGetZ(interpolatedRotationLocal),
				XMVectorGetW(interpolatedRotationLocal));
		}

		// **6. 로컬 회전을 월드 회전으로 변환**
		_matrix interpolatedRotationMatrixLocal = XMMatrixRotationQuaternion(interpolatedRotationLocal);

		// **7. 카메라의 월드 행렬 생성 (스케일링 포함)**
		_matrix NewWorldMatrix = interpolatedRotationMatrixLocal;
		NewWorldMatrix.r[3] = XMVectorSetW(interpolatedPositionWorld, 1.0f); // 위치 설정

		// 방향 벡터 추출
		_vector right = NewWorldMatrix.r[0];
		_vector up = NewWorldMatrix.r[1];
		_vector look = NewWorldMatrix.r[2];

		// 위치 설정 (쉐이크 오프셋 포함)
		_vector position = interpolatedPositionWorld + m_vShakeOffset;

		// Transform 컴포넌트 업데이트
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, right);
		m_pTransformCom->Set_State(CTransform::STATE_UP, up);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, look);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, position);

		//cout << XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) << "  " << XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) << "  " << XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) << endl;

	}
}

// 원형 회전 함수 구현
// 원형 회전 함수 구현
void CVirtual_Camera::CirclePlay(float fTimeDelta, const CameraPoint& currentPoint)
{
	// 모델의 월드 행렬 로드
	XMMATRIX modelWorldMatrix = Float4x4ToMatrix(*currentPoint.pWorldFloat4x4);

	// 좌우 반전에 따른 스케일링 제거
	CCharacter* character = static_cast<CCharacter*>(m_pEnemy != nullptr ? m_pEnemy : (m_iTeam == 1 ? m_p1pPlayer : m_p2pPlayer));
	int direction = character->Get_iDirection();

	if (direction == -1 && m_bIsIgnoreFlip)
	{
		// 스케일링 제거를 위한 행렬 분해
		XMVECTOR modelScale, modelRotationQuat, modelTranslation;
		XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, modelWorldMatrix);

		// 스케일링이 제거된 모델의 월드 행렬 재구성
		XMMATRIX modelRotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
		XMMATRIX modelTranslationMatrix = XMMatrixTranslationFromVector(modelTranslation);
		modelWorldMatrix = modelRotationMatrix * modelTranslationMatrix;
	}

	// 모델의 월드 위치 가져오기
	XMVECTOR modelPosition = modelWorldMatrix.r[3];

	// 초기 카메라 위치 (로컬)
	XMVECTOR initialCameraPositionLocal = XMLoadFloat3(&currentPoint.position);

	// 초기 카메라 위치 (월드)
	XMVECTOR initialCameraPositionWorld = XMVector3TransformCoord(initialCameraPositionLocal, modelWorldMatrix);

	// 반지름 계산 (카메라와 모델 간 거리)
	m_circleRadius = XMVectorGetX(XMVector3Length(initialCameraPositionWorld - modelPosition));

	// 시간 업데이트
	m_circleElapsedTime += fTimeDelta;

	// 각도 업데이트 (회전 속도에 따라 각도 증가)
	float angleIncrement = m_rotationSpeed * fTimeDelta;
	m_circleAngle += m_bIsClockwise ? angleIncrement : -angleIncrement;

	// 전체 회전 각도 제한 (필요 시)
	// 예를 들어, m_circleAngle이 2π를 초과하거나 -2π 미만일 때 초기화할 수 있습니다.
	// 현재는 계속 누적되므로, 큰 각도에서도 문제가 없도록 합니다.

	// 새로운 카메라 위치 계산 (Y축 기준 회전)
	_float x = m_circleRadius * cosf(m_circleAngle);
	_float z = m_circleRadius * sinf(m_circleAngle);
	// Y축 높이는 초기 카메라의 Y 위치를 유지
	_float y = XMVectorGetY(initialCameraPositionWorld - modelPosition);

	XMVECTOR newCameraPositionOffset = XMVectorSet(x, y, z, 0.0f);
	XMVECTOR newCameraPosition = modelPosition + newCameraPositionOffset;

	// 카메라가 모델을 바라보도록 방향 벡터 계산
	XMVECTOR lookDirection = XMVector3Normalize(modelPosition - newCameraPosition);

	// Up 벡터 설정 (Y축)
	XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Right 벡터 계산
	XMVECTOR rightDirection = XMVector3Normalize(XMVector3Cross(upDirection, lookDirection));

	// Up 벡터 재계산 (정직교화)
	upDirection = XMVector3Cross(lookDirection, rightDirection);

	// 새로운 월드 행렬 구성
	XMMATRIX NewWorldMatrix;
	NewWorldMatrix.r[0] = rightDirection;
	NewWorldMatrix.r[1] = upDirection;
	NewWorldMatrix.r[2] = lookDirection;
	NewWorldMatrix.r[3] = XMVectorSetW(newCameraPosition, 1.0f);

	// Transform 컴포넌트 업데이트
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, NewWorldMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, NewWorldMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, NewWorldMatrix.r[2]);
	_vector position = m_vShakeOffset + NewWorldMatrix.r[3];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, position);

	// 원형 회전이 완료되었는지 체크
	if (m_circleElapsedTime >= m_circleDuration)
	{
		Stop();
	}
}

void CVirtual_Camera::Set_Camera_Position(_float averageX, _float distanceX, _float higherY, _gvector pos1, _gvector pos2)
{

	const float fixedZ = -5.f; //Z값 고정
	const float baseFixedY = 1.5f;  // Y가 0일 때 사용할 고정값

	//플레이어 둘이 thresholdDistance 보다 멀어지면 그제서야 카메라가 멀어지고
	const float thresholdDistance = 2.f;
	//maxDistance까지는 비례하면서 늘어나다 여기서부터는 더 늘어나지 않음
	const float maxDistance = 5.17f;

	//가변 Y 셋팅
	float dynamicY = baseFixedY;

	// higherY가 0보다 큰 경우 가변 오프셋을 추가
	if (higherY > 0.f)
	{
		// higherY 값에 따라 baseFixedY보다 낮은 값을 설정
		const float maxReduction = 1.5f;  // 최대 Y 감소값

		// higherY가 커질수록 dynamicY를 1에서 더 낮게 조정
		dynamicY = baseFixedY - min(maxReduction, higherY * 0.8f);
	}

	// 최종 타겟 위치를 설정
	_float3 targetPosition = _float3(averageX, higherY + dynamicY, fixedZ);

	// DistanceX가 thresholdDistance를 넘을 때 카메라의 Z와 Y를 조절하는 기존 로직 유지
	if (distanceX > thresholdDistance)
	{
		float t = (distanceX - thresholdDistance) / (maxDistance - thresholdDistance);
		t = max(0.f, min(t, 1.f)); // [0, 1] 범위로 클램프

		const float maxYOffset = 0.8f;  // 최대 Y 이동 거리
		const float maxZOffset = 9.f;   // 최대 Z 이동 거리

		_float3 tangent1, tangent2;
		m_pGameInstance->Get_ParallelVectorsInPlane(tangent1, tangent2, m_fFovy);

		// Y와 Z 오프셋을 적용
		targetPosition.y += tangent1.y * maxYOffset * t;
		targetPosition.z += tangent1.z * maxZOffset * t;
	}

	_vector targetVector = XMLoadFloat3(&targetPosition);
	// 위치 설정 (쉐이크 오프셋 포함)
	_vector position = targetVector + m_vShakeOffset;

	// 카메라 위치 설정
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(position, 1.f));
}

void CVirtual_Camera::Set_Camera_Direction(_float averageX, _gvector pos1, _gvector pos2)
{
	// 카메라의 Look 방향을 고정된 값으로 설정 (예: Z축을 향하도록)
	_vector fixedRight = XMVectorSet(1.f, 0.f, 0.0f, 0.f);
	_vector fixedUp = XMVectorSet(0.f, 1.f, 0.0f, 0.f);
	_vector fixedLook = XMVectorSet(0.0f, 0.0, 1.0f, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, fixedRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, fixedUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, fixedLook);
}

void CVirtual_Camera::Print_Flip_Rotation()
{
	// 입력 쿼터니언 값 설정 (데이터 파일에서 가져오거나 하드코딩)
	float x = -0.162778f;
	float y = -0.584241f;
	float z = -0.121022f;
	float w = 0.785824f;

	// 쿼터니언 로드 및 정규화
	XMVECTOR quat = XMVectorSet(x, y, z, w);
	quat = XMQuaternionNormalize(quat);

	// 원본 쿼터니언 출력
	cout << "Original Quaternion: (" << x << ", " << y << ", " << z << ", " << w << ")" << endl;

	// 반사 변환을 적용하는 함수 정의
	auto ReflectQuaternion = [](XMVECTOR q, XMMATRIX reflectionMatrix) {
		// 쿼터니언을 회전 행렬로 변환
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(q);
		// 반사된 회전 행렬 계산: M' = R * M * R
		XMMATRIX reflectedMatrix = reflectionMatrix * rotMatrix * reflectionMatrix;
		// 반사된 회전 행렬을 쿼터니언으로 변환
		XMVECTOR qReflected = XMQuaternionRotationMatrix(reflectedMatrix);
		// 정규화
		qReflected = XMQuaternionNormalize(qReflected);
		return qReflected;
		};

	// 각 축에 대한 반사 행렬 정의
	XMMATRIX reflectX = XMMatrixScaling(-1.0f, 1.0f, 1.0f);
	XMMATRIX reflectY = XMMatrixScaling(1.0f, -1.0f, 1.0f);
	XMMATRIX reflectZ = XMMatrixScaling(1.0f, 1.0f, -1.0f);
	XMMATRIX reflectXY = XMMatrixScaling(-1.0f, -1.0f, 1.0f);
	XMMATRIX reflectXZ = XMMatrixScaling(-1.0f, 1.0f, -1.0f);
	XMMATRIX reflectYZ = XMMatrixScaling(1.0f, -1.0f, -1.0f);
	XMMATRIX reflectXYZ = XMMatrixScaling(-1.0f, -1.0f, -1.0f);

	// 결과를 저장할 변수
	XMFLOAT4 reflectedQuaternion;

	// X축 반전
	XMVECTOR qReflectX = ReflectQuaternion(quat, reflectX);
	XMStoreFloat4(&reflectedQuaternion, qReflectX);
	cout << "Reflection over X-axis: (" << reflectedQuaternion.x << ", " << reflectedQuaternion.y << ", "
		<< reflectedQuaternion.z << ", " << reflectedQuaternion.w << ")" << endl;

	// Y축 반전
	XMVECTOR qReflectY = ReflectQuaternion(quat, reflectY);
	XMStoreFloat4(&reflectedQuaternion, qReflectY);
	cout << "Reflection over Y-axis: (" << reflectedQuaternion.x << ", " << reflectedQuaternion.y << ", "
		<< reflectedQuaternion.z << ", " << reflectedQuaternion.w << ")" << endl;

	// Z축 반전
	XMVECTOR qReflectZ = ReflectQuaternion(quat, reflectZ);
	XMStoreFloat4(&reflectedQuaternion, qReflectZ);
	cout << "Reflection over Z-axis: (" << reflectedQuaternion.x << ", " << reflectedQuaternion.y << ", "
		<< reflectedQuaternion.z << ", " << reflectedQuaternion.w << ")" << endl;

	// XY축 반전
	XMVECTOR qReflectXY = ReflectQuaternion(quat, reflectXY);
	XMStoreFloat4(&reflectedQuaternion, qReflectXY);
	cout << "Reflection over X and Y axes: (" << reflectedQuaternion.x << ", " << reflectedQuaternion.y << ", "
		<< reflectedQuaternion.z << ", " << reflectedQuaternion.w << ")" << endl;

	// XZ축 반전
	XMVECTOR qReflectXZ = ReflectQuaternion(quat, reflectXZ);
	XMStoreFloat4(&reflectedQuaternion, qReflectXZ);
	cout << "Reflection over X and Z axes: (" << reflectedQuaternion.x << ", " << reflectedQuaternion.y << ", "
		<< reflectedQuaternion.z << ", " << reflectedQuaternion.w << ")" << endl;

	// YZ축 반전
	XMVECTOR qReflectYZ = ReflectQuaternion(quat, reflectYZ);
	XMStoreFloat4(&reflectedQuaternion, qReflectYZ);
	cout << "Reflection over Y and Z axes: (" << reflectedQuaternion.x << ", " << reflectedQuaternion.y << ", "
		<< reflectedQuaternion.z << ", " << reflectedQuaternion.w << ")" << endl;

	// XYZ축 반전
	XMVECTOR qReflectXYZ = ReflectQuaternion(quat, reflectXYZ);
	XMStoreFloat4(&reflectedQuaternion, qReflectXYZ);
	cout << "Reflection over X, Y, and Z axes: (" << reflectedQuaternion.x << ", " << reflectedQuaternion.y << ", "
		<< reflectedQuaternion.z << ", " << reflectedQuaternion.w << ")" << endl;
}

void CVirtual_Camera::Set_Player(CGameObject* pPlayer, CGameObject* pEnemy)
{
	_uint uTeam = static_cast<CCharacter*>(pPlayer)->Get_iPlayerTeam();

	if (uTeam == 1)
		m_p1pPlayer = pPlayer;
	else if (uTeam == 2)
		m_p2pPlayer = pPlayer;

	for (auto& iter : m_mapPoints)
	{
		vector<CameraPoint> vecPoints = iter.second;
		for (auto& iter : vecPoints)
		{
			iter.pWorldFloat4x4 = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();
		}
	}

	m_pEnemy = pEnemy;
}

void CVirtual_Camera::Start_Play(_int animationIndex, _bool isImguiPlay, CGameObject* gameObject, _bool ignoreFlip)
{
	if (m_mapPoints[animationIndex].size() == 0)
		return;

	for (auto& iter : m_mapPoints)
		for (auto& iter2 : iter.second)
			iter2.pWorldFloat4x4 = static_cast<CTransform*>(gameObject->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();

	m_bIsIgnoreFlip = ignoreFlip;
	m_AnimationIndex = animationIndex;
	m_iTeam = static_cast<CCharacter*>(gameObject)->Get_iPlayerTeam();
	// Stopped 상태에서 Play를 시작하면 초기화
	if (m_currentPlayMode == Stopped) {
		m_currentPointIndex = 0;
		m_elapsedTime = 0.f;
	}

	m_currentMode = CAMERA_CINEMATIC_MODE;
	m_currentPlayMode = Playing;

	m_bIsImguiPlay = isImguiPlay;
	//플레이를 하면 여기로
	Move_Point(0, m_AnimationIndex);
}

void CVirtual_Camera::Pause()
{
	if (m_currentPlayMode == CAMERA_PLAY_MODE::Playing)
		m_currentPlayMode = CAMERA_PLAY_MODE::Paused;
}

void CVirtual_Camera::Stop()
{
	m_currentMode = m_bIsImguiPlay ? CAMERA_FREE_MODE : CAMERA_NORMAL_MODE;

	m_currentPlayMode = CAMERA_PLAY_MODE::Stopped;
	m_currentPointIndex = 0;
	m_elapsedTime = 0.f;
	m_bIsIgnoreFlip = false;
	m_pEnemy = nullptr;
	m_bIsCirclePlay = false;
	m_circleElapsedTime = 0.0f;
	m_circleAngle = 0.0f;
	m_circleRadius = 0.0f;
}

// 원형 회전 모드 설정 함수
void CVirtual_Camera::SetCirclePlay(_bool isClockwise, _float rotationSpeed)
{
	m_bIsCirclePlay = true;
	m_bIsClockwise = isClockwise;
	m_rotationSpeed = rotationSpeed;
	m_circleElapsedTime = 0.0f;
	m_circleAngle = 0.0f;
}

void CVirtual_Camera::Button_Stop()
{
	m_currentMode = CAMERA_FREE_MODE;
	m_currentPlayMode = CAMERA_PLAY_MODE::Stopped;
	m_currentPointIndex = 0;
	m_elapsedTime = 0.f;
	Move_Point(0, m_AnimationIndex);
}

void CVirtual_Camera::Free_Camera(_float fTimeDelta)
{
	//기본 이동 속도
	_float fMoveSpeed = m_fMoveSpeed;

	// 오른쪽 버튼이 눌렸는지 확인
	if (m_pGameInstance->Mouse_Pressing(1))
	{
		// Shift 키가 눌렸는지 확인하고, 눌렸다면 이동 속도를 증가
		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			fMoveSpeed *= 10.f;
		}

		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta * fMoveSpeed);
		}

		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right(fTimeDelta * fMoveSpeed);
		}

		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * fMoveSpeed);
		}

		if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			m_pTransformCom->Go_Backward(fTimeDelta * fMoveSpeed);
		}

		if (m_pGameInstance->Key_Pressing(DIK_Q))
		{
			m_pTransformCom->Go_Down(fTimeDelta * fMoveSpeed);
		}

		if (m_pGameInstance->Key_Pressing(DIK_E))
		{
			m_pTransformCom->Go_Up(fTimeDelta * fMoveSpeed);
		}

		_long MouseMove = {};

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensor * MouseMove * fTimeDelta);
		}

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * MouseMove * fTimeDelta);
		}
	}

	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	CImgui_Manager::IMGUI_SCREEN tDesc = CImgui_Manager::Get_Instance()->Get_Screen_Desc();

	// 마우스가 ImGui 창 위에 있는지 확인
	_bool isOverShaderImGui = (ptMouse.x >= tDesc.ShaderImGuiPos.x &&
		ptMouse.x <= tDesc.ShaderImGuiPos.x + tDesc.ShaderImGuiSize.x &&
		ptMouse.y >= tDesc.ShaderImGuiPos.y &&
		ptMouse.y <= tDesc.ShaderImGuiPos.y + tDesc.ShaderImGuiSize.y);

	_bool isOverMainImGui = (ptMouse.x >= tDesc.MainImGuiPos.x &&
		ptMouse.x <= tDesc.MainImGuiPos.x + tDesc.MainImGuiSize.x &&
		ptMouse.y >= tDesc.MainImGuiPos.y &&
		ptMouse.y <= tDesc.MainImGuiPos.y + tDesc.MainImGuiSize.y);

	if (isOverShaderImGui == true || isOverMainImGui == true)
	{
		__super::Camera_Update(fTimeDelta);

		return;
	}

	_long MouseMoveX = {};
	_long MouseMoveY = {};

	// 시프트 + 가운데 마우스 버튼을 누른 상태에서 카메라 이동
	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT) && (m_pGameInstance->Get_DIMouseState(DIMK_WHEEL) & 0x80))
	{
		MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMM_X);
		MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMM_Y);

		// 좌우 마우스 움직임에 따라 Go_Left와 Go_Right로 카메라 이동
		if (MouseMoveX < 0)
			m_pTransformCom->Go_Right(-MouseMoveX * m_fMouseSensor * fTimeDelta);
		else if (MouseMoveX > 0)
			m_pTransformCom->Go_Left(MouseMoveX * m_fMouseSensor * fTimeDelta);

		// 상하 마우스 움직임에 따라 Go_Up과 Go_Down로 카메라 이동
		if (MouseMoveY > 0)
			m_pTransformCom->Go_Up(MouseMoveY * m_fMouseSensor * fTimeDelta);
		else if (MouseMoveY < 0)
			m_pTransformCom->Go_Down(-MouseMoveY * m_fMouseSensor * fTimeDelta);
	}
	else if (m_pGameInstance->Get_DIMouseState(DIMK_WHEEL) & 0x80) // 휠만 눌렀을 때 회전
	{
		if ((MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMM_X)))
		{
			// Y축 회전
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensor * MouseMoveX * fTimeDelta);
		}

		if ((MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMM_Y)))
		{
			// 상하 회전 (오른쪽 벡터를 축으로)
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * MouseMoveY * fTimeDelta);
		}
	}

	// 휠 스크롤로 줌인 줌아웃
	_long MouseWheel = m_pGameInstance->Get_DIMouseMove(DIMM_WHEEL);
	if (MouseWheel > 0)
	{
		// 휠을 위로 돌려 줌인
		m_pTransformCom->Go_Straight(m_fMouseSensor * MouseWheel * fTimeDelta);
	}
	else if (MouseWheel < 0)
	{
		// 휠을 아래로 돌려 줌아웃
		m_pTransformCom->Go_Backward(-m_fMouseSensor * MouseWheel * fTimeDelta);
	}
}

void CVirtual_Camera::Default_Camera(_float fTimeDelta)
{
	if (m_p1pPlayer == nullptr || m_p2pPlayer == nullptr)
		return;

	//// 플레이어들의 위치를 가져옵니다.
	_vector pos1 = static_cast<CTransform*>(m_p1pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	_vector pos2 = static_cast<CTransform*>(m_p2pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	//// 두 플레이어 간의 X 거리 계산
	_float distanceX = ComputeDistanceX(pos1, pos2);

	// 평균 X 위치 계산
	_float averageX = (XMVectorGetX(pos1) + XMVectorGetX(pos2)) * 0.5f;

	_float Player1P_Y = XMVectorGetY(pos1);
	_float Player2P_Y = XMVectorGetY(pos2);
	// 두 플레이어의 Y 값 비교 (둘 중 높은 값을 선택)
	_float higherY = max(Player1P_Y, Player2P_Y);

	// 카메라의 위치 설정, 추가한 higherY를 사용
	Set_Camera_Position(averageX, distanceX, higherY, pos1, pos2);

	//// 카메라의 방향 벡터 설정
	Set_Camera_Direction(averageX, pos1, pos2);

}

void CVirtual_Camera::Map_Camera(_float fTimeDelta)
{
	if (m_isEastFinish == true)
	{
		if (m_bIsShaking == true)
		{
			_vector position = m_vBaseCameraPosition + m_vShakeOffset;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, position);
		}
	}
	else if (m_isDestructive == true)
	{
		if (m_isDyingTeam == 0)
			return;

		_vector vPlayerPos = { 0.f,0.f,0.f,1.f };

		switch (m_isDyingTeam)
		{
		case 1:
			vPlayerPos = static_cast<CTransform*>(m_p1pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			break;
		case 2:
			vPlayerPos = static_cast<CTransform*>(m_p2pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			break;
		}

		vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + 1.f);

		m_pTransformCom->LookAt(vPlayerPos);
		if (m_bIsShaking == true)
		{
			_vector position = m_vBaseCameraPosition + m_vShakeOffset;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, position);
		}
	}
}


_float CVirtual_Camera::ComputeDistanceX(_gvector pos1, _gvector pos2)
{
	// 두 플레이어 간의 X축 차이 계산
	_float deltaX = XMVectorGetX(XMVectorSubtract(pos2, pos1));
	return abs(deltaX);
}


void CVirtual_Camera::Add_Point(_float duration, _int type, const _float4x4* pModelFloat4x4, _float damping, _bool hasWorldFloat4x4, _int animationIndex)
{
	CameraPoint cameraPoint{};

	//해당 모델의 월드행렬 저장

	//디폴트 카메라는 모델의 로컬이 없어서 월드행렬을 저장하고
	//나머지 카메라는 모델의 로컬을 변환한 월드행렬 저장해야함
	if (hasWorldFloat4x4 == true)
	{
		_matrix matrix = Float4x4ToMatrix(*pModelFloat4x4);

		// 모델 행렬의 역행렬 계산
		_vector determinant = XMVectorZero();
		_matrix inverseModelMatrix = XMMatrixInverse(&determinant, matrix);

		// 현재 가상 카메라의 월드 위치 가져오기
		_vector worldPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 월드 위치를 모델의 로컬 좌표로 변환
		_vector localPosition = XMVector3TransformCoord(worldPosition, inverseModelMatrix);

		// 변환된 로컬 위치를 CameraPoint에 저장
		XMStoreFloat3(&cameraPoint.position, localPosition);

		// 카메라의 월드 회전 행렬 생성
		_vector right = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		_vector up = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
		_vector look = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_matrix cameraRotationMatrix = XMMatrixIdentity();
		cameraRotationMatrix.r[0] = right;
		cameraRotationMatrix.r[1] = up;
		cameraRotationMatrix.r[2] = look;
		cameraRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 제거

		// 모델 역행렬에서 회전 부분 추출
		_matrix inverseModelRotationMatrix = inverseModelMatrix;
		inverseModelRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 제거

		// 로컬 회전 행렬 계산
		_matrix localRotationMatrix = inverseModelRotationMatrix * cameraRotationMatrix;

		// 로컬 회전 행렬을 쿼터니언으로 변환하여 저장
		_vector localQuaternion = XMQuaternionRotationMatrix(localRotationMatrix);
		XMStoreFloat4(&cameraPoint.rotation, localQuaternion);

		// 추가 정보 저장
		cameraPoint.duration = duration;
		cameraPoint.interpolationType = type;
		cameraPoint.damping = damping;
		cameraPoint.pWorldFloat4x4 = pModelFloat4x4;
		cameraPoint.hasWorldFloat4x4 = hasWorldFloat4x4;

		// 카메라 포인트를 맵에 추가
		m_mapPoints[animationIndex].push_back(cameraPoint);
	}
	else
	{
		//그냥 월드행렬 저장할 것
		_vector worldPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat3(&cameraPoint.position, worldPosition);

		// **카메라의 월드 회전 행렬 정보 생성**
		_vector right = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		_vector up = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
		_vector look = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_matrix cameraRotationMatrix = XMMatrixIdentity();
		cameraRotationMatrix.r[0] = right;
		cameraRotationMatrix.r[1] = up;
		cameraRotationMatrix.r[2] = look;
		cameraRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 정보 제거

		_vector worldQuaternion = XMQuaternionRotationMatrix(cameraRotationMatrix);
		XMStoreFloat4(&cameraPoint.rotation, worldQuaternion);

		cameraPoint.duration = duration;
		cameraPoint.interpolationType = type;
		cameraPoint.damping = damping;
		cameraPoint.pWorldFloat4x4 = nullptr;
		cameraPoint.hasWorldFloat4x4 = hasWorldFloat4x4;
		m_mapPoints[animationIndex].push_back(cameraPoint);
	}
}

void CVirtual_Camera::Add_NormalPoint(_float duration, _int type, const _float4x4* pModelFloat4x4, _float damping, _bool hasWorldFloat4x4, _int animationIndex, CTransform* transform)
{
	CameraPoint cameraPoint{};

	//해당 모델의 월드행렬 저장

	//디폴트 카메라는 모델의 로컬이 없어서 월드행렬을 저장하고
	//나머지 카메라는 모델의 로컬을 변환한 월드행렬 저장해야함
	_matrix matrix = Float4x4ToMatrix(*pModelFloat4x4);

	// 모델의 월드 행렬의 역행렬 계산
	_vector determinant = XMVectorZero();
	_matrix inverseModelMatrix = XMMatrixInverse(&determinant, matrix);

	// 현재 가상카메라의 월드 포지션 가져오기
	_vector worldPosition = transform->Get_State(CTransform::STATE_POSITION);

	// 월드 포지션을 모델의 로컬 좌표로 변환
	_vector localPosition = XMVector3TransformCoord(worldPosition, inverseModelMatrix);

	// 변환된 로컬 포지션을 CameraPoint 구조체에 저장
	XMStoreFloat3(&cameraPoint.position, localPosition);

	// **카메라의 월드 회전 행렬 생성**
	_vector right = XMVector3Normalize(transform->Get_State(CTransform::STATE_RIGHT));
	_vector up = XMVector3Normalize(transform->Get_State(CTransform::STATE_UP));
	_vector look = XMVector3Normalize(transform->Get_State(CTransform::STATE_LOOK));

	_matrix cameraRotationMatrix = XMMatrixIdentity();
	cameraRotationMatrix.r[0] = right;
	cameraRotationMatrix.r[1] = up;
	cameraRotationMatrix.r[2] = look;
	cameraRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 정보 제거

	// **모델 역행렬에서 회전 부분 추출**
	_matrix inverseModelRotationMatrix = inverseModelMatrix;
	inverseModelRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 정보 제거

	// 로컬 회전 행렬 계산 (순서 변경)
	_matrix localRotationMatrix = inverseModelRotationMatrix * cameraRotationMatrix;

	// **로컬 회전 행렬을 쿼터니언으로 변환하여 저장**
	_vector localQuaternion = XMQuaternionRotationMatrix(localRotationMatrix);
	XMStoreFloat4(&cameraPoint.rotation, localQuaternion);

	cameraPoint.duration = duration;
	cameraPoint.interpolationType = type;
	cameraPoint.damping = damping;
	cameraPoint.pWorldFloat4x4 = pModelFloat4x4;
	cameraPoint.hasWorldFloat4x4 = hasWorldFloat4x4;

	m_mapPoints[animationIndex].push_back(cameraPoint);
}


void CVirtual_Camera::Remove_Point(_int currentIndex, _int animationIndex)
{
	if (currentIndex < 0 || currentIndex >= static_cast<int>(m_mapPoints[animationIndex].size())) {
		return; // 유효하지 않은 인덱스일 경우 아무 작업도 하지 않음
	}

	auto it = m_mapPoints[animationIndex].begin();
	advance(it, currentIndex);
	m_mapPoints[animationIndex].erase(it);
}

//list의 index 순으로 찾아가서 나온 Position과 Rotation으로 카메라의 트랜스폼을 셋팅하면 되겠다
void CVirtual_Camera::Move_Point(_int index, _int animationIndex)
{
	if (index < 0 || index >= m_mapPoints[animationIndex].size()) {
		return;
	}

	const CameraPoint& targetPoint = m_mapPoints[animationIndex][index];  // 인덱스 접근

	if (targetPoint.hasWorldFloat4x4 == true)
	{
		// 1. 로컬 포지션
		_float3 localPosition = targetPoint.position;

		// 2. 로컬 회전
		_float4 localQuaternion = targetPoint.rotation;


		_matrix modelWorldMatrix = {};

		modelWorldMatrix =
			targetPoint.pWorldFloat4x4 == nullptr
			? Float4x4ToMatrix(*static_cast<CTransform*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"))->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr())
			: Float4x4ToMatrix(*targetPoint.pWorldFloat4x4);

		// **스케일링 제거를 위한 행렬 분해**
		_vector modelScale;
		_vector modelRotationQuat;
		_vector modelTranslation;
		XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, modelWorldMatrix);

		// **스케일링이 제거된 모델의 월드 행렬 재구성**
		_matrix modelRotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
		_matrix modelTranslationMatrix = XMMatrixTranslationFromVector(modelTranslation);
		_matrix modelWorldMatrixNoScale = modelRotationMatrix * modelTranslationMatrix;

		// **4. 로컬 포지션을 월드 포지션으로 변환**
		_vector interpolatedPositionWorld = XMVector3TransformCoord(XMLoadFloat3(&localPosition), modelWorldMatrixNoScale);

		// **5. 로컬 회전을 월드 회전으로 변환**
		// 로컬 회전 행렬 생성
		_matrix interpolatedRotationMatrixLocal = XMMatrixRotationQuaternion(XMLoadFloat4(&localQuaternion));

		// 월드 회전 행렬 계산
		_matrix interpolatedRotationMatrixWorld = interpolatedRotationMatrixLocal * modelRotationMatrix;

		// **6. 카메라의 월드 행렬 생성**
		_matrix NewWorldMatrix = interpolatedRotationMatrixWorld;
		NewWorldMatrix.r[3] = XMVectorSetW(interpolatedPositionWorld, 1.0f); // 위치 설정

		// 월드 매트릭스에서 Right, Up, Look 벡터 추출
		_vector right = NewWorldMatrix.r[0];
		_vector up = NewWorldMatrix.r[1];
		_vector look = NewWorldMatrix.r[2];
		_vector position = NewWorldMatrix.r[3];

		// CTransform 컴포넌트에 설정
		CTransform* cameraTransform = static_cast<CTransform*>(Get_Component(TEXT("Com_Transform")));

		// 방향 벡터 설정
		cameraTransform->Set_State(CTransform::STATE_RIGHT, right);
		cameraTransform->Set_State(CTransform::STATE_UP, up);
		cameraTransform->Set_State(CTransform::STATE_LOOK, look);
		// 위치 설정
		cameraTransform->Set_State(CTransform::STATE_POSITION, position);
	}
	else
	{
		// CTransform 컴포넌트에 설정
		CTransform* cameraTransform = static_cast<CTransform*>(Get_Component(TEXT("Com_Transform")));

		// 월드 포지션 로드
		_float3 worldPosition = targetPoint.position;

		// 월드 회전 로드
		_float4 worldQuaternion = targetPoint.rotation;

		// 월드 회전 행렬 생성
		_matrix interpolatedRotationMatrixWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&worldQuaternion));

		// 카메라의 월드 행렬 생성
		_matrix NewWorldMatrix = interpolatedRotationMatrixWorld;
		NewWorldMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&worldPosition), 1.f); // 위치 설정

		// 월드 매트릭스에서 Right, Up, Look 벡터 추출
		_vector right = NewWorldMatrix.r[0];
		_vector up = NewWorldMatrix.r[1];
		_vector look = NewWorldMatrix.r[2];
		_vector position = NewWorldMatrix.r[3];

		// 방향 벡터 설정
		cameraTransform->Set_State(CTransform::STATE_RIGHT, right);
		cameraTransform->Set_State(CTransform::STATE_UP, up);
		cameraTransform->Set_State(CTransform::STATE_LOOK, look);
		// 위치 설정
		cameraTransform->Set_State(CTransform::STATE_POSITION, position);
	}
}

void CVirtual_Camera::Modify_Transform(_int index, _int animationIndex)
{
	CameraPoint& targetPoint = m_mapPoints[animationIndex][index];  // 인덱스 접근

	//현재 수정하려고 하는 인덱스의 Point에 접근해서
	//현재 가상카메라의 포지션과 Rotation을 로컬로 내려서 저장해야함
	if (targetPoint.pWorldFloat4x4 != nullptr)
	{
		_matrix matrix = Float4x4ToMatrix(*targetPoint.pWorldFloat4x4);

		// 모델의 월드 행렬의 역행렬 계산
		_vector determinant = XMVectorZero();
		_matrix inverseModelMatrix = XMMatrixInverse(&determinant, matrix);

		// 현재 가상카메라의 월드 포지션 가져오기
		_vector worldPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 월드 포지션을 모델의 로컬 좌표로 변환
		_vector localPosition = XMVector3TransformCoord(worldPosition, inverseModelMatrix);

		// 변환된 로컬 포지션을 CameraPoint 구조체에 저장
		XMStoreFloat3(&targetPoint.position, localPosition);

		// **카메라의 월드 회전 행렬 생성**
		_vector right = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		_vector up = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
		_vector look = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_matrix cameraRotationMatrix = XMMatrixIdentity();
		cameraRotationMatrix.r[0] = right;
		cameraRotationMatrix.r[1] = up;
		cameraRotationMatrix.r[2] = look;
		cameraRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 정보 제거

		// **모델 역행렬에서 회전 부분 추출**
		_matrix inverseModelRotationMatrix = inverseModelMatrix;
		inverseModelRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 정보 제거

		// **카메라의 로컬 회전 행렬 계산**
		_matrix localRotationMatrix = cameraRotationMatrix * inverseModelRotationMatrix;

		// **로컬 회전 행렬을 쿼터니언으로 변환하여 저장**
		_vector localQuaternion = XMQuaternionRotationMatrix(localRotationMatrix);
		_float4 rotation = {};
		XMStoreFloat4(&rotation, localQuaternion);
		targetPoint.rotation = rotation;
	}
	//디폴트는 월드 포지션과 로테이션만
	else
	{
		// 현재 가상카메라의 월드 포지션 가져와 서 저장
		_vector worldPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat3(&targetPoint.position, worldPosition);

		// **카메라의 월드 회전 행렬 생성**
		_vector right = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		_vector up = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
		_vector look = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_matrix cameraRotationMatrix = XMMatrixIdentity();
		cameraRotationMatrix.r[0] = right;
		cameraRotationMatrix.r[1] = up;
		cameraRotationMatrix.r[2] = look;
		cameraRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 위치 정보 제거

		_vector worldQuaternion = XMQuaternionRotationMatrix(cameraRotationMatrix);
		XMStoreFloat4(&targetPoint.rotation, worldQuaternion);
	}
}

_float CVirtual_Camera::AdjustT_Damping(_float t, _float damping)
{
	// Damping 계수에 따라 t 값을 조절
	 // damping > 1.0f: 더 느리게 시작하고 빠르게 끝남 (ease-in)
	 // damping < 1.0f: 더 빠르게 시작하고 느리게 끝남 (ease-out)
	 // damping = 1.0f: 기본 Smoothstep 함수와 동일
	if (damping == 1.0f)
	{
		return t * t * (3.0f - 2.0f * t); // 기본 Smoothstep
	}
	else if (damping > 1.0f)
	{
		// Smoothstep과 pow(t, damping)의 혼합
		float smooth = t * t * (3.0f - 2.0f * t); // Smoothstep
		float easeIn = pow(t, damping); // Ease-In
		float weight = 0.1f; // Smoothstep의 가중치 조절 (0.0f ~ 1.0f)
		return weight * smooth + (1.0f - weight) * easeIn;
	}
	else // damping < 1.0f
	{
		// Ease-out 효과 강화
		return t * t * (3.0f - 2.0f * t) + (1.0f - t) * t * (1.0f - damping);
	}
}

void CVirtual_Camera::Delete_Points(_int animationIndex)
{
	m_mapPoints[animationIndex].clear();
}

void CVirtual_Camera::ApplyCameraShake(_float fTimeDelta)
{
	m_fElapsedShakeTime += fTimeDelta;

	if (m_fElapsedShakeTime >= m_fShakeDuration)
	{
		// 흔들림 종료
		StopCameraShake();
		return;
	}

	// 흔들림 오프셋 계산
	_float progress = m_fElapsedShakeTime / m_fShakeDuration;
	_float damper = 1.0f - progress; // 흔들림 감쇠 비율

	// 랜덤 오프셋 계산
	_float offsetX = ((rand() % 1000) / 500.0f - 1.0f) * m_fShakeMagnitude * damper;
	_float offsetY = ((rand() % 1000) / 500.0f - 1.0f) * m_fShakeMagnitude * damper;
	_float offsetZ = ((rand() % 1000) / 500.0f - 1.0f) * m_fShakeMagnitude * damper;

	// 흔들림 오프셋 벡터 생성
	m_vShakeOffset = XMVectorSet(offsetX, offsetY, offsetZ, 0.0f);

	// **저장된 기준 위치에 흔들림 오프셋 적용**
	_vector shakenPosition = m_vBaseCameraPosition + m_vShakeOffset;
}

void CVirtual_Camera::StartCameraShake(_float fDuration, _float fMagnitude)
{
	m_vBaseCameraPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_bIsShaking = true;
	m_fShakeDuration = fDuration;
	m_fShakeMagnitude = fMagnitude;
	m_fElapsedShakeTime = 0.0f;
}

void CVirtual_Camera::StopCameraShake()
{
	m_bIsShaking = false;
	m_fShakeDuration = 0.0f;
	m_fElapsedShakeTime = 0.0f;
	m_vShakeOffset = XMVectorZero();
}

void CVirtual_Camera::Set_CameraMode(CMain_Camera::VIRTUAL_CAMERA cameraMode)
{
	if (cameraMode == CMain_Camera::VIRTUAL_CAMERA_NORMAL)
		m_currentMode = CAMERA_NORMAL_MODE;
	else if (cameraMode == CMain_Camera::VIRTUAL_CAMERA_FREE)
		m_currentMode = CAMERA_FREE_MODE;
	else if (cameraMode == CMain_Camera::VIRTUAL_CAMERA_MAP)
		m_currentMode = CAMERA_MAP_MODE;
	else
		m_currentMode = CAMERA_FREE_MODE;
}


void CVirtual_Camera::Set_DyingTeam(_uint iTeamIndex, _matrix CamWorldMatrix)
{
	m_isDyingTeam = iTeamIndex;
	_float4x4 ResultMatrix;
	XMStoreFloat4x4(&ResultMatrix, CamWorldMatrix);
	m_pTransformCom->Set_WorldMatrix(ResultMatrix);
	m_isDestructive = true;
	m_isEastFinish = false;
}

void CVirtual_Camera::Set_EastFinish()
{
	m_isEastFinish = true;
	m_isDestructive = false;
}

CVirtual_Camera* CVirtual_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVirtual_Camera* pInstance = new CVirtual_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVirtual_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVirtual_Camera::Clone(void* pArg)
{
	CVirtual_Camera* pInstance = new CVirtual_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVirtual_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVirtual_Camera::Free()
{
	__super::Free();

}

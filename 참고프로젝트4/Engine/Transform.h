#pragma once

#include "Component.h"

/* 객체들의 월드 상태를 표현하기위한 데이터를 가진다. (월드행렬) */
/* 해당 월드 상에서의 변환을 위한 기능을 가진다. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	/* 행렬의 행의 정보를 교체한다. */
	void Set_State(STATE eState, const _float4 & vState) {
		memcpy(&m_WorldMatrix.m[eState], &vState, sizeof(_float3));
	}
	void Set_State(STATE eState, _fvector vState) {
		m_WorldMatrix.m[eState][0] = XMVectorGetX(vState);
		m_WorldMatrix.m[eState][1] = XMVectorGetY(vState);
		m_WorldMatrix.m[eState][2] = XMVectorGetZ(vState);
		m_WorldMatrix.m[eState][3] = XMVectorGetW(vState);
	}

	void Set_State(STATE eState, _float3 vState) {
		m_WorldMatrix.m[eState][0] = vState.x;
		m_WorldMatrix.m[eState][1] = vState.y;
		m_WorldMatrix.m[eState][2] = vState.z;
	}

	_vector Get_State(STATE eState) {
 		return XMVectorSet(m_WorldMatrix.m[eState][0],
			m_WorldMatrix.m[eState][1],
			m_WorldMatrix.m[eState][2],
			m_WorldMatrix.m[eState][3]);
	}

	_float3 Get_Scaled() {
		return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_RIGHT])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_UP])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_LOOK])));
	}

	_float3 Get_Rotated() { 

		_matrix rotationMatrix = Get_WorldMatrix();

		XMVECTOR rotationQuat = XMQuaternionRotationMatrix(rotationMatrix);
		XMMATRIX rotationMatrixFromQuat = XMMatrixRotationQuaternion(rotationQuat);

		_float4x4 rotationMatrixFromQuatValues;
		XMStoreFloat4x4(&rotationMatrixFromQuatValues, rotationMatrixFromQuat);

		_float3 rotation;
		rotation.x = atan2(rotationMatrixFromQuatValues._32, rotationMatrixFromQuatValues._33);
		rotation.y = asin(-rotationMatrixFromQuatValues._31);
		rotation.z = atan2(rotationMatrixFromQuatValues._21, rotationMatrixFromQuatValues._11);

		//rotation.x = XMConvertToDegrees(rotation.x);
		//rotation.y = XMConvertToDegrees(rotation.y);
		//rotation.z = XMConvertToDegrees(rotation.z);

		return rotation;
	}

	_float3 Get_Rotated_Degree() {
		_float3 Rotation = Get_Rotated();

		Rotation.x = XMConvertToDegrees(Rotation.x);
		Rotation.y = XMConvertToDegrees(Rotation.y);
		Rotation.z = XMConvertToDegrees(Rotation.z);

		return Rotation;
	}


	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_float4x4 Get_WorldFloat4x4() {
		return m_WorldMatrix;
	}
	_matrix Get_WorldMatrixInverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	_float4x4 Get_WorldFloat4x4Inverse() {
		_float4x4	InverseMatrix;
		XMStoreFloat4x4(&InverseMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
		return InverseMatrix;
	}

	void	Set_WorldFloat4x4(_float4x4 mat) { m_WorldMatrix = mat; }

	void Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);



public:
	virtual HRESULT Initialize_Prototype();

public:
	void Move_To_AnimationPos(weak_ptr<class CCommonModelComp> pModel, _float fTimeDelta);

	void Add_Position(_float fTimeDelta, _fvector vAddPosition);
	void Add_Position(_fvector vAddPosition);
	_bool Set_Position(_float fTimeDelta, _fvector vNewPosition);
	void Slide(_float fTimeDelta, _vector vPosition, _vector vLineDir, _vector vMoveDir);
	_bool Go_Straight(_float fTimeDelta,_Out_ _float3* vAddPos = nullptr);
	_bool Go_Left(_float fTimeDelta, _Out_ _float3* vAddPos = nullptr);
	_bool Go_Right(_float fTimeDelta, _Out_ _float3* vAddPos = nullptr);
	_bool Go_Backward(_float fTimeDelta, _Out_ _float3* vAddPos = nullptr);
	void  Go_Up(_float fTimeDelta, _Out_ _float3* vAddPos = nullptr);
	void  Go_Down(_float fTimeDelta, _Out_ _float3* vAddPos = nullptr);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation_Reset(_fvector vAxis, _float fRadian);
	void Rotation(_float fX, _float fY, _float fZ);
	void Rotation_Quaternion(_fvector vQuaternion);
	
	// 해당방향으로 서서히 회전시키는 함수
	_bool Rotate_On_BaseDir(_float fTimeDelta, _float fRadian);
	_bool Rotate_On_BaseDir(_float fTimeDelta, _float3 vDestDir);
	// 세팅된 BaseDir로 그 방향으로 향하게 하는 함수
	void Rotate_On_BaseDir(_float fRadian);
	
	// Look이 BaseDir을 향하는지 체크하는 함수
	_float Check_Look_On_BaseDir(_float fRadian);

	_bool Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare = 0.1f);
	// 해당 위치로 바라보도록 하는 함수
	void Look_At(_fvector vTargetPos);
	void Look_At(_fvector vTargetPos, _float fRadianSpeed);
	// Z축 회전 없이 바라보기
	void Look_At_OnLand(_fvector vTargetPos);

	// 해당 방향으로 바라보며 축을 수직으로 만드는 함수
	void Set_Look(_fvector vLookDir, _bool bUseCamera = false);
	// 해당 방향으로 바라보며 축을 수직으로 만드는 함수 [미완성]
	void Set_Look(_fvector vLookDir, _fvector vUpDir);
	void Set_Look_Manual(_fvector vLookDir);
	// 업벡터를 설정하고 축을 수직으로 만드는 함수
	void Set_Up(_fvector vDir); 

public:
	HRESULT  Bind_ShaderResource(class CShader* pShader, const _char * pConstantName);


private:
	_float3				m_vAnimationPosition = {};
	_float3				m_vBaseDir = {0.f,0.f,1.f};
	_float				m_fSpeedPerSec = { 5.0f };
	_float				m_fAdjustSpeed = { 1.0f };
	_float				m_fRotationPerSec = { 5.0f };

	_bool				m_bIsOnGround = { true };
	_bool				m_bMove = { true };
	_bool				m_bReset_AnimationPosition = { true };
	
	_bool				m_bMove_AnimationPosition= { true };

	_float4x4			m_WorldMatrix = {};

	weak_ptr<class CPhysX_Controller> m_pPhysX_ControllerCom;

public:
	GETSET_EX2(_float, m_fSpeedPerSec, Speed, GET, SET)
	GETSET_EX1(_float3, m_vBaseDir,BaseDir, GET)
	GETSET_EX2(_bool, m_bMove, Move, GET, SET)
	GETSET_EX2(_bool, m_bIsOnGround, IsOnGround, GET, SET)
	GETSET_EX2(_bool, m_bReset_AnimationPosition, Reset_AnimationPosition, GET, SET)
	GETSET_EX2(_bool, m_bMove_AnimationPosition, Move_AnimationPosition, GET, SET)

	void Set_BaseDir(_float3 vDir) { m_vBaseDir = XMVector3Normalize(float3(vDir.x,0.f, vDir.z)); }
	void Set_PhysXControllerCom(weak_ptr<class CPhysX_Controller> pPhysX_ControllerCom) { m_pPhysX_ControllerCom = pPhysX_ControllerCom; }

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public:
	static shared_ptr<CTransform> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END
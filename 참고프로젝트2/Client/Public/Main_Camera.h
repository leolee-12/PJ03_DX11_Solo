#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Character.h"

BEGIN(Client)

class CMain_Camera final : public CCamera
{
public:
	enum VIRTUAL_CAMERA
	{
		VIRTUAL_CAMERA_NORMAL,
		VIRTUAL_CAMERA_FREE,
		VIRTUAL_CAMERA_SON_HEAVY,
		VIRTUAL_CAMERA_SON_KNOCK_AWAY_UP,
		VIRTUAL_CAMERA_SON_AIR_SMASH,
		VIRTUAL_CAMERA_SON_GRAB,
		VIRTUAL_CAMERA_SON_SAME_GRAB,
		VIRTUAL_CAMERA_SON_ENERGY,
		VIRTUAL_CAMERA_SON_ULTIMATE,
		VIRTUAL_CAMERA_21_HEAVY,
		VIRTUAL_CAMERA_21_KNOCK_AWAY_UP,
		VIRTUAL_CAMERA_21_AIR_SMASH,
		VIRTUAL_CAMERA_21_GRAB,
		VIRTUAL_CAMERA_21_SAME_GRAB,
		VIRTUAL_CAMERA_21_GRAB_SPECIAL,
		VIRTUAL_CAMERA_21_ENERGY,
		VIRTUAL_CAMERA_21_ULTIMATE,
		VIRTUAL_CAMERA_HIT_HEAVY,
		VIRTUAL_CAMERA_HIT_KNOCK_AWAY_UP,
		VIRTUAL_CAMERA_HIT_GRAB,
		VIRTUAL_CAMERA_HIT_214_MIDDLE,
		VIRTUAL_CAMERA_HIT_236_MIDDLE,
		VIRTUAL_CAMERA_HIT_236_HEAVY,
		VIRTUAL_CAMERA_HIT_1_ULTIMATE,
		VIRTUAL_CAMERA_HIT_3_ULTIMATE,
		VIRTUAL_CAMERA_FRIEZA_LIGHT_FINAL,
		VIRTUAL_CAMERA_FRIEZA_HEAVY,
		VIRTUAL_CAMERA_FRIEZA_KNOCK_AWAY_UP,
		VIRTUAL_CAMERA_FRIEZA_GRAB,
		VIRTUAL_CAMERA_FRIEZA_ULTIMATE_1,
		VIRTUAL_CAMERA_FRIEZA_ULTIMATE_3,
		VIRTUAL_CAMERA_FRIEZA_GOLDEN,
		VIRTUAL_CAMERA_FRIEZA_DOWN_FORWARD_J,
		VIRTUAL_CAMERA_FRIEZA_FALL_REFLECT,
		VIRTUAL_CAMERA_COMMON_APPEAR,
		VIRTUAL_CAMERA_GOKU_ENTRY,
		VIRTUAL_CAMERA_21_ENTRY,
		VIRTUAL_CAMERA_HIT_ENTRY,
		VIRTUAL_CAMERA_FRIEZA_ENTRY,
		VIRTUAL_CAMERA_GOKU_VS_FRIEZA_ENTRY,
		VIRTUAL_CAMERA_HIT_WIN,
		VIRTUAL_CAMERA_MAP,
		VIRTUAL_CAMERA_END
	};

private:
	CMain_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_Camera(const CMain_Camera& Prototype);
	virtual ~CMain_Camera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	//버츄얼 카메라 셋팅
	void Set_Virtual_Camera(VIRTUAL_CAMERA mode);
	VIRTUAL_CAMERA Get_Virtual_Camera() { return m_currentVirtualMode; };
	void Add_Point(_float duration, _int type, const _float4x4* worldMatrixPtr, _float damping, _bool hasWorldFloat4x4, _int animationIndex);
	void Add_NormalPoint(_float duration, _int type, const _float4x4* worldMatrixPtr, _float damping, _bool hasWorldFloat4x4, _int animationIndex);
	void Remove_Point(_int currentIndex, _int animationIndex);
	vector<CameraPoint>& Get_VectorPoint(_int index);
	void IMGUI_Play(_int animationIndex, CGameObject* gameObject = nullptr);
	void Play(VIRTUAL_CAMERA cameraID, _int animationIndex, CGameObject* gameObject = nullptr, CGameObject* EnemyObject = nullptr, _bool ignoreFlip = false);
	void Stop();
	void Pause();
	void Button_Stop();
	void StartCameraShake(_float fDuration, _float fMagnitude);

	void Move_Point(_int index, _int animationIndex);
	void Modify_Transform(_int index, _int animationIndex);
	void ApplyCameraData(CameraSaveData& cameraData);
	_int Get_CameraIndex(_int modelID, _int skillID);
	void Delete_Points(_int animationIndex);
	void SetPosition(_fvector position);

	void Set_Player(CGameObject* pPlayer, CGameObject* pEnemy = nullptr);
	const char* Get_Current_CameraName();
	_bool Get_IsPlay();
	void Set_DyingTeam(_uint iDyingTeam);
	void Set_EastFinish(_float4 InitPos);
	void Set_CirclePlay(_bool isClockwise = true, _float rotationSpeed = XM_2PI / 10.0f);

public:
	vector<class CVirtual_Camera*> m_vecVirtualCamera;

	class CGameObject* m_pCutsceneCharacter = { nullptr };

private:
	VIRTUAL_CAMERA m_currentVirtualMode = { VIRTUAL_CAMERA_NORMAL };

	unordered_map<std::string, int> stringToSkillID;
	unordered_map<std::string, int> stringToAnimID;

public:
	static CMain_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
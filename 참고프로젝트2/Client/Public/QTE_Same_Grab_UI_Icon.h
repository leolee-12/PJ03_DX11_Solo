#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CQTE_Same_Grab_UI_Icon final : public CGameObject
{
public:
	struct QTE_Same_Grab_UI_ICON_DESC
	{
		_float	fSizeX{}, fSizeY{}, fX{}, fY{}, fAlpha{}, iTextureNumber{}, fFallDelay{};
		_bool isFirst = false;
		_bool isLast = false;
		CGameObject* SameGrab = { nullptr };
	};

	enum IconState
	{
		NOT_SELECTED,        // 기본 상태
		FALLING,             // 떨어지는 중인 상태 (새로 추가)
		SELECTED,            // 현재 선택된 상태
		ALREADY_PRESSED,     // 올바르게 눌린 상태
		WRONG_PRESSED,        // 잘못 눌린 상태
		ASCEND,				 // 올라가기
	};

private:
	CQTE_Same_Grab_UI_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Same_Grab_UI_Icon(const CQTE_Same_Grab_UI_Icon& Prototype);
	virtual ~CQTE_Same_Grab_UI_Icon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Set_State(IconState state);
	void Set_AscendDelay(_float delay) { m_fAscendDelay = delay; };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Update_Falling(_float fTimeDelta);
	void Update_Selected(_float fTimeDelta);
	void Update_AlreadyPressed(_float fTimeDelta);
	void Update_WrongPressed(_float fTimeDelta);
	void Update_Ascend(_float fTimeDelta);
	void Update_NotSelected(_float fTimeDelta);

public:
	_float m_fX{}, m_fY{};
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float					m_fSizeX{}, m_fSizeY{},  m_fAlpha{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};
	_int m_iTextureNumber = {};

	_bool m_bIsFirst = { false };
	_bool m_bIsLast = { false };
	IconState m_State = NOT_SELECTED; // 아이콘 상태

	// 애니메이션 관련 변수
	_float m_fTargetY = {};          // 목표 Y 위치
	_float m_fCurrentY = {};         // 현재 Y 위치
	_float m_fVelocityY = {};        // Y 속도 (부드러운 이동을 위한 변수)
	const _float m_fDefault_Y = 300.f;   // 기본 Y 위치
	const _float m_fSelected_Y = 250.f;  // 선택된 상태의 Y 위치
	const _float m_fSmoothing_Time = 0.05f; // 애니메이션 속도 조절 (필요에 따라 조정)

	// 흔들림 애니메이션 변수
	_float m_fShakeDuration = { 0.5f };    // 흔들림 지속 시간
	_float m_fShakeTime = {};        // 현재 흔들림 시간
	_float m_fShakeAmplitude = { 20.f };   // 흔들림 진폭
	_float m_fShakeFrequency = { 25.f };   // 흔들림 주파수
	_bool m_bIsShaking = { false };         // 흔들림 상태

	// 새로운 애니메이션 변수
	_float m_fStartY = -100.f;          // 초기 Y 위치
	_float m_fAnimationDuration = 0.7f; // 애니메이션 지속 시간
	_float m_fElapsedTime = 0.f;        // 경과 시간
	_bool m_bIsFalling = false;         // 떨어지는 중인지 여부
	_float m_fFallDelay = 0.f;          // 떨어지기 시작하는 지연 시간

	// ASCEND 상태 관련 변수
	_bool m_bIsAscending;       // 올라가는 중인지 여부
	_float m_fAscendDelay;      // 올라가기 시작하는 지연 시간

	class CGameObject* m_pSameGrab = { nullptr };

public:
	static CQTE_Same_Grab_UI_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
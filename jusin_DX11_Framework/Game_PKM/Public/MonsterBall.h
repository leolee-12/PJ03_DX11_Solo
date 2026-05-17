#pragma once
#include "GameObject.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

/* -------------------------------------------------- */
// CMonsterBall : Capture 레벨에서 던지는 몬스터볼
//  - READY → FLYING → IMPACT → DONE 상태 머신
//  - FLYING 시 포물선 운동 (XZ 선형, Y 선형 + arc)
//  - 충돌체 및 명중 판정은 별도 단위 (현재 IMPACT 는 시간 종료로 전이)
/* -------------------------------------------------- */

class CMonsterBall final : public CGameObject
{
public:
	enum class BALL_STATE : _ubyte { READY, FLYING, IMPACT, DONE, END };

	struct MONSTER_BALL_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vTargetPos = { 0.f, 0.f, 0.f };       // 카메라 vAt = CaptureTarget 위치
		_float  fFlightDuration = 1.0f;                    // 비행 시간(초)
		_float  fArcHeight = 2.0f;                    // 포물선 최고점 추가 높이
		_float  fImpactDuration = 0.5f;                    // IMPACT 정지 시간(초)
	};

private:
	CMonsterBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBall(const CMonsterBall& Prototype);
	virtual ~CMonsterBall() = default;

public:
	virtual _string Get_TypeName() const override { return "MonsterBall"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	BALL_STATE Get_State() const { return m_eState; }
	_bool      Is_Done()   const { return BALL_STATE::DONE == m_eState; }

	void       Launch();   // READY 상태에서만 동작 → FLYING 전이 + 자동 Show
	void       Reset();    // 강제로 READY 로 복귀 — 재던지기 진입 시 호출 (가시성 건드리지 않음)
	void       Hide();     // 외부 가시성 false — Late_Update 의 RenderGroup 등록 차단
	void       Show();     // 외부 가시성 true

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void    Update_Position();    // 현재 t 기반 포물선 좌표 → Transform 반영

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	BALL_STATE m_eState = { BALL_STATE::READY };
	_float     m_fElapsed = { 0.f };

	_float3 m_vStartPos = { -1.3f, 1.5f, -7.3f };
	_float3 m_vTargetPos = { 0.f, 0.f, 0.f };
	_float  m_fFlightDuration = 1.0f;
	_float  m_fArcHeight = 2.0f;
	_float  m_fImpactDuration = 0.5f;
	_bool   m_bVisible = { true };    // 디폴트 보임. Late_Update 에서 false 시 RenderGroup 등록 생략.

public:
	static CMonsterBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END
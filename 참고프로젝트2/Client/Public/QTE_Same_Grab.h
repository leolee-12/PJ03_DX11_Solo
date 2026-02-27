#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CQTE_Same_Grab final : public CGameObject
{
	enum UI_COMMAND
	{
		UI_COMMAND_LIGHT, //약공
		UI_COMMAND_MIDDLE, //중공
		UI_COMMAND_HEAVY, //강공
		UI_COMMAND_ULTIMATE, //특수공격
		UI_COMMAND_END
	};

private:
	CQTE_Same_Grab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Same_Grab(const CQTE_Same_Grab& Prototype);
	virtual ~CQTE_Same_Grab() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Notify_Last_UI_Final_Complete() { m_bUI_Final_Complate = true; };
	void Start(class CGameObject* callObject, _float _lifeTime);
private:
	void Start_QTE(class CGameObject* callObject, _float _lifeTime);
	void End_QTE();
	void Handle_QTEInput();
	void Process_Command(UI_COMMAND input, _int playerID);
	void Create_UIIcons(_int playerID, const vector<UI_COMMAND>& sequence);
	void Clear_UIIcons();
	void Handle_WrongInput(_int playerID); // 오답 처리 함수 선언
	void Ascend_UIIcons(vector<class CQTE_Same_Grab_UI_Icon*>& icons);

	void Final_End_QTE();
	_bool Check_GameStart(class CGameObject* callObject);
	_int Determine_QTE_Result() const;
private:
	_bool m_bIsQTEActive = { false }; // QTE 활성화 여부
	_float m_fTimer = { 0.f }; // 타이머
	_float m_iTotalTime = { 6.f }; // 총 시간 (예: 10초)
	_int m_iSequenceLength = { 10 }; // 시퀀스 길이 (N)

	// 1P 관련
	queue<UI_COMMAND> m_CommandQueue_P1;
	vector<UI_COMMAND> m_CurrentSequence_P1;
	_int m_iCorrectInputs_P1 = {};
	_int m_CurrentIndex_P1 = { 0 }; // 현재 선택된 아이콘 인덱스

	// 2P 관련
	queue<UI_COMMAND> m_CommandQueue_P2;
	vector<UI_COMMAND> m_CurrentSequence_P2;
	_int m_iCorrectInputs_P2 = {};
	_int m_CurrentIndex_P2 = { 0 }; // 현재 선택된 아이콘 인덱스

	// UI 아이콘 객체 저장
	vector<class CQTE_Same_Grab_UI_Icon*> m_UIIcons_P1;
	vector<class CQTE_Same_Grab_UI_Icon*> m_UIIcons_P2;
	vector<class CQTE_Same_Grab_UI_Particle*> m_UIParticles;
	class CQTE_Same_Grab_UI_Gauge* m_UIGauge = { nullptr };

	// 각 플레이어의 쿨다운 타이머
	_float m_fCooldown_P1 = {};
	_float m_fCooldown_P2 = {};

	// 쿨다운 지속 시간 (초)
	const _float COOLDOWN_DURATION = 1.0f; // 필요에 따라 조절 가능

	// QTE 종료 후 초기화 관리
	_bool m_bIsEndQTE = { false };           // QTE 종료 프로세스 진행 중인지 여부
	_float m_fEndQTE_Timer = { 0.f };      // QTE 종료 후 초기화까지의 경과 시간
	const _float m_fEndQTE_Delay = { 1.5f }; // 초기화까지 기다릴 시간 (초)


	//마지막 UI_Final
	_bool m_bUI_Final_Complate = { false };
	//카메라
	class CMain_Camera* m_pMain_Camera = { nullptr };
	//두명 모두 호출해야함
	list<class CGameObject*> m_pListGameObject;
	class CCharacter* m_p1PCharacter = { nullptr };
	class CCharacter* m_p2PCharacter = { nullptr };

public:
	static CQTE_Same_Grab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
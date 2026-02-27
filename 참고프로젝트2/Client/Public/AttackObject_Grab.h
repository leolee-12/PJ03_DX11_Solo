#pragma once

//#include "GameObject.h"
//#include "Client_Defines.h"
//#include "AnimationEvent_Defines.h"
#include "AttackObject.h"



BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)



class CAttackObject_Grab final : public CAttackObject
{
public:
	//enum HitMotion { HIT_LIGHT, HIT_MEDIUM, HIT_HEAVY, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP };

	typedef struct ATTACK_Grab_DESC : public  CAttackObject::ATTACK_DESC
	{
		//_uint m_iOnwerNextAnimationIndex   는 이미 있음  m_bOwnerNextAnimation 도 있음

		//피격자의 애니메이션? 
	
		
		//공격자 와의 거리 조절
		_float2 fDistance = {};

		//잡기 애니메이션 index, position
		//_ushort iGrabAnimationIndex = {};    == //m_iOnwerNextAnimationIndex

		_float fGrabAnimationPosition={};


		//Set_Animation인지 Set_NextAnimation인지 bool값?

	};
private:
	CAttackObject_Grab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackObject_Grab(const CAttackObject_Grab& Prototype);
	virtual ~CAttackObject_Grab() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


public:
	//void Set_RemoteDestory();

public:
	virtual void OnCollisionEnter(class CCollider* other, _float fTimeDelta) override;
	virtual void OnCollisionStay(class CCollider* other, _float fTimeDelta) override;
	virtual void OnCollisionExit(class CCollider* other) override;


private:
	void CollisingAttack();
	void CollisingPlayer();

	//카메라
	void Camera_Grab(class CCharacter* pOwner, class CCharacter* pHitOwner);
	void Camera_Same_Grab(class CCharacter* pOwner, class CCharacter* pHitOwner);

private:

	_float2 m_fDistance = {};
	//_ushort m_iGrabAnimationIndex = {};
	_float m_fGrabAnimationPosition = {};


	
	//캐릭터가 가지고있어야하는가?
	//_ushort m_iLoofCycle = 3;


public:
	static CAttackObject_Grab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
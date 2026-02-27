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

class CAttackObject_Reflect final : public CAttackObject
{
public:
	//enum HitMotion { HIT_LIGHT, HIT_MEDIUM, HIT_HEAVY, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP };

	typedef struct ATTACK_Reflect_DESC : public  CAttackObject::ATTACK_DESC
	{
		
	};
private:
	CAttackObject_Reflect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackObject_Reflect(const CAttackObject_Reflect& Prototype);
	virtual ~CAttackObject_Reflect() = default;

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


	void Erase();

private:
	void CollisingAttack();
	void CollisingPlayer();

private:

public:
	static CAttackObject_Reflect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END







/*

*/
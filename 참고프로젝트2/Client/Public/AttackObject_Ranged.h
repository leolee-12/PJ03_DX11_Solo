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

class CAttackObject_Ranged final : public CAttackObject
{
public:
	//enum HitMotion { HIT_LIGHT, HIT_MEDIUM, HIT_HEAVY, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP };

	enum Ranged_LightColor { RANGED_LIGHT_NONE, RANGED_LIGHT_YELLOW , RANGED_LIGHT_PURPLE, RANGED_LIGHT_PINK};

	typedef struct ATTACK_RANGED_DESC : public  CAttackObject::ATTACK_DESC
	{
		_float2 fStartOffset = {};
		//_vector vPosition 시작 중점 좌표는 pOnwer로부터 Transform 뜯어와서 알아서 할것

		_float2 fRanged_Impus_NoneDirection = {};  //1초간 이동할 xy거리  //방향값 미리 적용 해야하나?  
		_short iDirection = {};		//

		Ranged_LightColor eRangeColor = { RANGED_LIGHT_NONE };

		_bool bExplosion = true;
		_bool bPierce = false;
		_wstring strEffectName = {};

		_float fEffectRotationDegree = {};

		_bool bExplosionEffectisHitEffect = { false };
		_bool bGroundExplosionEffect = { false };

	};
private:
	CAttackObject_Ranged(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackObject_Ranged(const CAttackObject_Ranged& Prototype);
	virtual ~CAttackObject_Ranged() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
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

	_bool BeReflect();

private:
	void CollisingAttack();
	void CollisingPlayer();


	_bool Check_MapOut();

	void Add_YellowLight();
	void Add_YellowLight(_float3 fPosition);

private:
	_float2	m_fStartOffset = {};
	_float2 m_fRanged_Impus_NoneDirection = {};
	_short  m_iDirection = {};		
	_bool m_bExplosion = { true };
	_bool m_bPierce = { false };

	Ranged_LightColor m_eRangeColor = { RANGED_LIGHT_NONE };

	class CEffect_Layer* m_pRangedEffect_Layer = { nullptr };

	_bool m_bCreateReflectEnable = true;
	_bool m_bExplosionEffectisHitEffect = false;
	_bool m_bGroundExplosionEffect = { false };


public:
	static CAttackObject_Ranged* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
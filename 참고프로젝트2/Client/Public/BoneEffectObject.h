#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

//#include "AnimationEvent_Defines.h"
//#include "AttackObject.h"




//Prototype_GameObject_Bone_Effect_Object

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBoneEffectObject  : public CGameObject
{
public:
	//enum HitMotion { HIT_LIGHT, HIT_MEDIUM, HIT_HEAVY, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP };

	typedef struct BONE_EFFECT_DESC : public  CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
		const _float4x4* pSocketBoneMatrix;

		_wstring strEffectName = {};
	};
private:
	CBoneEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoneEffectObject(const CBoneEffectObject& Prototype);
	virtual ~CBoneEffectObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


public:
	//void Set_RemoteDestory();

public:
	

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

	/* 이 파트오브젝트를 포함하고 있는 부모 객체의 상태 행렬.  */
	const _float4x4* m_pParentMatrix = {};

	/* 나의 Transform클래스의 월드행렬은 부모기준의 상태변환행렬로 개념이 바뀐다.  */
	/* 나를 그리기위한 m_WorldMatrix = Transform`s WorldMatrix * *m_pParentMatrix */
	_float4x4				m_WorldMatrix = {};


	_float4x4				m_tFinalMatrix = {};

private:
	_bool m_bUpdateLock = false;
	_float m_fAccUpdateLockTime = 0.f;
	_float m_fMaxUpdateLickTime = 0.f;

	_short	m_iLookDirection = {};

	_wstring m_strEffectName = {};;

public:
	static CBoneEffectObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#include "AnimationEvent_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CParryingRangedObject  : public CGameObject
{
public:
	//enum HitMotion { HIT_LIGHT, HIT_MEDIUM, HIT_HEAVY, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP };
	enum Ranged_LightColor { RANGED_LIGHT_NONE, RANGED_LIGHT_YELLOW, RANGED_LIGHT_PURPLE, RANGED_LIGHT_PINK };

	typedef struct PARRYING_RANGED_DESC
	{
		//시작좌표
		_vector vPos;


		//목적지좌표 또는 날아가는 방향


		//이건 기물을 부수는 오브젝트인가
		_bool bDestroyObject = { false };

		//둘 중 하나 쓰기
		_uint iColor = 0;
		wstring strEffectName = {};

	};
protected:
	CParryingRangedObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParryingRangedObject(const CParryingRangedObject& Prototype);
	virtual ~CParryingRangedObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;



public:
	

public:



protected:
	_bool Check_UpdateStop(_float fTimeDelta);

private:

	_bool m_bDestroyObject = { false };

	_float m_fAccLifeTime = {0.f};

	//_float* m_pHeight = { nullptr };
	class CEffect_Layer* m_pEffect_Layer = { nullptr };



	_bool m_bFlipEnable = { true };

	_float2 m_fGoalPosXZ = {};
	_ushort m_iMapBreakIndex = { 60000 };

private:

protected:

	

private:
	HRESULT Ready_Components(PARRYING_RANGED_DESC* pDesc);

public:
	static CParryingRangedObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
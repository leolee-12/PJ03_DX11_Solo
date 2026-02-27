#pragma once

#include "Base.h"
//#include "StatusComp.h"

/*

*/
#define STATUS_DISPLAY_HP 0

BEGIN(Engine)
class CGameObject;
class CPhysX_Collider;
END

BEGIN(Client)

/// <summary>
/// 스테이터스 변동결과
/// </summary>
enum class EStatusModified : _uint
{
	None = EBIT_FLAG32_0,
	Block = EBIT_FLAG32_1,
	Damage = EBIT_FLAG32_2,
	Critical = EBIT_FLAG32_3,
	Burst = EBIT_FLAG32_4,
	BurstCritical = EBIT_FLAG32_5,
	Guard = EBIT_FLAG32_6,
	GuardCritical = EBIT_FLAG32_7,
	Heal = EBIT_FLAG32_8
};

/// <summary>
/// 스테이터스간의 상호작용을 담당하는 인터페이스 입니다.
/// </summary>
class IStatusInterface abstract
{
public:
	// 대미지 계산을 수행합니다. 이미 구현된 함수
	_uint Status_DamageTo(const string& strSkillName, CPhysX_Collider* pDstCol, weak_ptr<CGameObject> pDst, weak_ptr<CGameObject> pSrc);
	// 대미지를 받았을 때 작동할 함수
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) = 0;
	// 상속받아 설정하여 쓰는 함수들
	virtual HRESULT Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject) = 0;
	virtual HRESULT Set_StatusComByOwner(const string& strSkillName) = 0;
	virtual weak_ptr<class CStatusComp> Get_StatusCom() = 0;

protected:
	//weak_ptr<class CStatusComp>	m_pStatusCom;	// 대미지 계산을 위해 참조하는 스테이터스
};

END
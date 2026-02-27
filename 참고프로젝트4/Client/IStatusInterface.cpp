#include "stdafx.h"
#include "IStatusInterface.h"

#include "StatusComp.h"
#include "UI_Manager.h"

_uint IStatusInterface::Status_DamageTo(const string& strSkillName, CPhysX_Collider* pDstCol, weak_ptr<CGameObject> pDst, weak_ptr<CGameObject> pSrc)
{
	if (pDst.expired() || pSrc.expired())
	{
		assert(false);
		return 0;
	}

	auto pDstObj = DynPtrCast<IStatusInterface>(pDst.lock());
	auto pSrcObj = DynPtrCast<IStatusInterface>(pSrc.lock());

	if (!pDstObj || !pSrcObj)
	{
		assert(false);
		return 0;
	}

	weak_ptr<CStatusComp> pDstStatusWeak = pDstObj->Get_StatusCom();
	weak_ptr<CStatusComp> pSrcStatusWeak = pSrcObj->Get_StatusCom();

	if (pDstStatusWeak.expired() || pSrcStatusWeak.expired())
	{
		assert(false);
		return 0;
	}

	CStatusComp* pDstStatus = pDstStatusWeak.lock().get();
	CStatusComp* pSrcStatus = pSrcStatusWeak.lock().get();

	// 대미지 계산 및 대미지 유형 반환
	_uint iStatusModify = 0;
	_uint iHitPower = 0, iAddHitPower = 0;
#if STATUS_DISPLAY_HP == 0
	_int iDamage = pSrcStatus->Apply_Damage(strSkillName, *pDstStatus, iStatusModify, iHitPower, iAddHitPower);
#else
	_int iDamage = pSrcStatus->Apply_Damage(strSkillName, *pDstStatus, iStatusModify, iHitPower, iAddHitPower);
	iDamage = pDstStatus->Get_CurHP();
#endif
	GET_SINGLE(CUI_Manager)->Make_DamageFont(pDstCol, iDamage);

	// 대미지 결과를 대미지를 준 상대에게 알린다.
	pDstObj->Status_Damaged(iStatusModify, iHitPower, iAddHitPower);

	return iStatusModify;
}

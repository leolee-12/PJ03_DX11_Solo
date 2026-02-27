#include "stdafx.h"
#include "StatusComp.h"

#include "GameInstance.h"

_unmap<string, TSkillSchema> CStatusComp::g_SkillTable;
_unmap<string, TCharacterSchema> CStatusComp::g_CharacterTable;
_unmap<string, TEnemySchema> CStatusComp::g_EnemyTable;
_unmap<string, TEnemySchema> CStatusComp::g_BossTable;
_unmap<string, TActionInfoSchema> CStatusComp::g_ActionInfoTable;

IMPLEMENT_CREATE(CStatusComp)

CStatusComp::CStatusComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pDevice, pContext)
{

}

CStatusComp::CStatusComp(const CStatusComp& rhs)
	: BASE(rhs)
{

}

HRESULT CStatusComp::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CStatusComp::Initialize(void* pArg)
{
	if (pArg != nullptr)
		//throw runtime_error("StatusComp : Desc가 전달되지 않았습니다.");
	{
		TStatusCompDesc& Desc = (*ReCast<TStatusCompDesc*>(pArg));
		m_fHP.Readjust(Desc.fHP); m_fHP.Set_Max();
		m_fMP.Readjust(Desc.fMP); m_fMP.Set_Max();
		m_iAttack = Desc.iAttack;
		m_iDefence = Desc.iDefence;
	}

	return S_OK;
}

HRESULT CStatusComp::Initialize_SkillDataTable(const wstring& strFileName)
{
	FRapidJson Data;
	if (FAILED(Data.Input_Json(strFileName)))
		RETURN_EFAIL;

	// 전용 데이터테이블로 로드한다. MainApp에서 로드한다.
	_uint iCharacterSkillSize = Data.Read_ArraySize("CharacterSkills");
	for (_uint i = 0; i < iCharacterSkillSize; i++)
	{
		FRapidJson SkillData;
		Data.Read_ObjectFromArray("CharacterSkills", i, SkillData);

		TSkillSchema Schema;
		if (FAILED(SkillData.Read_Data("Name", Schema.strSkillName)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("DamageType", Schema.iDamageType)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("Damage", Schema.iBaseDamage)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("Penetration", Schema.iPenetration)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("Burst", Schema.iBurst)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("HitPower", Schema.iHitPower)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("AddHitPower", Schema.iAddHitPower)))
			RETURN_EFAIL;

		// 중복 이름 존재시 오류 내보냄
		if (!g_SkillTable.emplace(Schema.strSkillName, move(Schema)).second)
			RETURN_EFAIL;
	}

	// 전용 데이터테이블로 로드한다. MainApp에서 로드한다.
	_uint iEnemySkills = Data.Read_ArraySize("EnemySkills");
	for (_uint i = 0; i < iEnemySkills; i++)
	{
		FRapidJson SkillData;
		Data.Read_ObjectFromArray("EnemySkills", i, SkillData);

		TSkillSchema Schema;
		if (FAILED(SkillData.Read_Data("Name", Schema.strSkillName)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("DamageType", Schema.iDamageType)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("Damage", Schema.iBaseDamage)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("Penetration", Schema.iPenetration)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("Burst", Schema.iBurst)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("HitPower", Schema.iHitPower)))
			RETURN_EFAIL;

		if (FAILED(SkillData.Read_Data("AddHitPower", Schema.iAddHitPower)))
			RETURN_EFAIL;

		// 중복 이름 존재시 오류 내보냄
		if (!g_SkillTable.emplace(Schema.strSkillName, move(Schema)).second)
			RETURN_EFAIL;
	}

	return S_OK;
}

HRESULT CStatusComp::Initialize_CharacterDataTable(const wstring& strFileName)
{
	FRapidJson Data;
	if (FAILED(Data.Input_Json(strFileName)))
		RETURN_EFAIL;
	
	_uint iCharacterSize = Data.Read_ArraySize("CharacterLists");
	for (_uint i = 0; i < iCharacterSize; i++)
	{
		FRapidJson CharacterData;
		Data.Read_ObjectFromArray("CharacterLists", i, CharacterData);

		string strName = "";
		if (FAILED(CharacterData.Read_Data("Name", strName)))
			RETURN_EFAIL;

		TCharacterSchema Schema;
		if (FAILED(CharacterData.Read_Data("LV", Schema.iLV)))
			RETURN_EFAIL;

		if (FAILED(CharacterData.Read_Data("HP", Schema.iHP)))
			RETURN_EFAIL;
		if (FAILED(CharacterData.Read_Data("HP_Growth", Schema.iHP_Growth)))
			RETURN_EFAIL;

		if (FAILED(CharacterData.Read_Data("MP", Schema.iMP)))
			RETURN_EFAIL;
		if (FAILED(CharacterData.Read_Data("MP_Growth", Schema.iMP_Growth)))
			RETURN_EFAIL;

		if (FAILED(CharacterData.Read_Data("Attack", Schema.iAttack)))
			RETURN_EFAIL;
		if (FAILED(CharacterData.Read_Data("Attack_Growth", Schema.iAttack_Growth)))
			RETURN_EFAIL;

		if (FAILED(CharacterData.Read_Data("Magic", Schema.iMagic)))
			RETURN_EFAIL;
		if (FAILED(CharacterData.Read_Data("Magic_Growth", Schema.iMagic_Growth)))
			RETURN_EFAIL;

		if (FAILED(CharacterData.Read_Data("Defence", Schema.iDefence)))
			RETURN_EFAIL;
		if (FAILED(CharacterData.Read_Data("Defence_Growth", Schema.iDefence_Growth)))
			RETURN_EFAIL;

		if (FAILED(CharacterData.Read_Data("M_Defence", Schema.iM_Defence)))
			RETURN_EFAIL;
		if (FAILED(CharacterData.Read_Data("M_Defence_Growth", Schema.iM_Defence_Growth)))
			RETURN_EFAIL;

		if (FAILED(CharacterData.Read_Data("Exp_Gauge", Schema.iExp_Gauge)))
			RETURN_EFAIL;
		if (FAILED(CharacterData.Read_Data("Exp_Growth", Schema.iExp_Growth)))
			RETURN_EFAIL;

		// 중복 ID 존재
		if (!g_CharacterTable.emplace(strName, move(Schema)).second)
			RETURN_EFAIL;	
	}

	return S_OK;
}

HRESULT CStatusComp::Initialize_EnemyDataTable(const wstring& strFileName)
{
	FRapidJson Data;
	if (FAILED(Data.Input_Json(strFileName)))
		RETURN_EFAIL;

	_uint iEnemySize = Data.Read_ArraySize("EnemyLists");
	for (_uint i = 0; i < iEnemySize; i++)
	{
		FRapidJson EnemyData;
		Data.Read_ObjectFromArray("EnemyLists", i, EnemyData);

		string strName = "";
		if (FAILED(EnemyData.Read_Data("Name", strName)))
			RETURN_EFAIL;

		TEnemySchema Schema;
		if (FAILED(EnemyData.Read_Data("LV", Schema.iLV)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("HP", Schema.iHP)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("MP", Schema.iMP)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Attack", Schema.iAttack)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Magic", Schema.iMagic)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Defence", Schema.iDefence)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("M_Defence", Schema.iM_Defence)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Exp_Give", Schema.iExp_Give)))
			RETURN_EFAIL;

		// 중복 ID 존재
		if (!g_EnemyTable.emplace(strName, move(Schema)).second)
			RETURN_EFAIL;
	}

	_uint iBossSize = Data.Read_ArraySize("BossLists");
	for (_uint i = 0; i < iBossSize; i++)
	{
		FRapidJson EnemyData;
		Data.Read_ObjectFromArray("BossLists", i, EnemyData);

		string strName = "";
		if (FAILED(EnemyData.Read_Data("Name", strName)))
			RETURN_EFAIL;

		TEnemySchema Schema;
		if (FAILED(EnemyData.Read_Data("LV", Schema.iLV)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("HP", Schema.iHP)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("MP", Schema.iMP)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Attack", Schema.iAttack)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Magic", Schema.iMagic)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Defence", Schema.iDefence)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("M_Defence", Schema.iM_Defence)))
			RETURN_EFAIL;
		if (FAILED(EnemyData.Read_Data("Exp_Give", Schema.iExp_Give)))
			RETURN_EFAIL;

		// 중복 ID 존재
		if (!g_BossTable.emplace(strName, move(Schema)).second)
			RETURN_EFAIL;
	}

	return S_OK;
}

HRESULT CStatusComp::Initialize_ActionInfoTable(const wstring& strFileName)
{
	FRapidJson Data;
	if (FAILED(Data.Input_Json(strFileName)))
		RETURN_EFAIL;

	_uint iEnemySize = Data.Read_ArraySize("ActionInfos");
	for (_uint i = 0; i < iEnemySize; i++)
	{
		FRapidJson EnemyData;
		Data.Read_ObjectFromArray("ActionInfos", i, EnemyData);

		string strName = "";
		if (FAILED(EnemyData.Read_Data("Name", strName)))
			RETURN_EFAIL;

		TActionInfoSchema Schema;
		if (FAILED(EnemyData.Read_Data("MP", Schema.iMP)))
			RETURN_EFAIL;
		// 중복 ID 존재
		if (!g_ActionInfoTable.emplace(strName, move(Schema)).second)
			RETURN_EFAIL;
	}

	return S_OK;
}

_uint CStatusComp::Find_ConsumeMP(const string& strSkillName)
{
	auto iter = g_ActionInfoTable.find(strSkillName);
	if (iter == g_ActionInfoTable.end())
		return 0;

	return (*iter).second.iMP;
}

_uint CStatusComp::StFind_ConsumeMP(const string& strSkillName)
{
	auto iter = g_ActionInfoTable.find(strSkillName);
	if (iter == g_ActionInfoTable.end())
		return 0;

	return (*iter).second.iMP;
}

shared_ptr<CComponent> CStatusComp::Clone(void* pArg)
{
	DERIVED* pInstance = new DERIVED(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("StatusComp : Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared(pInstance);
}

void CStatusComp::Free()
{
	__super::Free();

}

_uint CStatusComp::Apply_Damage(const string& strSkillName, CStatusComp& Other, _uint& iDamageStatus, _uint& iOutHitPower, _uint& iOutAddHitPower)
{
	// Other가 대미지를 받는 스테이터스

	_int iActionPower = 0, iDamageType = 0, iPenetration = 0, iBurst = 0;
	_uint iHitPower = 0, iAddHitPower = 0;
	Other.Calculate_ActionPower(strSkillName, iActionPower, iDamageType, iPenetration, iBurst, iHitPower, iAddHitPower);

	// 대충 인터넷에 있던 공식 가져와서 때려넣어봤음.
	_float fBaseDamage 
		= ((2.f * ((m_iAttack * (iDamageType == 0)) + (m_iMagic * (iDamageType == 1))))
		- (Other.m_iDefence * (iDamageType == 0) + Other.m_iMagicDefence * (iDamageType == 1) - iPenetration))
		* iActionPower * 0.01f;
	if (fBaseDamage <= 0.f)
		fBaseDamage = 0.f;


	// 랜덤 보정
	_float fRandom = m_pGameInstance->Random(75.f, 100.f);
	fBaseDamage *= fRandom * 0.01f;


	// 레벨로 데미지 보정
	_int iLevelGap = (m_iLV - Other.m_iLV);
	fBaseDamage *= 1.f + iLevelGap * 0.025f;


	// 크리티컬 데미지 보정
	_bool bIsCritical = false;
	fRandom = m_pGameInstance->Random(0.f, 100.f);
	if (fRandom <= (_float)m_iCriticalRate)
		bIsCritical = true;
	fBaseDamage += (_float)bIsCritical * fBaseDamage * 0.5f;

	if (Other.m_bIsGuard)
		fBaseDamage *= 0.3f;

	if (fBaseDamage >= 9999.f)
		fBaseDamage = 9999.f;
	else if (fBaseDamage < 0.f)
		fBaseDamage = 0.f;

	if (Other.m_bIsInvincible)
		fBaseDamage = 0.f;

	// 상대 HP 감소
	Other.Set_Damage(fBaseDamage);
	m_iLastDamage = fBaseDamage;

	if (fBaseDamage == 0.f)
		iDamageStatus |= (_uint)EStatusModified::Block;
	// 대미지 들어갈 때 판정
	else
	{
		iDamageStatus |= (_uint)EStatusModified::Damage;
		if (bIsCritical)
			iDamageStatus |= (_uint)EStatusModified::Critical;

		if (!Other.m_fBurstTime.IsMax())
		{
			if (bIsCritical)
				iDamageStatus |= (_uint)EStatusModified::BurstCritical;
			else
				iDamageStatus |= (_uint)EStatusModified::Burst;
		}
		if (Other.m_bIsGuard)
		{
			if (bIsCritical)
				iDamageStatus |= (_uint)EStatusModified::GuardCritical;
			else
				iDamageStatus |= (_uint)EStatusModified::Guard;
		}
	}
	

	Other.m_fBurstGauge.Increase((_float)iBurst);
	iOutHitPower = iHitPower;
	iOutAddHitPower = iAddHitPower;

	return fBaseDamage;
}

_bool CStatusComp::IsCanConsume_MP(_int iConsumeMP) const
{
	return (m_fMP.fCur - (_float)iConsumeMP) >= 0.f;
}

_bool CStatusComp::Consume_MP(_int iConsumeMP)
{
	if (!IsCanConsume_MP(iConsumeMP))
		return false;

	m_fMP.Decrease((_float)iConsumeMP);

	return true;
}

void CStatusComp::Init_StatsByTable(ETableType eType, const string& strName)
{
	switch (eType)
	{
	case ETableType::Character:
	{
		auto iter = g_CharacterTable.find(strName);
		if (iter == g_CharacterTable.end())
			assert(true);

		TCharacterSchema& Schema = (*iter).second;
		m_iLV = Schema.iLV;
		m_fHP = { (_float)Schema.iHP, true };
		m_fMP = { (_float)Schema.iMP, true };
		m_iAttack = Schema.iAttack;
		m_iMagic = Schema.iMagic;
		m_iDefence = Schema.iDefence;
		m_iMagicDefence = Schema.iM_Defence;
		break;
	}
	case ETableType::Enemy:
	{
		auto iter = g_EnemyTable.find(strName);
		if (iter == g_EnemyTable.end())
			assert(true);

		TEnemySchema& Schema = (*iter).second;
		m_iLV = Schema.iLV;
		m_fHP = { (_float)Schema.iHP, true };
		m_fMP = { (_float)Schema.iMP, true };
		m_iAttack = Schema.iAttack;
		m_iMagic = Schema.iMagic;
		m_iDefence = Schema.iDefence;
		m_iMagicDefence = Schema.iM_Defence;
		break;
	}
	case ETableType::Boss:
	{
		auto iter = g_BossTable.find(strName);
		if (iter == g_BossTable.end())
			assert(true);

		TEnemySchema& Schema = (*iter).second;
		m_iLV = Schema.iLV;
		m_fHP = { (_float)Schema.iHP, true };
		m_fMP = { (_float)Schema.iMP, true };
		m_iAttack = Schema.iAttack;
		m_iMagic = Schema.iMagic;
		m_iDefence = Schema.iDefence;
		m_iMagicDefence = Schema.iM_Defence;
		break;
	}
	}

}

void CStatusComp::Init_Stats(_int iLV, _int iAttack, _int iMagic, _int iDefence, _int iMagicDefence)
{
	m_iLV = iLV;
	m_iAttack = iAttack;
	m_iMagic = iMagic;
	m_iDefence = iDefence;
	m_iMagicDefence = iMagicDefence;
}

void CStatusComp::Init_Stats(TCharacterSchema tSchema)
{
	
}

void CStatusComp::Calculate_ActionPower(const string& strActionName, _int& iDamage, _int& iDamageType, _int& iPenetration, _int& iBurst, _uint& iHitPower, _uint& iOutAddHitPower) const
{
	auto iter = g_SkillTable.find(strActionName);
	if (iter == g_SkillTable.end())
	{
		assert(false);
	}

	iDamage = (*iter).second.iBaseDamage;
	iDamageType = (*iter).second.iDamageType;
	iPenetration = (*iter).second.iPenetration;
	iBurst = (*iter).second.iBurst;
	iHitPower = (*iter).second.iHitPower;
	iOutAddHitPower = (*iter).second.iAddHitPower;

	// 대미지 = 공격 * 레벨값 * 랜덤값
	//m_fLastRandomDamage = (fRandom / 100.f);
	//iDamage = iDamage * 0.01f;
}


_int CStatusComp::Calculate_TotalPower() const
{
	_int iPower = 0;
	iPower += (2.f * (m_iAttack + m_iMagic) + m_iDefence + m_iMagicDefence) * m_iLV;

	return iPower;
}

_int CStatusComp::Calculate_PowerGap(const CStatusComp& OtherStatusComp) const
{
	_int iMyPower = Calculate_TotalPower();
	_int iOtherPower = OtherStatusComp.Calculate_TotalPower();

	return (_int)(iMyPower - iOtherPower);
}

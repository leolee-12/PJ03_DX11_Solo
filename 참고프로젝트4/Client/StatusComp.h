#pragma once

#include "Client_Defines.h"
#include "Component.h"
#include "Utility/LogicDeviceBasic.h"
#include "Utility/RapidJson_Utility.h"
#include "IStatusInterface.h"

BEGIN(Client)

#pragma region 공격 타입

// 스킬 테이블 스키마
struct TSkillSchema
{
	_int iID;
	string strSkillName;
	_int iDamageType;
	_int iBaseDamage;
	_int iPenetration;
	_int iBurst;
	_int iHitPower;
	_int iAddHitPower;
};

struct TCharacterSchema
{
	_uint iLV;
	_uint iHP;
	_uint iHP_Growth;
	_uint iMP;
	_uint iMP_Growth;
	_uint iAttack;
	_uint iAttack_Growth;
	_uint iMagic;
	_uint iMagic_Growth;
	_uint iDefence;
	_uint iDefence_Growth;
	_uint iM_Defence;
	_uint iM_Defence_Growth;
	_uint iExp_Gauge;
	_uint iExp_Growth;
};

struct TEnemySchema
{
	_uint iLV;
	_uint iHP;
	_uint iMP;
	_uint iAttack;
	_uint iMagic;
	_uint iDefence;
	_uint iM_Defence;
	_uint iExp_Give;
};

struct TActionInfoSchema
{
	_uint iMP;
};

#pragma endregion

/// <summary>
/// 캐릭터의 상태를 저장하는 컴포넌트
/// </summary>
class CStatusComp final : public CComponent
{
	INFO_CLASS(CStatusComp, CComponent)
public:
	enum class ETableType
	{
		Character,
		Enemy,
		Boss
	};

public:
	struct TStatusCompDesc
	{
		_int	iLV;
		_float	fHP;
		_float	fMP;
		_uint	iAttack;
		_uint	iDefence;
	};

private:
	CStatusComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CStatusComp(const CStatusComp& rhs);
	virtual ~CStatusComp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	// 지연로딩 방식의 생성법
	static shared_ptr<CStatusComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	// 기존 방식의 생성법
	static shared_ptr<CStatusComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

public:
	// 대미지 계산을 위한 데이터 테이블 로드
	static HRESULT Initialize_SkillDataTable(const wstring& strFileName);
	static HRESULT Initialize_CharacterDataTable(const wstring& strFileName);
	static HRESULT Initialize_EnemyDataTable(const wstring& strFileName);
	static HRESULT Initialize_ActionInfoTable(const wstring& strFileName);

public:
	_uint Find_ConsumeMP(const string& strSkillName);
	static _uint StFind_ConsumeMP(const string& strSkillName);

private:
	static _unmap<string, TSkillSchema> g_SkillTable;
	static _unmap<string, TCharacterSchema> g_CharacterTable;
	static _unmap<string, TEnemySchema> g_EnemyTable;
	static _unmap<string, TEnemySchema> g_BossTable;
	static _unmap<string, TActionInfoSchema> g_ActionInfoTable;
	
public:	// 체력관련 함수
	// 단순 GETSET 함수들
	GETSET_EX2(FGauge, m_fHP, HP, GET_C_REF, SET)
	void Init_HP(_float fMaxHP) { m_fHP.Readjust(fMaxHP); m_fHP.Set_Max(); }
	_float	Get_HP_Percent() { return m_fHP.Get_Percent(); }
	_float  Get_CurHP() const { return m_fHP.fCur; }
	_float  Get_MaxHP() const { return m_fHP.fMax; }
	_bool	IsZeroHP() { return m_fHP.fCur <= 0.f; }
	void    Set_HP_Full() { m_fHP.Set_Max(); }
	void	Set_CurHP(_float fCur) { m_fHP.fCur = fCur; }
	// 공격타입과 스테이터스 컴포넌트를 보내 대미지를 계산하고 체력을 깎게 한다.
	// 반환값은 UI 출력에 쓰인다.
	_uint	Apply_Damage(const string& strSkillName, CStatusComp& Other, _uint& iDamageStatus, _uint& iHitPower, _uint& iOutAddHitPower);

	void	Set_Damage(_float fDamage) { m_fHP.Decrease(fDamage); }
	
public:	// MP관련 함수
	// 단순 GETSET 함수들
	GETSET_EX2(FGauge, m_fMP, MP, GET_C_REF, SET)
	void Init_MP(_float fMaxMP) { m_fMP.Readjust(fMaxMP); m_fMP.Set_Max(); }
	_float	Get_MP_Percent() { return m_fMP.Get_Percent(); }
	_float  Get_CurMP() const { return m_fMP.fCur; }
	_float  Get_MaxMP() const { return m_fMP.fMax; }
	void	Set_CurMP(_float fCur) { m_fMP.fCur = fCur; }
	// 단순히 MP 소모가 가능한지 판별하는 함수이다.
	_bool IsCanConsume_MP(_int iConsumeMP) const;
	// 반환값은 소모가 가능했는지 판단하고 MP를 깎는다.
	_bool Consume_MP(_int iConsumeMP);

public:
	const FGauge& CRef_LimitBreak() const { return m_fLimitBreak; }
	_bool IsCanUse_LimitBreak() { return m_fLimitBreak.IsMax(); }

	//유진 // Limit 연결 위해 임의로 만듬
	void	Init_LimitBreak(_float fMaxLimitBreak) { m_fLimitBreak.Readjust(fMaxLimitBreak);}
	_bool	Add_LimitBreak(_float _AddLP) { return m_fLimitBreak.Increase(_AddLP); }
	_float  Get_CurLimitBreak() const { return m_fLimitBreak.fCur; }
	_float  Get_MaxLimitBreak() const { return m_fLimitBreak.fMax; }


protected:
	FGauge			m_fHP;			// 체력 수치
	FGauge			m_fMP;			// 마법 사용시 씀
	FGauge			m_fATB;			// 2가 최대치
	FGauge			m_fLimitBreak;	// 필살기 게이지

public:
	// 테이블 로드 세팅, 테이블에서 정보를 찾아 스탯을 적용해준다.
	void Init_StatsByTable(ETableType eType, const string& strName);
	// 수동 세팅
	void Init_Stats(_int iLV, _int iAttack, _int iMagic, _int iDefence, _int iMagicDefence);
	// 수동 세팅, 그러나 캐릭터의 데이터를 매니저에 업로드하고 쓸 때 사용할 함수. (준비중)
	void Init_Stats(TCharacterSchema tSchema);
	// 계산해줄 행동에 대한 공격력을 돌려준다.
	void Calculate_ActionPower(const string& strSkillName, _int& iDamage, _int& iDamageType, _int& iPenetration, _int& iBurst, _uint& iHitPower, _uint& iOutAddHitPower) const;

#pragma region 전투관련
protected:
	_int			m_iLV = { 1 };
	_int			m_iAttack = { 0 };
	_int			m_iMagic = { 0 };
	_int			m_iDefence = { 0 };
	_int			m_iMagicDefence = { 0 };
	_int			m_iCriticalRate = { 25 };
#pragma endregion


#pragma region 평가치관련 (AI용)
public:
	// 행동 위력 업데이트, 필요한 경우 상태별로 세팅해주어야 한다. (스킬 위력이 아님)
	// 상태를 처음 들어갈 때 사용한다.
	void Update_ActionPower(_int iPower) { m_iActionPower = iPower; }
	// 행동 위력을 더하는 업데이트, 행동의 위력이 점차 증가하는 
	void Update_ActionPowerAdd(_int iPower) { m_iActionPower += iPower; }
	// 행동에 대한 위력을 리셋한다.
	void Reset_ActionPower() { m_iActionPower = 0; }
	// 행동에 대한 비교를 한다.
	_bool IsActionPowerEqGreater(_int iCompare) const { return m_iActionPower >= iCompare; }
	_bool IsActionPowerEqLess(_int iCompare) const { return m_iActionPower <= iCompare; }
	_bool IsActionPowerEqual(_int iCompare) const { return m_iActionPower == iCompare; }
	_int Get_ActionPower() const { return m_iActionPower; }

	// 상태를 처음 들어갈 때 사용한다.
	void Update_ActionGap(_int iGap) { m_iActionGap = iGap; }
	// 행동 위력을 더하는 업데이트, 행동의 빈틈 증가
	void Update_ActionGapAdd(_int iGap) { m_iActionGap += iGap; }
	// 행동에 대한 위력을 리셋한다.
	void Reset_ActionGap() { m_iActionGap = 0; }
	// 행동에 대한 비교를 한다.
	_bool IsActionGapEqGreater(_int iCompare) const { return m_iActionGap >= iCompare; }
	_bool IsActionGapEqLess(_int iCompare) const { return m_iActionGap <= iCompare; }
	_int Get_ActionGap() const { return m_iActionGap; }

	void Reset_AllActionValue() { m_iActionPower = 0; m_iActionGap = 0; }
	
	// 전투력 평가치 (전체적인 능력치에 기반함)
	_int Calculate_TotalPower() const;
	// 전투력차이 평가치
	_int Calculate_PowerGap(const CStatusComp& OtherStatusComp) const;
protected:
	_int m_iActionPower = { 0 };
	_int m_iActionGap = { 0 };
#pragma endregion



#pragma region 상태관련
public:
	GETSET_EX2(_bool, m_bIsGuard, Guard, GET_C_REF, SET_C_REF)
	GETSET_EX2(_bool, m_bIsInvincible, IsInvincible, GET_C_REF, SET_C_REF)

protected:
	FGauge	m_fBurstGauge = { 1000.f };
	FGauge	m_fBurstTime = { 10.f, true };
	_bool	m_bIsGuard = { false };
	_bool	m_bIsInvincible = { false };
#pragma endregion


private:
	mutable _float	m_fLastRandomDamage = { 1.f };
	// 마지막으로 받은 대미지. (UI)
	mutable _int	m_iLastDamage = { 0 };

};

END
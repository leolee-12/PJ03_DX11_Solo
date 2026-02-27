#pragma once
#include "Client_Defines.h"
#include "DynamicObject.h"
#include "Utility/LogicDeviceBasic.h"
#include "IStatusInterface.h"

#include "Effect_Manager.h"

BEGIN(Client)

class CStatusComp;

class CCharacter : public CDynamicObject, public IStatusInterface
{
	INFO_CLASS(CCharacter, CDynamicObject)

protected:
	CCharacter(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

protected:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	weak_ptr<CPartObject> TurnOn_Weapon(wstring strTag = L"");
	weak_ptr<CPartObject> TurnOff_Weapon(wstring strTag = L"");
	weak_ptr<CPartObject> TurnOn_PartEffect(wstring strTag = L"");
	weak_ptr<CPartObject> TurnOff_PartEffect(wstring strTag = L"");

public:
	virtual void OnCollision_Enter(class CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Stay(class  CCollider* pThisCol, class  CCollider* pOtherCol);
	virtual void OnCollision_Exit(class  CCollider* pThisCol, class  CCollider* pOtherCol);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

	virtual void PhysX_Raycast_Stay(weak_ptr<CGameObject> pOther, _uint iOtherColLayer, _float4 vOrigin, _float4 vDirection, _float4 vColPosition);
public:
	static shared_ptr<CCharacter> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

protected:
	_uint	m_iShaderPassIndex = { 0 };

#pragma region IStatusInterface
public:
	// GameObject중에 Status를 가지는 하위 클래스를 찾아 스테이터스를 전달 받습니다.
	virtual HRESULT Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject) override { return S_OK; }
	virtual HRESULT Set_StatusComByOwner(const string& strSkillName) override { return S_OK; }
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override {}
	virtual weak_ptr<class CStatusComp> Get_StatusCom() override { return m_pStatusCom; }

protected:
	shared_ptr<CStatusComp>		m_pStatusCom = { nullptr };
#pragma endregion

#pragma region Dissolve

public:
	void		Update_Dissolve(_cref_time fTimeDelta,_float fAmountSpeed = 1.f, _float fGradiationDistanceSpeed = 1.f);
	HRESULT		Ready_DissovleTex();
	void		Dead_DissolveType(wstring strParticleDataName);

protected:
	_bool		m_bDissovleStart = { false }; // 죽음 조건이 처리되면 디졸브 실행
	_bool		m_bDissovleCheck = { false }; // 디졸브 한 번만 실행

	_float		m_fDissolveAmout = { 0.f };
	_float		m_fDissolveGradiationDistance = { 0.f };

	_float4		m_vDissolveGradiationStartColor = { 0.f,0.f,0.f,0.f };
	_float4		m_vDissolveGradiationEndColor = { 0.f,0.f,0.f,0.f };

	wstring		m_strDissolveTextureTag;

#pragma endregion

protected:
	FDelay		m_fDamageFont_Delay = FDelay(0.3f);
	FTimeChecker	m_TimeChecker = FTimeChecker(1.f);
};

END
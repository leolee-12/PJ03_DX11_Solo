#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"
#include "Battle_AnimDef.h"

#include "ContainerObject.h"

NS_BEGIN(Engine)
class CRenderRule;
NS_END

NS_BEGIN(Game_PKM)
class CBattle_Manager;
class CBody;

class CBattle_Pokemon final : public CContainerObject
{
public:
	struct POKEMON_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		POKEMON_INSTANCE* pInstance = { nullptr };
		_uint iSide = { g_kBattleSide_Player };
		WNameID strBodyProtoTag = {};
		WNameID strShaderProtoTag = {};
		const CRenderRule* pRenderRule = { nullptr };
		_uint iDefaultAnim = { 0 };
		_bool bLoop = { true };
		_float fScale = { 1.f };
		_bool bStartVisible = { true };

		_float3 vPos = { 0.f, 0.f, 0.f };
		_float fYaw = { 0.f };
	};

protected:
	CBattle_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattle_Pokemon(const CBattle_Pokemon& Prototype);
	virtual ~CBattle_Pokemon() = default;

public:
	virtual _string Get_TypeName() const override { return "Pokemon"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	// P7-B 신규: anim 제어 + 페이싱 락 연동
	void  Set_Manager(CBattle_Manager* pManager);
	HRESULT Apply_Switch(POKEMON_INSTANCE* pNewInstance);
	void Play_Attack(ANIM_KIND eAttackKind = ANIM_KIND::ATTACK_PHYSICAL);
	void  Play_Hurt();
	void  Play_Enter();
	void  Play_Faint();
	void  Return_To_Idle();
	_bool Is_Anim_Finished() const { return ANIM_KIND::IDLE == m_eCurrentKind || ANIM_KIND::FAINT == m_eCurrentKind; }

	void  Begin_SendOutAppear();
	_float3 Get_EffectPivot() const;
	void  Set_BattleVisible(_bool bVisible) { m_bBattleVisible = bVisible; }
	_bool Is_BattleVisible() const { return m_bBattleVisible; }

private:
	POKEMON_INSTANCE* m_pInstance = { nullptr };
	_uint m_iSide = { g_kBattleSide_Player };
	WNameID m_strBodyProtoTag = {};
	WNameID m_strShaderProtoTag = {};
	WNameID m_strSpeciesModelTag = {};
	_uint m_iDefaultAnim = { 0 };
	_bool m_bLoop = { true };
	_float m_fScale = { 1.f };
	const CRenderRule* m_pRenderRule = { nullptr };

	// P7-B 신규
	CBattle_Manager* m_pManager = { nullptr };  // weak
	CBody* m_pBody = { nullptr };  // weak - Container owns

	ANIM_KIND	m_eCurrentKind = { ANIM_KIND::IDLE };
	_float		m_fAnimTimer = { 0.f };
	_float		m_fAnimDuration = { 0.f };  // 0 = loop, >0 = 시간 만료 후 자동 IDLE
	_bool		m_bLockHeld = { false };
	_bool		m_bBattleVisible = { true };

private:
	HRESULT Ready_PartObjects();
	void    Play_Anim_NonLoop(ANIM_KIND eKind, _float fDuration);
	_float3 Get_SendOutEffectPos() const;

public:
	static CBattle_Pokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END
#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_AnimDef.h"
#include "GameObject.h"
#include "RenderProfile.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CMonster final : public CGameObject
{
public:
	struct MONSTER_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iComponentLevel = ETOUI(LEVEL::GAMEPLAY);
		WNameID strShaderProtoTag = PROTO_COM_SHADER_VTXMESH;
		WNameID strModelProtoTag = PROTO_COM_MODEL_PM0025_00;
		const CRenderRule* pRenderRule = { nullptr };
		const _char* pRenderMappingPath = { nullptr };
		_float fScale = { 1.f };

		_bool  bActivateOnCreate = { false };
		_float fIdleVariantBaseInterval = { 4.0f };
		_float fIdleVariantJitter = { 1.5f };
		ANIM_KIND eInitialSpecialKind = { ANIM_KIND::END };
	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	class Engine::CModel* Get_Model() const { return m_pModelCom; }

	virtual _string Get_TypeName() const { return "Monster"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Activate(ANIM_KIND eInitialSpecialKind = ANIM_KIND::END);
	void Deactivate();
	_bool Is_Active() const { return m_bActive; }

	_bool Play_SpecialAnim(ANIM_KIND eKind);
	void Return_To_Idle();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_uint m_iComponentLevel{};
	WNameID m_strShaderProtoTag{};
	WNameID m_strModelProtoTag{};

	CRenderProfile m_RenderProfile;
	const CRenderRule* m_pRenderRule = { nullptr }; // weak

	_float m_fAlpha = { };
	_float m_fDir = { 3.f };

	_bool m_bActive = { false };
	ANIM_KIND m_eCurrentAnimKind = { ANIM_KIND::IDLE };

	_float m_fIdleVariantElapsed = { 0.f };
	_float m_fNextIdleVariantTime = { 0.f };

	_float m_fIdleVariantBaseInterval = { 4.0f };
	_float m_fIdleVariantJitter = { 1.5f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	_bool Play_IdleVariant(ANIM_KIND eKind);
	_bool Play_RandomIdleVariant();
	_bool Is_CustomAnimDefined(ANIM_KIND eKind) const;
	void Schedule_NextIdleVariant();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END
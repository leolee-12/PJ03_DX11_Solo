#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"

#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CBattle_Trainer final : public CGameObject
{
public:
	struct BATTLE_TRAINER_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iSide = { g_kBattleSide_Player };
		WNameID strModelProtoTag = {};
	};

protected:
	CBattle_Trainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattle_Trainer(const CBattle_Trainer& Prototype);
	virtual ~CBattle_Trainer() = default;

public:
	virtual _string Get_TypeName() const override { return "BattleTrainer"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_uint m_iSide = { g_kBattleSide_Player };
	WNameID m_strModelProtoTag = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBattle_Trainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END
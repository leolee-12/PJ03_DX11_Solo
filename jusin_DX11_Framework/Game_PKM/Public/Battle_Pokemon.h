#pragma once
#include "Game_PKM_Defines.h"
#include "Battle_Session.h"

#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CBattle_Pokemon final : public CGameObject
{
public:
	struct POKEMON_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		POKEMON_INSTANCE* pInstance = { nullptr };
		_uint iSide = { g_kBattleSide_Player };
	};

protected:
	CBattle_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattle_Pokemon(const CBattle_Pokemon& Prototype);
	virtual ~CBattle_Pokemon() = default;

public:
	virtual _string Get_TypeName() const { return "Pokemon"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	POKEMON_INSTANCE* m_pInstance = { nullptr };
	_uint m_iSide = { g_kBattleSide_Player };
	WNameID m_strSpeciesModelTag = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBattle_Pokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END
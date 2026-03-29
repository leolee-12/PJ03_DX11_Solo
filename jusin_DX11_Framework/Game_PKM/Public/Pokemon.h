#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CPokemon final : public CGameObject
{
public:
	struct POKEMON_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iDexNum;								// 도감 번호
		_tchar szName[32];							// 이름
		_ubyte eType1, eType2;						// 타입, 타입2는 단일 타입인 경우 TYPE_END
		_ushort iBaseHP, iBaseAtk, iBaseDef, iBaseSpAtk, iBaseSpDef, iBaseSpd;	// 종족값
		_uint iAbility1, iAbility2, iHiddenAbility;	// 특성, 특성2는 단일 특성인 경우 ABILITY_END
		_uint iLearnset[MAX_LEARNSET];				// 기술 습득 정보
	};

protected:
	CPokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPokemon(const CPokemon& Prototype);
	virtual ~CPokemon() = default;

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

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPokemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END
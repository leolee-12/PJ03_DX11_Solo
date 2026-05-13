#pragma once
#include "Game_PKM_Defines.h"
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
	_uint m_iCurrAnim = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END
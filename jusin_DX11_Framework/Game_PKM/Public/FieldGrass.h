#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"
#include "RenderProfile.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CRenderRule;
NS_END

NS_BEGIN(Game_PKM)

class CFieldGrass final : public CGameObject
{
public:
	struct FIELDGRASS_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID strModelTag = PROTO_COM_MODEL_FIELD_GRASS;
		_uint iModelLevelIndex = ETOUI(LEVEL::GAMEPLAY);
		const CRenderRule* pRenderRule = nullptr;
		_float fYaw = 0.f;
		_float fScale = 1.f;
	};

private:
	CFieldGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFieldGrass(const CFieldGrass& Prototype);
	virtual ~CFieldGrass() = default;

public:
	virtual _string Get_TypeName() const override { return "FieldGrass"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	FIELDGRASS_DESC m_tDesc = {};
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CRenderProfile m_RenderProfile;

public:
	static CFieldGrass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END
#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"
#include "RenderProfile.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CMapObject final : public CGameObject
{
public:
	struct MAPOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID strModelTag = {};
		_uint	iModelLevelIndex = {};
		const CRenderRule* pRenderRule = { nullptr }; // weak
	};

protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MAPOBJECT_DESC& tDesc);
	virtual ~CMapObject() = default;

public:
	class Engine::CModel* Get_Model() const { return m_pModelCom; }

	virtual _string Get_TypeName() const { return "Map"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	MAPOBJECT_DESC m_tDesc = {};

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderProfile m_RenderProfile;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MAPOBJECT_DESC& tDesc);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END
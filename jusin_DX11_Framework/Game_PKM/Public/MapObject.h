#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CMapObject final : public CGameObject
{
	struct MAPOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{

	};

protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject(const CMapObject& Prototype);
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


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END
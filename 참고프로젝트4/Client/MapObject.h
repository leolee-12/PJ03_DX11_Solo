#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMapObject : public CGameObject
{
	INFO_CLASS(CMapObject, CGameObject)

public:
	enum MODEL_TYPE { BOX, CYLINDER, CONE, NONE, TYPE_END };
	enum LIGHT_OBJECT_TYPE { NONE_LIGHTOBJ, WHITE_LIGHTOBJ, YELLOW_LIGHTOBJ, RED_LIGHTOBJ, LIGHTOBJ_TYPEEND };

	typedef struct tagModelDesc : public GAMEOBJECT_DESC
	{
		wstring strModelTag = L"";

	}MODEL_DESC;

private:
	CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

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
private:
	wstring m_szModelTag; 
	MODEL_TYPE m_ModelType = { MODEL_TYPE::NONE };
	LIGHT_OBJECT_TYPE	m_iLightObjectType = { NONE_LIGHTOBJ };

public:
	GETSET_EX1(wstring, m_szModelTag, ModelTag, GET);
	GETSET_EX2(MODEL_TYPE, m_ModelType, ModelType, GET,SET);
	GETSET_EX2(LIGHT_OBJECT_TYPE, m_iLightObjectType, LightObjectType, GET, SET)

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	HRESULT Change_ModelTag(wstring ModelTag);

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public:
	static shared_ptr<CMapObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;
};

END
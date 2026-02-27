#pragma once

#include "Client_Defines.h"
#include "DynamicObject.h"

BEGIN(Client)

class CInteractionBox : public CDynamicObject
{
	INFO_CLASS(CInteractionBox, CDynamicObject)

	enum BOX_TYPE {METAL,WOOD};

private:
	CInteractionBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CInteractionBox(const CInteractionBox& rhs);
	virtual ~CInteractionBox() = default;

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

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

private:
	wstring m_szModelTag = { L"" };
	BOX_TYPE m_eBoxType = { METAL };

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public: 
	GETSET_EX1(wstring, m_szModelTag, ModelTag, GET)
	GETSET_EX2(BOX_TYPE, m_eBoxType, BoxType, GET, SET)

public:
	static shared_ptr<CInteractionBox> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

END
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CTool_Trigger final : public CGameObject
{
	INFO_CLASS(CTool_Trigger, CGameObject)

public:
	enum TRIGGER_TYPE { ENTER, EXIT };

	typedef struct tagTriggerDesc :public GAMEOBJECT_DESC
	{
		TRIGGER_TYPE eType;
		_float3 vPos;
		string strName;
	}TRIGGERDESC;

private:
	CTool_Trigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTool_Trigger(const CTool_Trigger& rhs);
	virtual ~CTool_Trigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	string m_szTrigger_Tag;
	TRIGGER_TYPE m_eType = ENTER;
public:
	GETSET_EX2(string, m_szTrigger_Tag, TriggerTag, GET, SET);
	GETSET_EX2(TRIGGER_TYPE, m_eType, Type, GET,SET);

private:
	HRESULT Ready_Components(void* pArg);

public:
	static shared_ptr<CTool_Trigger> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;
};

END
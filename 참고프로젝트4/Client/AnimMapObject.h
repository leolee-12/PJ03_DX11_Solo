#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "ComputeShaderComp.h"

BEGIN(Client)

class CAnimMapObject : public CGameObject
{
	INFO_CLASS(CAnimMapObject, CGameObject)

public:
	struct TMODEL_DESC : public GAMEOBJECT_DESC
	{
		wstring strModelTag;
		_float3 vPosition;
	};

private:
	CAnimMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAnimMapObject(const CAnimMapObject& rhs);
	virtual ~CAnimMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public:
	static shared_ptr<CAnimMapObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	wstring		m_strModelTag = TEXT("");
	_float3		m_vPosition = { 0.f, 0.f, 0.f };
	_bool		m_bIsAnimPlaying = { false };
	_bool		m_bIsAnimPlayLever = { false };
	_bool		m_bCollision = { false };
	_bool		m_bLeverOnce = { true };

	_float		m_fTimeAcc = { 0.f };
	_bool		m_bFirst = { true };

	_bool		m_bDoor1 = { true };
	_bool		m_bDoor2 = { true };
	_bool		m_bDoor3_1 = { true };
	_bool		m_bDoor3_2 = { true };

	_bool		m_bIsOpenBox = { false };

public:
	GETSET_EX1(wstring, m_strModelTag, ModelTag, GET);
	GETSET_EX2(_float3, m_vPosition, Position, GET,SET);
	GETSET_EX2(_bool, m_bIsAnimPlaying, IsAnimPlaying, GET, SET);
	GETSET_EX2(_bool, m_bIsAnimPlayLever,IsAnimPlayLever, GET, SET);
	GETSET_EX2(_bool, m_bCollision, Collision, GET, SET);


};

END
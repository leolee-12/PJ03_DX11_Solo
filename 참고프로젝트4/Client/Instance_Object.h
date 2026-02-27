#pragma once

#include "GameObject.h"
#include "Client_Defines.h" 

BEGIN(Engine)

class CModel_Inst;

END

BEGIN(Client)

class CInstance_Object : public CGameObject
{
public:
	typedef struct tag_InstObjectDesc
	{
		string					strName;
		vector<_float4x4>		vecInstanceVertex;

	}INSTANCE_OBJECT_DESC;

protected:
	CInstance_Object(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CInstance_Object(const CInstance_Object& rhs);
	virtual ~CInstance_Object() = default;

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

protected:
	string m_strModelTag;
	_float3 m_vPosition;
	_float3 m_vScale;
	_float3 m_vRotation;
	_float	m_fAnge;
	_float4x4 m_matWorldMatrix;
	_float	fRadius;

	_uint	m_iPassIndex;
	wstring m_szModelTag;

	 shared_ptr<class CModel_Inst> m_pInstanceModel;

public:
	GETSET_EX2(wstring, m_szModelTag, ModelTag, GET,SET);

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CInstance_Object> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;
};

END
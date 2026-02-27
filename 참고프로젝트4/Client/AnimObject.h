#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "ComputeShaderComp.h"

BEGIN(Client)

/// <summary>
/// 애님툴 전용 객체
/// </summary>
class CAnimObject : public CGameObject
{
	INFO_CLASS(CAnimObject, CGameObject)

public:
	struct TMODEL_DESC : public GAMEOBJECT_DESC
	{
		wstring strModelTag;
	};

private:
	CAnimObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAnimObject(const CAnimObject& rhs);
	virtual ~CAnimObject() = default;

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
	HRESULT Change_ModelTag(wstring ModelTag);

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public:
	static shared_ptr<CAnimObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	wstring m_strModelTag = TEXT("");

public:
	GETSET_EX1(wstring, m_strModelTag, ModelTag, GET)

public:
	void Set_AnimPlay(_bool value) { m_bIsAnimPlaying = value; }
	_bool Get_IsAnimPlaying() const { return m_bIsAnimPlaying; }

private:
	_bool		m_bIsAnimPlaying = { true };


#pragma region 테스트 공간
private:
	HRESULT Create_ComputeBuffer();
	HRESULT Create_ComputeUAV();
	HRESULT Dispatch_ComputeShader();

	struct Constants
	{
		_uint iCount;
		_uint iPadding[3];
	};

private:
	shared_ptr<CComputeShaderComp>	m_pCSShader = { nullptr };
	ComPtr<ID3D11Buffer>			m_pBuffer1 = { nullptr };
	ComPtr<ID3D11Buffer>			m_pBuffer2 = { nullptr };
	ComPtr<ID3D11Buffer>			m_pBackBuffer = { nullptr };

	ComPtr<ID3D11ShaderResourceView>	m_pSRV1 = { nullptr };
	ComPtr<ID3D11ShaderResourceView>	m_pSRV2 = { nullptr };

	ComPtr<ID3D11UnorderedAccessView>	m_pUAV1 = { nullptr };
	ComPtr<ID3D11UnorderedAccessView>	m_pUAV2 = { nullptr };

	ComPtr<ID3D11Buffer>			m_pConstantBuffer = { nullptr };
#pragma endregion

};

END
#pragma once

#include "Component.h"
#include "Utility/DelegateTemplate.h"

#ifdef _DEBUG
#ifndef SHADER_COMP_DEBUG
#define SHADER_COMP_DEBUG 1
#endif  
#endif // _DEBUG




BEGIN(Engine)
class FEffectData;

class ENGINE_DLL CShader final : public CComponent
{
	INFO_CLASS(CShader,CComponent)

private:
	CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;


public:	// 외부용 함수
	// 셰이더 컴포넌트 중에 원하는 컴포넌트를 찾는다.
	static shared_ptr<CShader> Find_ShaderCompByID(_uint iID);
	// 이펙트 불러오기
	HRESULT Link_Effect(const wstring & strEffectFileName, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements, const D3D_SHADER_MACRO * pShaderMacro = nullptr);
	// 이펙트 풀기
	HRESULT Unlink_Effect();
	// 외부에서 
	HRESULT	IsRender_Ready();
	// 오너 세팅
	void Set_Owner(weak_ptr<CBase> pOwner) { m_pBaseOwner = pOwner; }
	// 오너 락킹, 원하는 타입으로. 렌더링을 기반으로 얻게되는 데이터를 기반으로 CPU에서도 처리하기 위해 쓰인다.
	template<typename T>
	shared_ptr<T>	Lock_Owner()
	{
		if (m_pBaseOwner.expired())
			return nullptr;

		return DynPtrCast<T>(m_pBaseOwner.lock());
	}

private:

	// 내부적으로 쓰이는 셰이더 매니저 링크 함수. 이를 통해 전역적으로 셰이더를 등록하고 해제하는 행위를 한다.
	HRESULT Link_ToShaderManager();
	
	static _uint Find_Unused_RenderID(_bool bAllocate);
	void Allocate_RenderID();
	// 자신이 가지고 있는 ID를 관리에서 제외시킨다.
	void Remove_RenderID();
	

private:
	// 연결된 셰이더 매니저
	class CShader_Manager*	m_pShaderManager = { nullptr };
	// CBase를 오너로 삼을 수 있다. DynPtrCast를 활용해 다양한 곳에서 사용할 수 있도록 한다.
	weak_ptr<CBase>			m_pBaseOwner;

	// 사용중인 ID
	static vector<_uint_64>				g_UsingRenderIDs;
	// 등록된 셰이더 컴포넌트
	static vector<weak_ptr<CShader>>	g_ShaderComps;
	
	
public:
	// Begin과 동시에 BindRenderID도 작동시킬 수 있다. 해당 함수는 필요한 곳에서 전역적으로 쓰도록 할 수 있다.
	HRESULT Begin(_uint iPassIndex, _bool bBindRenderID = false, 
		 const _char* pConstantName = "g_iRenderID", const _char* pFlagConstantName = "g_iRenderFlag");
	HRESULT Bind_RenderID(const _char* pConstantName, const _char* pFlagConstantName);
	HRESULT Bind_Matrix(const _char * pConstantName, const _float4x4 * pMatrix);
	HRESULT Bind_Matrices(const _char * pConstantName, const _float4x4 * pMatrix, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char * pConstantName, ID3D11ShaderResourceView * pSRV);
	HRESULT Bind_SRVs(const _char * pConstantName, ID3D11ShaderResourceView * *ppSRV, _uint iNumTextures);
	HRESULT Bind_RawValue(const _char * pConstantName, const void* pData, _uint iSize);

private:
	// 이펙트 데이터를 보관하는 객체연결, 실제 관리는 매니저에서 이뤄진다.
	shared_ptr<FEffectData>		m_pEffectData = { nullptr };
	// 렌더링 ID를 기록하기 위해 쓰이는 식별번호
	_uint						m_iRenderID = { UINT_MAX };
	// 셰이더 코드내에서 렌더플래그 값을 사용하여 다양한 처리를 할 수 있다.
	_uint						m_iRenderFlag = { 0 };

public:
	// 지연로딩 방식의 생성법
	static shared_ptr<CShader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	// 기존 방식의 생성법
	static shared_ptr<CShader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

};

END
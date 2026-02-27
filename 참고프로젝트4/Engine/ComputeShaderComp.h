#pragma once

#include "Component.h"
#include "Shader_Manager.h"


BEGIN(Engine)

/// <summary>
/// 컴퓨트 셰이더를 사용하도록 제공되는 컴포넌트 방식의 인터페이스 입니다.
/// </summary>
class ENGINE_DLL CComputeShaderComp : public CComponent
{
	INFO_CLASS(CComputeShaderComp, CComponent)

private:
	CComputeShaderComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CComputeShaderComp(const CComputeShaderComp& rhs);
	virtual ~CComputeShaderComp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath, const _char* pEntryPoint);
	virtual HRESULT Initialize(void* pArg) override;


public:	// 외부용 함수
	// 이펙트 불러오기
	HRESULT Link_Shader(const wstring& strEffectFileName, const _char* pEntryPoint, const D3D_SHADER_MACRO* pShaderMacro = nullptr);
	// 이펙트 풀기
	HRESULT Unlink_Shader();
	// 외부에서 
	HRESULT	IsShader_Ready();

private:
	// 내부적으로 쓰이는 셰이더 매니저 링크 함수. 이를 통해 전역적으로 셰이더를 등록하고 해제하는 행위를 한다.
	HRESULT Link_ToShaderManager();

private:
	class CShader_Manager* m_pShaderManager = { nullptr };


public:
	// 지연로딩 방식의 생성법
	static shared_ptr<CComputeShaderComp>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	// 기존 방식의 생성법
	static shared_ptr<CComputeShaderComp>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strShaderFilePath, const _char* pEntryPoint);
	virtual shared_ptr<CComponent>			Clone(void* pArg) override;
	virtual void Free() override;

public:
	const ComPtr<ID3D11ComputeShader>& CRef_Shader() const { return m_pShader; }

private:
	ComPtr<ID3D11ComputeShader>		m_pShader = { nullptr };

};

END
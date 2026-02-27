#pragma once

#include "Base.h"
#include "ShaderMgr_Enum.h"

BEGIN(Engine)


/// <summary>
/// 일반 셰이더파일 저장 데이터
/// </summary>
class FShaderData final
{
	THIS_CLASS(FShaderData)
private:
	FShaderData() = default;
public:
	~FShaderData()
	{
	}

public:
	HRESULT Initialize()
	{
		return S_OK;
	}

public:
	static FShaderData* Create()
	{
		ThisClass* pInstance = new ThisClass();

		if (FAILED(pInstance->Initialize()))
		{
			Engine::Safe_Delete(pInstance);
			MSG_BOX("RenderMgr Create Failed");
		}

		return pInstance;
	}

	void Free() 
	{
		delete this;
	}
	void Set_Shader(const wstring& _strFileName, ID3DBlob* _pShaderByte, ID3D11DeviceChild* _pShaderBuffer)
	{
		if (_pShaderByte == nullptr || _pShaderBuffer == nullptr)
			return;

		strFileName = _strFileName;
		bLoaded = true;
		pShaderByte = _pShaderByte;
		pShaderBuffer = _pShaderBuffer;
	}

public:
	const ComPtr<ID3DBlob>&				CRef_ShaderByte() const { return pShaderByte; }
	const ComPtr<ID3D11DeviceChild>&	CRef_ShaderBuffer() const { return pShaderBuffer; }
	const _bool							IsLoaded() const { return bLoaded; }

private:
	wstring							strFileName = L"";				// 로드할 셰이더 코드, 파일 링크용
	_bool							bLoaded = false;				// 로드됨
	ComPtr<ID3DBlob>				pShaderByte = { nullptr };		// 셰이더 바이트 코드
	ComPtr<ID3D11DeviceChild>		pShaderBuffer = { nullptr };	// 셰이더 버퍼
};


/// <summary>
/// FX11 저장 데이터
/// </summary>
class FEffectData final : public CBase
{
	DERIVED_CLASS(CBase, FEffectData)

private:
	FEffectData() = default;
	~FEffectData() = default;

public:
	HRESULT Initialize() { return S_OK; }

public:
	static shared_ptr<FEffectData> Create()
	{
		ThisClass* pInstance = new ThisClass();

		if (FAILED(pInstance->Initialize()))
		{
			MSG_BOX("RenderMgr Create Failed");
			Safe_Release(pInstance);
		}

		return BaseMakeShared(pInstance);
	}

public:
	virtual void Free() {}

public:
	D3DX11_TECHNIQUE_DESC				tTechniqueDesc;		// 테크니션 설명 객체
	ComPtr<ID3DX11Effect>				pEffect;			// FX 셰이더 버퍼
	vector<ComPtr<ID3D11InputLayout>>	pInputLayouts;		// 레이아웃 벡터

public:
	const D3D11_INPUT_ELEMENT_DESC*	pElements = { nullptr };	// 재컴파일 전용
	_uint							iNumElements = { 0 };		// 재컴파일 전용
	const D3D_SHADER_MACRO*			pShaderMacro = { nullptr };	// 재컴파일 전용
};


/// <summary>
/// 셰이더 코드를 저장하고 관리해주는 매니저입니다.
/// 기존 컴포넌트 방식으로 써도 되고
/// 매니저를 사용하여 지연로딩으로 사용하여도 좋습니다.
/// </summary>
class CShader_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CShader_Manager)
private:
	explicit CShader_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CShader_Manager(const CShader_Manager& rhs) = delete;
	virtual ~CShader_Manager() = default;

public:
	HRESULT					Initialize(const wstring& strMainPath);

public:
	static CShader_Manager*	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strMainPath);

private:
	virtual void			Free() override;

#pragma region 디바이스
private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pDeviceContext = { nullptr };
#pragma endregion



#pragma region 일반 셰이더 (FX11 아님)
public:
	// 파일이름을 통해 셰이더를 로드
	HRESULT	Load_Shader(const wstring& strFileName, const EShaderType eType, const _char* pEntryPoint);

	// 로드된 셰이더가 있다면 그 값을 반환한다.
	const ComPtr<ID3DBlob> Find_ShaderByte(const EShaderType eType, const wstring& strName) const;
	template <EShaderType Type>
	ComPtr<ShaderType<Type>> Find_ShaderBuffer(const wstring& strName) const;
	

private:
	ID3DBlob* Read_ShaderBinary(const wstring& strFile);
	ID3DBlob* Complie_Shader(const wstring& strFile, const _char* pEntryPoint);

private:
	wstring									m_strMainPath;
	unordered_map<wstring, FShaderData*>	m_mapShaderData[ECast(EShaderType::Size)];
#pragma endregion


#pragma region 이펙트 셰이더 (구 FX 기반 셰이더, FX11)
public:
	// 이펙트 로드
	HRESULT Load_Effect(const wstring& strFileName, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const D3D_SHADER_MACRO* pShaderMacro = nullptr);
	HRESULT Recompile_EffectsAll();
	HRESULT Recompile_Effect(const wstring& strFileName, shared_ptr<FEffectData> pEffectData);
	// 이펙트 찾기
	ID3DX11Effect* Find_Effect(const wstring& strKey) const;
	// 이펙트 데이터를 찾기
	shared_ptr<FEffectData> Find_EffectData(const wstring& strKey, const D3D_SHADER_MACRO* pShaderMacro) const;

private:
	// 내부적으로 Wstring을 만들때 사용
	wstring Make_MacroToWstring(const D3D_SHADER_MACRO* pShaderMacro) const;


private:
	map<const wstring, shared_ptr<FEffectData>>	m_mapEffects;		// Effect11로 만들어진 셰이더 저장소.
#pragma endregion

};


#pragma region Get Shader 템플릿

template <EShaderType Type>
ComPtr<ShaderType<Type>> CShader_Manager::Find_ShaderBuffer(const wstring& strName) const
{
	constexpr _uint iIndex = ECast(Type);

	auto iter = m_mapShaderData[iIndex].find(strName);
	if (iter == m_mapShaderData[iIndex].end() || !(*iter).second->IsLoaded())
		return nullptr;

	ID3D11DeviceChild* pShaderBuffer = (*iter).second->CRef_ShaderBuffer().Get();

	return ComPtr<ShaderType<Type>>(Cast<ShaderType<Type>*>(pShaderBuffer));
}

#pragma endregion



END
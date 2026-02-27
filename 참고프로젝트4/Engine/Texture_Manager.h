#pragma once

#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CTexture_Data;

/// <summary>
/// 텍스처를 관리하는 매니저.
/// 텍스처는 처음 설정된 리소스의 경로를 기준으로,
/// 경로값으로 키를, 파일 이름에 확장자를 뺀 값으로 이름을 가짐.
/// 경로폴더 또한 추가적으로 값을 가진다.
/// 그로하여금 머터리얼에 들어가있는 텍스처 경로를 토대로 매니저에 접근하여 텍스처를 얻을 수 있더럭한다.
/// </summary>
class CTexture_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CTexture_Manager)

private:
	explicit CTexture_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTexture_Manager(const CTexture_Manager& rhs) = delete;
	virtual ~CTexture_Manager() = default;

public:
	HRESULT						Initialize(const wstring& strMainPath);

public:
	static CTexture_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strMainPath);

private:
	virtual void				Free();

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

public:
	// 텍스처가 존재하는지 확인
	HRESULT						IsExists_SRV(const wstring& strTextureKey);

	// 찾은 텍스처의 가장 앞 SRV를 받아오는 함수 (잘 안쓸거임)
	ID3D11ShaderResourceView*	Front_SRV(const wstring& strTextureKey) { return Find_SRVFromIndex(strTextureKey, 0); }

	// 찾은 텍스처의 특정 인덱스의 SRV를 받아오는 함수 (잘 안쓸거임)
	ID3D11ShaderResourceView*	Find_SRVFromIndex(const wstring& strTextureKey, const _uint iIndex);

	// 셰이더 리소스 배열을 저장된 텍스처 셋으로부터 받아오기
	HRESULT						Reference_SRVs(const wstring& strTextureKey, vector<ComPtr<ID3D11ShaderResourceView>>& RefSRVs);

	// 텍스처를 로드하기
	HRESULT						Ready_Texture(const wstring& strFilePath, const _uint iNumTextures, const _bool bPermanent, _bool bUseMainPath);

	// 특정 파일 안에 모든 텍스쳐를 로드 -> 스프라이트 텍스쳐 안됨
	HRESULT						Include_All_Textures(const wstring& strMainPath, _bool bSavePath, const _bool bPermanent);

	HRESULT Insert_TextureAsync(const wstring& strTextureTag, CTexture_Data* pTexture, 
		const wstring& strFilePath, _uint iNumTextures, _bool bIsPermanent, _bool bIsSavePath);

	// 텍스처의 레퍼런스 카운트가 0인 경우 제거하는 함수, true를 넣으면 영구적인 것과 관계없이 텍스처를 정리한다. [새로 추가, 검증 필요]
	void						Clear_Textures(_bool bIsAllClear);

public:
	const wstring&				Get_MainPath() const { return m_strMainPath; }
	const map<wstring, shared_ptr<CTexture_Data>>* Get_TexturesMap() const { return &m_TexturesMap; }

private:
	wstring										m_strMainPath = { L"" };	// 메인 패스 설정. 로드되는 텍스처의 키 값은 메인 패스를 제거한 값.
	map<wstring, shared_ptr<CTexture_Data>>		m_TexturesMap;				// 텍스처 정보 저장, 패스 이름이 키값. 이제 텍스처는 단일 정보만을 가짐. 여기서는 분류를 하지 않음.


public:
	// 비동기 대기
	void WaitAsync();
	// 비동기 뮤텍스 작동
	void GuardAsync();
	// 비동기 뮤텍스 종료
	void UnGuardAsync();

private:
	atomic<_bool>			m_bIsUsingAsync = { false };
	mutex					m_AsyncMutex;
	list<future<HRESULT>>	m_Futures;

};

END
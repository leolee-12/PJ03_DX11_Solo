#pragma once

//#include <System/Define/ModelMgr_Define.h>
#include "ModelContainer.h"
#include "MeshContainer.h"
#include "BoneContainer.h"
#include "BoneAnimContainer.h"
#include <filesystem>

BEGIN(Engine)

namespace fs = filesystem;

/// <summary>
/// 모델 관리자
/// 메쉬(버퍼 형태)
/// 머터리얼
/// 뼈
/// 애니메이션
/// 정보를 저장하고 관리하는 클래스
/// 
/// 머터리얼의 경우 텍스처 매니저와 연계되어 바로 로드하도록 설정된다.
/// 
/// 프로젝트의 확장으로 모델과 애니메이션을 별도로 로드한다.
/// 애니메이션을 로드 할 때는 aanim의 헤더의 조사를 통해 애니메이션 추가용인지 확인하여 진행하며, 확인되면 해당 애니메이션을 로드하여 별도의 애니메이션 공간에 저장한다.
/// 
/// </summary>
class ENGINE_DLL CModel_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CModel_Manager)

private:
	explicit CModel_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CModel_Manager(const CModel_Manager& rhs) = delete;
	virtual ~CModel_Manager() = default;

public:
	HRESULT				Initialize(const wstring& strMainDir);

public:
	static CModel_Manager*	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strMainDir);
private:
	virtual void		Free() override;


private:
	wstring	m_strMainDir = { TEXT("") };			// 참조할 메인 디렉터리
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };

public:
	HRESULT Load_Binary(const wstring& strFileName, class CModelBinary* pModel);

	/// <summary>
	/// 단일 바이너리 파일을 로드하는 함수
	/// </summary>
	/// <param name="strFileName">MainPath를 제외한 파일 경로</param>
	/// <returns></returns>
	HRESULT Load_Model(const wstring& strFileName,_bool bSavePath = false);

	/// <summary>
	/// 설정한 폴더의 하위 파일들을 모두 찾아 로드해주는 함수
	/// </summary>
	/// <param name="strFolderPath"> MainPath를 제외한 폴더 경로</param>
	/// <returns></returns>
	HRESULT Load_DirectorySub_Models(const wstring & strFolderPath,_bool bSavePath = false);
	HRESULT Load_DirectorySub_Models_Recursive(const fs::path& directory, list<wstring>& listModels, list<wstring>& listAnims);

	HRESULT Rebake_Model(const wstring& strFileName);

	HRESULT Rebake_MaterialsAll(const wstring& strFolderPath);
	HRESULT Rebake_MaterialsAll_Recursive(const fs::path& directory, list<wstring>& listModels);


	// 모델 데이터 저장
public:
	void Load_Meshes(FModelData* pModelData, class CModelBinary* pModel);
	void Load_Materials(FModelData* pModelData, class CModelBinary* pModel, const wstring& strPath);
	// FileName은 키값, 해쉬값과 비교한다.
	void Load_Animations(FModelData* pModelData, class CModelBinary* pModel, const wstring& strParentFolderName);
	void Load_Bones(FModelData* pModelData, class CModelBinary* pModel);


	// 모델 데이터 접근
public:
	const FModelData* const	Find_ModelData(const wstring & strModelKey);
	FModelData* Add_ModelData(const wstring & strModelKey, CModelBinary* pModelBinary);


	// 메쉬 데이터 접근
public:
	const FMeshData* const	Find_MeshData(const wstring& strModelKey, const wstring& strMeshKey, const _uint iRangeIndex);
	const FMeshData* const	Find_MeshData(const wstring& strModelKey, const _uint iIndex);
	

	// 뼈 데이터 접근
public:
	CBoneGroup*				Clone_BoneGroup(const wstring& strModelKey);


	// 머터리얼 데이터 접근
public:
	const FMaterialData* const Find_MaterialData(const wstring& strModelKey, const _uint iIndex);


	// 각 데이터 그룹 접근
public:
	CMeshGroup*				Find_MeshGroup(const wstring& strModelKey);
	CBoneGroup*				Find_BoneGroup(const wstring& strModelKey);
	CBoneAnimGroup*			Find_AnimGroup(const wstring& strModelKey);
	CMaterialGroup*			Find_MaterialGroup(const wstring& strModelKey);

private:
	string				m_strLoadFilePath;				// 내부용, 로드한 파일 경로.
	_int				m_iNodeID;						// 내부용, 노드 번호 부여용.
	_uint				m_iMaterialCount = 0U;
	_uint				m_iBoneCount = 0U;

public:
	const unordered_map<wstring, FModelData*>& Get_ModelDatas() const { return m_mapModelDatas; }

private:
	_unmap<wstring, FModelData*>& Lock_ModelDatas() 
	{ 
		GaurdAsync();
		return m_mapModelDatas; 
	}

private:
	// 모델 정보, 메쉬, 뼈, 머터리얼, 애니메이션
	unordered_map<wstring, FModelData*>	m_mapModelDatas;
	map<_uint_64, wstring> m_mapModelHashes;


public:
	// 비동기 대기
	void WaitAsync(_bool bAllPushed);
	// 비동기 뮤텍스 작동
	void GaurdAsync();
	// 비동기 뮤텍스 종료
	void UnGaurdAsync();
	void WaitAtomic();
	void ExitAtomic();

private:
	using SLoadModelFuture = future<HRESULT>;

	atomic<_bool>			m_bWaitingAsync = { false };
	atomic<_bool>			m_bIsUsingAsync = { false };	// 테스트로 만들어본 아토믹 기반 동기화. 잘됨.
	_uint					m_iThreadLimit = { 8 };
	mutex					m_AsyncMutex;	// 비동기 뮤텍스
	list<SLoadModelFuture>	m_Futures;		// 비동기 대기

};

END
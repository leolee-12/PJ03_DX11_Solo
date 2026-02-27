#include "Texture_Data.h"

#include <DirectXTex/DirectXTex.h>

CTexture_Data::CTexture_Data(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice), m_pDeviceContext(pContext)
{
}

CTexture_Data::CTexture_Data(const CTexture_Data& rhs)
{
}

HRESULT CTexture_Data::Initialize()
{
	return S_OK;
}

CTexture_Data* CTexture_Data::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	ThisClass* pInstance = new ThisClass(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Texture_Data Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture_Data::Free()
{
	int t = 0;
}

void CTexture_Data::Unload()
{
	for (auto& Texture : m_SRVs)
		Texture.Reset();
	m_SRVs.clear();
	m_iNumTextures = 0;
	m_bIsLoaded = false;
}

HRESULT CTexture_Data::Load(ID3D11ShaderResourceView* _pSRV)
{
	if (m_bIsLoaded || _pSRV == nullptr)
		RETURN_EFAIL;

	m_SRVs.push_back(_pSRV);

	return S_OK;
}

HRESULT CTexture_Data::Insert_Texture(const wstring& strFilePath, const _uint iNumTextures, const _bool bPermanent)
{
	// 파일 존재여부 확인, 특히 사이즈에 해당하는 카운트 값이 존재하는지 확인, 잘못된 경로가 있다면 오류 반환
	vector<wstring> vecStrFullPath;
	vector<wstring> vecStrName;
	vecStrFullPath.reserve(iNumTextures);
	vecStrName.reserve(iNumTextures);

	for (_uint i = 0; i < iNumTextures; i++)
	{
		/* ..\정의훈\139\Framework\Client\Bin\Resources\Textures\Default.jpg */
		_tchar		szFullPath[MAX_PATH] = TEXT("");

		wsprintf(szFullPath, strFilePath.c_str(), i);

		/* D:\ */
		_tchar		szDrive[MAX_PATH] = TEXT("");

		/* 정의훈\139\Framework\Client\Bin\Resources\Textures\ */
		_tchar		szDirectory[MAX_PATH] = TEXT("");

		/* Default_0 */
		_tchar		szFileName[MAX_PATH] = TEXT("");

		/* .jpg */
		_tchar		szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		ifstream file(szFullPath);
		if (file.fail())
		{
			file.close();
			return E_FAIL;
		}

		vecStrFullPath.push_back(szFullPath);
		vecStrName.push_back(szFileName);

		file.close();
	}

	//CoUninitialize();
	HRESULT hr = S_OK;
	/*hr = 
	if (FAILED(hr))
	{
		CoUninitialize();
		RETURN_EFAIL;
	}*/

	m_SRVs.reserve(iNumTextures);
	for (_uint i = 0; i < iNumTextures; i++)
	{
		// 텍스처 로드
		ScratchImage image;
		TexMetadata Metadata = {};

		size_t iSubstrPoint = vecStrFullPath[i].find_last_of(TEXT("."));
		wstring strEXT = vecStrFullPath[i].substr(iSubstrPoint);

		if (strEXT == TEXT(".dds"))
		{
			hr = LoadFromDDSFile((vecStrFullPath[i]).c_str(), DDS_FLAGS_NONE, &Metadata, image);
			if (FAILED(hr))
			{
				RETURN_EFAIL;
			}

			/*if ((image.GetImages()->height % 2 != 0) || (image.GetImages()->width % 2 != 0))
				continue;*/
		}
		else if (strEXT == TEXT(".hdr"))
		{
			hr = LoadFromHDRFile((vecStrFullPath[i]).c_str(), &Metadata, image);
			if (FAILED(hr))
			{
				RETURN_EFAIL;
			}
		}
		else if (strEXT == TEXT(".png") || strEXT == TEXT(".bmp"))
		{
 			hr = LoadFromWICFile((vecStrFullPath[i]).c_str(), WIC_FLAGS_NONE, &Metadata, image);
			if (FAILED(hr))
			{
				RETURN_EFAIL;
			}
		}
		else if (strEXT == TEXT(".tga"))
		{
			hr = LoadFromTGAFile((vecStrFullPath[i]).c_str(), &Metadata, image);
			if (FAILED(hr))
			{
				RETURN_EFAIL;
			}
		}
		else
		{
			return E_FAIL;
		}

		/*ComPtr<ID3D11Texture2D> pTexture = { nullptr };
		CreateTexture(m_pDevice.Get(), image.GetImages(), image.GetImageCount(),
			image.GetMetadata(), pTexture.ReleaseAndGetAddressOf());*/

		ComPtr<ID3D11ShaderResourceView> pSRV = { nullptr };
		hr = CreateShaderResourceView(m_pDevice.Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), pSRV.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			RETURN_EFAIL;
		}

		//SaveDDSTextureToFile(m_pDeviceContext.Get(), pSRV.Get(), vecStrFullPath[i].c_str());

		D3D11_SHADER_RESOURCE_VIEW_DESC Desc;
		pSRV->GetDesc(&Desc);

		// 로드된 리소스 할당
		Load(pSRV.Get());
		Set_Permanent(bPermanent);
	}

	m_iNumTextures = iNumTextures;

	//CoUninitialize();

	return S_OK;
}

ID3D11ShaderResourceView* const CTexture_Data::Get_SRV(const _uint iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumTextures)
		return nullptr;

	return m_SRVs[iIndex].Get();
}

HRESULT CTexture_Data::Reference_SRVs(vector<ComPtr<ID3D11ShaderResourceView>>& RefSRVs)
{
	if (m_iNumTextures == 0)
		RETURN_EFAIL;

	RefSRVs.reserve(m_iNumTextures);
	RefSRVs = m_SRVs;

	return S_OK;
}

#pragma once
#include "Base.h"
#include "Path_Mgr.h"

BEGIN(Engine)

class ENGINE_DLL CUtility_File final : public CBase
{
public:
	enum DATA_TYPE {DATA,MODEL,TEXTURE,SHADER,TYPE_END};

public:
	static wstring PathFinder(const wstring& wstrMainPath, const wstring& wstrFileName,_bool bMultiple = false,
		_bool bSavePath = false);
	static wstring Get_FilePath(CPath_Mgr::FILE_TYPE eType, const wstring& wstrFileName);
	static void		Add_FilePath(CPath_Mgr::FILE_TYPE eType, const wstring& wstrFileName, const wstring& wstrFilePath);
	static void		Path_Mgr_Destroy();
	// 확장자 뺀 값 얻는 함수
	static wstring ConvertToFileName(const wstring& wstrFileName);

	static HRESULT	  All_FilePath_Save(const wstring& wstrMainPath);

	/*template<class T>
	static HRESULT	All_Create_Prototype_Data(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath);*/

	static string Get_FileName(string strFilePath); // 파일 경로에서 파일이름만 리턴

public:
	static _bool		Is_Number(const string& strFileName);
	static string		Remove_Number(const string& strFileName);

public:
	static map<wstring, wstring>	CRef_DataPathes() { return CPath_Mgr::GetInstance()->CRef_DataPathes(); }

};

END

//template<class T>
//static HRESULT	All_Create_Prototype_Data(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath)
//{
//	if (!filesystem::exists(wstrMainPath))
//		RETURN_EFAIL;
//
//	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator(wstrMainPath))
//	{
//		string strFIlePath = WstrToStr(entry.path());
//		string strTmp;
//		_bool	bNumCheck = false;
//		CPath_Mgr::FILE_TYPE eType = CPath_Mgr::FILE_TYPE::FILE_TYPE_END;
//
//		_char	szDdrive[MAX_PATH] = "";
//		_char	szDirectory[MAX_PATH] = "";
//		_char	szFileName[MAX_PATH] = "";
//		_char	szExc[MAX_PATH] = "";
//
//		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
//			szFileName, MAX_PATH, szExc, MAX_PATH);
//
//		strTmp = szExc;
//
//		if (!strcmp(strTmp.c_str(), ""))
//			continue;
//
//		if ((!strcmp(strTmp.c_str(), ".amodel")) || (!strcmp(strTmp.c_str(), ".groupmodel")))
//		{
//			eType = CPath_Mgr::FILE_TYPE::MODEL_FILE;
//		}
//		else if (!strcmp(strTmp.c_str(), ".aanim"))
//		{
//			eType = CPath_Mgr::FILE_TYPE::ANIM_FILE;
//		}
//		else if ((!strcmp(strTmp.c_str(), ".dds")) || (!strcmp(strTmp.c_str(), ".png")))
//		{
//			eType = CPath_Mgr::FILE_TYPE::TEXTURE_FILE;
//		}
//		else if (!strcmp(strTmp.c_str(), ".json"))
//		{
//			eType = CPath_Mgr::FILE_TYPE::DATA_FILE;
//		}
//		else if ((!strcmp(strTmp.c_str(), ".mp3")) || (!strcmp(strTmp.c_str(), ".wav")) ||
//			(!strcmp(strTmp.c_str(), ".ogg")))
//		{
//			eType = CPath_Mgr::FILE_TYPE::SOUND_FILE;
//		}
//		else
//			continue;
//		// 파일 확장자에 따른 분류
//
//		strTmp = szFileName;
//
//		Add_FilePath(eType, StrToWstr(strTmp), StrToWstr(strFIlePath));
//
//		CPath_Mgr::GetInstance()->Create_Prototype_Data<T>(pDevice, pContext, StrToWstr(strTmp));
//	}
//
//	return S_OK;
//}
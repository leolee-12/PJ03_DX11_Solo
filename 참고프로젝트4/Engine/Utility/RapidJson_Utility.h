#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

using namespace rapidjson;

/// <summary>
/// RapidJson이 사용된 동적 파싱 클래스
/// 기존 버전과 다르게 다양한 데이터 형식을 표현할 수 있도록 제작되었다.
/// 
/// 외부 데이터 저장 및 로드 기능
/// 1단 한정 멤버찾기 기능
/// 멤버에 Object 넣기 기능
/// 배열에 Object 넣기 기능
/// 읽기 기능
/// 
/// [사용처]
/// 빠른 오브젝트 구조 조합과 배열을 추가 및
/// RapidJson의 빠른 속도를 활용하여 속도가 필요한 부분.
/// 검색을 활용하여 JSON의 구조적으로 접근해야하는 부분이 필요할 때 FRapidJson을 사용.
/// </summary>
class ENGINE_DLL FRapidJson
{
public:
	FRapidJson();
	FRapidJson(const FRapidJson& rhs);
	~FRapidJson() {}

public:
	static FRapidJson Open_Json(const wstring& strPath);

public:
	HRESULT Output_Json(const wstring& strPath);
	HRESULT Input_Json(const wstring& strPath);
	void Print_Json();

public:
	// 멤버에 단순 데이터 추가, String
	HRESULT Write_StringData(const string& strMemberName, const string& strValue);
	// 멤버에 단순 데이터 추가, Wstring
	HRESULT Write_StringData(const string& strMemberName, const wstring& strValue);
	// 멤버에 단순 데이터 추가, Bool
	HRESULT Write_Data(const string& strMemberName, const _bool bValue);
	// 멤버에 단순 데이터 추가, Int
	HRESULT Write_Data(const string& strMemberName, const _int iValue);
	// 멤버에 단순 데이터 추가, Int
	HRESULT Write_Data(const string& strMemberName, const _uint iValue);
	// 멤버에 단순 데이터 추가, Float
	HRESULT Write_Data(const string& strMemberName, const _float fValue);
	// 멤버에 단순 데이터 추가, Float3
	HRESULT Write_Data(const string& strMemberName, const _float3 fValue);
	// 멤버에 단순 데이터 추가, Float4
	HRESULT Write_Data(const string& strMemberName, const _float4 fValue);
	// 멤버에 오브젝트 데이터 추가
	HRESULT Write_ObjectData(const string& strMemberName, FRapidJson& SerialData);
	// 배열에 다른 Document 추가
	HRESULT Pushback_ObjectToArray(const string& strMemberName, FRapidJson& SerialData);
	// 배열에 String 추가
	HRESULT Pushback_StringToArray(const string& strMemberName, const string& strValue);
	// 배열에 String 추가
	HRESULT Pushback_StringToArray(const string& strMemberName, const wstring& strValue)
	{ return Pushback_StringToArray(strMemberName, ConvertToString(strValue)); }

public:
	HRESULT Read_Data(const string& strMemberName, _bool& tValue) const;
	HRESULT Read_Data(const string& strMemberName, string& tValue) const;
	HRESULT Read_Data(const string& strMemberName, wstring& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _int& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _uint& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _float& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _float3& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _float4& tValue) const;
	HRESULT Read_ObjectData(const string& strMemberName, FRapidJson& Data);
	HRESULT Read_ObjectFromArray(const string& strMemberName, _uint iIndex, FRapidJson& DstData);
	HRESULT Read_DataFromArray(const string& strMemberName, _uint iIndex, string& strValue);
	HRESULT Read_DataFromArray(const string& strMemberName, _uint iIndex, wstring& strValue);

public:
	_uint Read_ArraySize(const string& strMemberName);
	
public:
	Document	m_Doc;

};

END
#include "Utility/RapidJson_Utility.h"

#include "rapidjson/istreamwrapper.h"

FRapidJson::FRapidJson()
{
	m_Doc.Parse("{}");
}

FRapidJson::FRapidJson(const FRapidJson& rhs)
{
	m_Doc.CopyFrom(rhs.m_Doc, m_Doc.GetAllocator());
}

FRapidJson FRapidJson::Open_Json(const wstring& strPath)
{
	FRapidJson Data;

	Data.Input_Json(strPath);

	return Data;
}

HRESULT FRapidJson::Output_Json(const wstring& strPath)
{
	ofstream ofs(strPath);

	if (ofs.is_open())
	{
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		m_Doc.Accept(writer);

		ofs << buffer.GetString() << endl;

		ofs.close();

		cout << "JSON 저장 성공" << endl;
	}
	else
	{
		cout << "파일 열기 실패" << endl;

		ofs.close();
		return E_FAIL;;
	}

	return S_OK;
}

HRESULT FRapidJson::Input_Json(const wstring& strPath)
{
	ifstream ifs(strPath);

	if (ifs.is_open())
	{
		IStreamWrapper isw(ifs);

		m_Doc.ParseStream(isw);
		
		ifs.close();

		if (m_Doc.HasParseError())
		{
			cout << "JSON 파일 파싱 오류" << endl;
			return E_FAIL;;
		}

		//cout << "JSON 열기 성공" << endl;
	}
	else
	{
		//cout << "파일 열기 실패" << endl;
		return E_FAIL;;
	}

	return S_OK;
}

void FRapidJson::Print_Json()
{
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	m_Doc.Accept(writer);

	cout << buffer.GetString() << endl;
}

HRESULT FRapidJson::Write_StringData(const string& strMemberName, const string& strValue)
{
	const _char* pMemberName = strMemberName.c_str();
	const _char* pStrValue = strValue.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value KeyValue(pMemberName, m_Doc.GetAllocator());
		Value StringValue(pStrValue, m_Doc.GetAllocator());
		m_Doc.AddMember(KeyValue, StringValue, m_Doc.GetAllocator());

		return S_OK;
	}

	Value& DataValue = iter->value;
	DataValue.SetString(pStrValue, m_Doc.GetAllocator());

	return S_OK;
}

HRESULT FRapidJson::Write_StringData(const string& strMemberName, const wstring& strValue)
{
	const _char* pMemberName = strMemberName.c_str();
	string strConvert = ConvertToString(strValue);
	const _char* pStrValue = strConvert.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value KeyValue(pMemberName, m_Doc.GetAllocator());
		Value StringValue(pStrValue, m_Doc.GetAllocator());
		m_Doc.AddMember(KeyValue, StringValue, m_Doc.GetAllocator());

		return S_OK;
	}

	Value& DataValue = iter->value;
	DataValue.SetString(pStrValue, m_Doc.GetAllocator());

	return S_OK;
}

HRESULT FRapidJson::Write_Data(const string& strMemberName, const _bool bValue)
{
	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value KeyValue(pMemberName, m_Doc.GetAllocator());
		m_Doc.AddMember(KeyValue, bValue, m_Doc.GetAllocator());

		return S_OK;
	}

	Value& DataValue = iter->value;
	DataValue.SetBool(bValue);

	return S_OK;
}

HRESULT FRapidJson::Write_Data(const string& strMemberName, const _int iValue)
{
	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value KeyValue(pMemberName, m_Doc.GetAllocator());
		m_Doc.AddMember(KeyValue, iValue, m_Doc.GetAllocator());

		return S_OK;
	}

	Value& DataValue = iter->value;
	DataValue.SetInt(iValue);

	return S_OK;
}

HRESULT FRapidJson::Write_Data(const string& strMemberName, const _uint iValue)
{
	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value KeyValue(pMemberName, m_Doc.GetAllocator());
		m_Doc.AddMember(KeyValue, iValue, m_Doc.GetAllocator());

		return S_OK;
	}

	Value& DataValue = iter->value;
	DataValue.SetUint(iValue);

	return S_OK;
}

HRESULT FRapidJson::Write_Data(const string& strMemberName, const _float fValue)
{
	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value KeyValue(pMemberName, m_Doc.GetAllocator());
		m_Doc.AddMember(KeyValue, fValue, m_Doc.GetAllocator());

		return S_OK;
	}

	Value& DataValue = iter->value;
	DataValue.SetFloat(fValue);

	return S_OK;
}

HRESULT FRapidJson::Write_Data(const string& strMemberName, const _float3 fValue)
{
	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value Key(pMemberName, m_Doc.GetAllocator());
		Value NewArray(kArrayType);
		m_Doc.AddMember(Key, NewArray, m_Doc.GetAllocator());
	}

	// Array Value 참조 및 배열 체크, 배열에 다른 시리얼 데이터 정보 추가
	iter = m_Doc.FindMember(pMemberName);
	Value& arrayValue = iter->value;
	if (arrayValue.IsArray())
	{
		Value ValueX, ValueY, ValueZ;
		ValueX.SetFloat(fValue.x);
		ValueY.SetFloat(fValue.y);
		ValueZ.SetFloat(fValue.z);
		arrayValue.PushBack(ValueX, m_Doc.GetAllocator());
		arrayValue.PushBack(ValueY, m_Doc.GetAllocator());
		arrayValue.PushBack(ValueZ, m_Doc.GetAllocator());
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Write_Data(const string& strMemberName, const _float4 fValue)
{
	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value Key(pMemberName, m_Doc.GetAllocator());
		Value NewArray(kArrayType);
		m_Doc.AddMember(Key, NewArray, m_Doc.GetAllocator());
	}

	// Array Value 참조 및 배열 체크, 배열에 다른 시리얼 데이터 정보 추가
	iter = m_Doc.FindMember(pMemberName);
	Value& arrayValue = iter->value;
	if (arrayValue.IsArray())
	{
		Value ValueX, ValueY, ValueZ, ValueW;
		ValueX.SetFloat(fValue.x);
		ValueY.SetFloat(fValue.y);
		ValueZ.SetFloat(fValue.z);
		ValueW.SetFloat(fValue.w);
		arrayValue.PushBack(ValueX, m_Doc.GetAllocator());
		arrayValue.PushBack(ValueY, m_Doc.GetAllocator());
		arrayValue.PushBack(ValueZ, m_Doc.GetAllocator());
		arrayValue.PushBack(ValueW, m_Doc.GetAllocator());
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Write_ObjectData(const string& strMemberName, FRapidJson& SerialData)
{
	// 추가하려는 Doc이 비어있으면 실패
	if (SerialData.m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
	{
		Value KeyValue(pMemberName, m_Doc.GetAllocator());
		Value CopyValue(SerialData.m_Doc, m_Doc.GetAllocator());
		m_Doc.AddMember(KeyValue, CopyValue, m_Doc.GetAllocator());

		return S_OK;
	}

	Value& DataValue = iter->value;
	Value CopyValue(SerialData.m_Doc, m_Doc.GetAllocator());
	DataValue.CopyFrom(CopyValue, m_Doc.GetAllocator());

	return S_OK;
}

HRESULT FRapidJson::Pushback_ObjectToArray(const string& strMemberName, FRapidJson& SerialData)
{
	// 추가하려는 Doc이 비어있으면 실패
	if (SerialData.m_Doc.HasParseError())
		return E_FAIL;;

	// 찾는 멤버가 있는지 확인, Array멤버 찾기, 없을시
	const _char* pArrayMember = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pArrayMember);
	if (iter == m_Doc.MemberEnd())
	{
		Value Key(pArrayMember, m_Doc.GetAllocator());
		Value NewArray(kArrayType);
		m_Doc.AddMember(Key, NewArray, m_Doc.GetAllocator());
	}

	// Array Value 참조 및 배열 체크, 배열에 다른 시리얼 데이터 정보 추가
	iter = m_Doc.FindMember(pArrayMember);
	Value& arrayValue = iter->value;
	if (arrayValue.IsArray())
	{
		Value CopiedValue(SerialData.m_Doc, m_Doc.GetAllocator());
		arrayValue.PushBack(CopiedValue, m_Doc.GetAllocator());
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Pushback_StringToArray(const string& strMemberName, const string& strValue)
{
	// 찾는 멤버가 있는지 확인, Array멤버 찾기, 없을시
	const _char* pArrayMember = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pArrayMember);
	if (iter == m_Doc.MemberEnd())
	{
		Value Key(pArrayMember, m_Doc.GetAllocator());
		Value NewArray(kArrayType);
		m_Doc.AddMember(Key, NewArray, m_Doc.GetAllocator());
	}

	// Array Value 참조 및 배열 체크, 배열에 다른 시리얼 데이터 정보 추가
	iter = m_Doc.FindMember(pArrayMember);
	Value& arrayValue = iter->value;
	if (arrayValue.IsArray())
	{
		Value stringValue(strValue.c_str(), m_Doc.GetAllocator());
		arrayValue.PushBack(stringValue, m_Doc.GetAllocator());
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, _bool& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsBool())
	{
		tValue = iter->value.GetBool();
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, string& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsString())
	{
		tValue = iter->value.GetString();
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, wstring& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsString())
	{
		tValue = ConvertToWstring(iter->value.GetString());
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, _int& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsInt())
	{
		tValue = iter->value.GetInt();
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, _uint& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsInt() || ValueData.IsUint())
	{
		tValue = iter->value.GetUint();
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, _float& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsFloat())
	{
		tValue = iter->value.GetFloat();
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, _float3& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsArray())
	{
		if (3 != Cast<_uint>(ValueData.GetArray().Size()))
			return E_FAIL;;

		for (_uint i = 0; i < 3; i++)
		{
			const Value& FloatData = ValueData.GetArray()[i];
			if (FloatData.IsFloat())
			{
				if (i == 0)
					tValue.x = FloatData.GetFloat();
				else if (i == 1)
					tValue.y = FloatData.GetFloat();
				else if (i == 2)
					tValue.z = FloatData.GetFloat();
			}
			else
				return E_FAIL;;
		}
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_Data(const string& strMemberName, _float4& tValue) const
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::ConstMemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	const Value& ValueData = iter->value;
	if (ValueData.IsArray())
	{
		if (4 != Cast<_uint>(ValueData.GetArray().Size()))
			return E_FAIL;;

		for (_uint i = 0; i < 4; i++)
		{
			const Value& FloatData = ValueData.GetArray()[i];
			if (FloatData.IsFloat())
			{
				if (i == 0)
					tValue.x = FloatData.GetFloat();
				else if (i == 1)
					tValue.y = FloatData.GetFloat();
				else if (i == 2)
					tValue.z = FloatData.GetFloat();
				else if (i == 3)
					tValue.w = FloatData.GetFloat();
			}
			else
				return E_FAIL;;
		}
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_ObjectData(const string& strMemberName, FRapidJson& Data)
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	Value& ValueData = iter->value;
	if (ValueData.IsObject())
	{
		const Value CopyValue(ValueData, m_Doc.GetAllocator());
		Data.m_Doc.CopyFrom(CopyValue, Data.m_Doc.GetAllocator());
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_ObjectFromArray(const string& strMemberName, _uint iIndex, FRapidJson& DstData)
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	Value& ValueData = iter->value;
	if (ValueData.IsArray())
	{
		if (iIndex < 0 || iIndex >= Cast<_uint>(ValueData.GetArray().Size()))
			return E_FAIL;;

		Value& ObjectData = ValueData.GetArray()[iIndex];
		DstData.m_Doc.CopyFrom(ObjectData, DstData.m_Doc.GetAllocator());
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_DataFromArray(const string& strMemberName, _uint iIndex, string& strValue)
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	Value& ValueData = iter->value;
	if (ValueData.IsArray())
	{
		if (iIndex < 0 || iIndex >= Cast<_uint>(ValueData.GetArray().Size()))
			return E_FAIL;;

		Value& StringData = ValueData.GetArray()[iIndex];
		strValue = StringData.GetString();
	}
	else
		return E_FAIL;;

	return S_OK;
}

HRESULT FRapidJson::Read_DataFromArray(const string& strMemberName, _uint iIndex, wstring& strValue)
{
	if (m_Doc.HasParseError())
		return E_FAIL;;

	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return E_FAIL;;

	Value& ValueData = iter->value;
	if (ValueData.IsArray())
	{
		if (iIndex < 0 || iIndex >= Cast<_uint>(ValueData.GetArray().Size()))
			return E_FAIL;;

		Value& StringData = ValueData.GetArray()[iIndex];
		strValue = ConvertToWstring(StringData.GetString());
	}
	else
		return E_FAIL;;

	return S_OK;
}

_uint FRapidJson::Read_ArraySize(const string& strMemberName)
{
	if (m_Doc.HasParseError())
		return 0;

	const _char* pMemberName = strMemberName.c_str();
	Value::MemberIterator iter = m_Doc.FindMember(pMemberName);
	if (iter == m_Doc.MemberEnd())
		return 0;

	Value& ValueData = iter->value;
	if (ValueData.IsArray())
	{
		return Cast<_uint>(ValueData.GetArray().Size());
	}

	return 0;
}



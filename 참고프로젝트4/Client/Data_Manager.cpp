#include "stdafx.h"
#include "Data_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Model_Inst.h"
#include "Instance_Object.h"
#include "UI_Manager.h"
#include "InteractionBox.h"

IMPLEMENT_SINGLETON(CData_Manager)

CData_Manager::CData_Manager()
{
	m_pGameInstance = CGameInstance::GetInstance();
}

HRESULT CData_Manager::Initialize()
{
	return S_OK;
}

void CData_Manager::Tick(_cref_time fTimeDelta)
{
}


HRESULT CData_Manager::Save_ObjectData(string strPath, vector<shared_ptr<CGameObject>>* pObjectList, LAYERTYPE eType)
{
	json Out_Json;

	time_t timer = time(NULL);
	tm     TimeDesc;

	localtime_s(&TimeDesc, &timer);

	string szDayInfo
		= to_string(TimeDesc.tm_mon) + "."
		+ to_string(TimeDesc.tm_wday) + " ("
		+ to_string(TimeDesc.tm_hour) + "-"
		+ to_string(TimeDesc.tm_min) + "-"
		+ to_string(TimeDesc.tm_sec) + ").json";

	// 레이어 세이브

	for (_uint i = 0; i < (_uint)LAYERTYPE_END; i++)
	{
		//#임시
		if (i != (_uint)eType)
			continue;

		_uint iIndex = 0;
		for (auto& iter : *pObjectList)
		{

			iter->Write_Json(Out_Json["GameObject"][iIndex]);

			++iIndex;
		}
	}
	//string("../Bin/Data/Test/") + strPath + ".json";// +szDayInfo; //+ to_string(iter.first) + " "
	string szFilePath = strPath;

	CJson_Utility::Save_Json(szFilePath.c_str(), Out_Json);
	//#해시태그
	Out_Json.clear();

	// 자동 세이브
	Write_Json(Out_Json);

	if (!Out_Json.empty())
		CJson_Utility::Save_Json(string(string("../Bin/Data/AutoSave/ ") + szDayInfo).c_str(), Out_Json);

	Out_Json.clear();

	return S_OK;
}

//HRESULT CData_Manager::Save_ObjectData(string strPath, shared_ptr<CGameObject> pObject)
//{
//	json Out_Json;
//
//	//Out_Json["GameObject"][iIndex]["Component"]["Transform"].emplace();
//
//	pObject->Write_Json(Out_Json["GameObject"]);
//
//	//string("../Bin/Data/Test/") + strPath + ".json";// +szDayInfo; //+ to_string(iter.first) + " "
//	string szFilePath = strPath;
//	CJson_Utility::Save_Json(szFilePath.c_str(), Out_Json);
//	//#해시태그
//	Out_Json.clear();
//
//	// 자동 세이브
//	Write_Json(Out_Json);
//
//	Out_Json.clear();
//
//	return S_OK;
//}

HRESULT CData_Manager::Save_ObjectData(string strPath, weak_ptr<CGameObject> pObject)
{
	json Out_Json;

	//Out_Json["GameObject"][iIndex]["Component"]["Transform"].emplace();

	pObject.lock()->Write_Json(Out_Json["GameObject"]);

	//string("../Bin/Data/Test/") + strPath + ".json";// +szDayInfo; //+ to_string(iter.first) + " "
	string szFilePath = strPath;
	CJson_Utility::Save_Json(szFilePath.c_str(), Out_Json);
	//#해시태그
	Out_Json.clear();

	// 자동 세이브
	Write_Json(Out_Json);

	Out_Json.clear();

	return S_OK;
}

HRESULT CData_Manager::Load_ObjectData(string strPath, LEVEL eLevel, LAYERTYPE eType)
{
	json In_Json;
	CJson_Utility::Load_Json(strPath.c_str(), In_Json);

	_uint i = 0;
	for (auto& iter : In_Json["GameObject"])
	{
		wstring strProtoTag = StrToWstr(iter["PrototypeTag"]);
		//LAYERTYPE eLayerType = (LAYERTYPE)(iter["LayerType"]);
		shared_ptr<CGameObject> pGameObject;
		if (iter.contains("ModelTag"))
		{
			wstring strObjectTag = StrToWstr(iter["ModelTag"]);

			if (eLevel != LEVEL_TOOL)
			{
				if (strObjectTag == L"Box3" || strObjectTag == L"Box4")
				{
					strProtoTag = L"Prototype_GameObject_InteractionBox";
				}
			}

			if (eType == L_MONSTER && (eLevel == LEVEL_STAGE1 || eLevel == LEVEL_STAGE2 || eLevel == LEVEL_STAGE3))
			{
				if (strObjectTag == L"EN0000_00_SecuritySoldier")
					strProtoTag = L"Prototype_GameObject_Security_Soldier";

				else if (strObjectTag == L"EN2000_00_GuardHound")
					strProtoTag = L"Prototype_GameObject_GuardHound";

				else if (strObjectTag == L"EN1000_00_1stray")
					strProtoTag = L"Prototype_GameObject_Stray";

				else if (strObjectTag == L"MonoEN3000_00_MonoDrive")
					strProtoTag = L"Prototype_GameObject_MonoDrive";

				else if (strObjectTag == L"EN1001_00_Sweeper")
					strProtoTag = L"Prototype_GameObject_Sweeper";
			}
			else if (eType == L_NPC)
			{
				if (strObjectTag == L"Biggs")
					strProtoTag = L"Prototype_GameObject_Biggs";
				else if (strObjectTag == L"ChocoboDriver")
					strProtoTag = L"Prototype_GameObject_ChocoboDriver";
				else if (strObjectTag == L"DanceManager")
					strProtoTag = L"Prototype_GameObject_DanceManager";
				else if (strObjectTag == L"GymMemeber")
					strProtoTag = L"Prototype_GameObject_GymMember";
				else if (strObjectTag == L"ItemShop")
					strProtoTag = L"Prototype_GameObject_ItemShop";
				else if (strObjectTag == L"StationStaff")
					strProtoTag = L"Prototype_GameObject_StationStaff";
				else if (strObjectTag == L"WeaponShop")
					strProtoTag = L"Prototype_GameObject_WeaponShop";
				else if (strObjectTag == L"Wedge")
					strProtoTag = L"Prototype_GameObject_Wedge";
				else if (strObjectTag.find(L"MM") != wstring::npos)
					strProtoTag = L"Prototype_GameObject_Man";
				else if (strObjectTag.find(L"MW") != wstring::npos)
					strProtoTag = L"Prototype_GameObject_Woman";
			}
			else if (eType == L_MOB)
			{
				if (strObjectTag.find(L"Cat") != wstring::npos)
					strProtoTag = L"Prototype_GameObject_Cat";
				else if (strObjectTag.find(L"Dog") != wstring::npos)
					strProtoTag = L"Prototype_GameObject_Dog";
				else if (strObjectTag.find(L"Rat") != wstring::npos)
					strProtoTag = L"Prototype_GameObject_Rat";
				else if (strObjectTag.find(L"Pigeon") != wstring::npos)
					strProtoTag = L"Prototype_GameObject_Pigeon";
				else if (strObjectTag.find(L"Chocobo") != wstring::npos)
					strProtoTag = L"Prototype_GameObject_Chocobo";

			}
		}

		m_pGameInstance->Add_CloneObject(eLevel, eType, strProtoTag, nullptr, &pGameObject);

		pGameObject->Load_Json(iter);
		pGameObject.reset();
		++i;
	}
	i;

	return S_OK;
}

HRESULT CData_Manager::Load_LaserData(string strPath, vector<_float4>* vecPos)
{
	json In_Json;
	CJson_Utility::Load_Json(strPath.c_str(), In_Json);

	_uint i = 0;
	for (auto& iter : In_Json["GameObject"])
	{
		_float4 vPos;

		vPos.x = iter["Component"]["Transform"][3][0];
		vPos.y = iter["Component"]["Transform"][3][1];
		vPos.z = iter["Component"]["Transform"][3][2];
		vPos.w = iter["Component"]["Transform"][3][3];
		
		vecPos->push_back(vPos);

	}
	return S_OK;
}

_bool CData_Manager::Load_BinaryData(string strPath, MODEL_DATA& pModelData)
{
	ifstream fin;

	fin.open(strPath, std::ios::binary);

	if (fin.is_open())
	{
		pModelData.Load_Binary(fin);
	}
	else
		return false;

	fin.close();
	return true;
}



CData_Manager::DEBUGDESC CData_Manager::Load_DebugJson()
{
	//json Out_Json;

	//_float4x4 matWorld = XMMatrixIdentity();
	//_float3 vPos = { 0.f,0.f,0.f };
	//Out_Json.clear();
	//Out_Json["Int"] = 1;
	//Out_Json["Float"] = 2;
	//Out_Json["Bool"] = true;
	//Out_Json["String"] = "string Test";
	//Out_Json["WString"] = WstrToStr(L"wstring Test");
	//Out_Json["Float3"] = { vPos.x,vPos.y,vPos.z };
	//Out_Json["WorldMatrix"] = matWorld.m;
	//CJson_Utility::Save_Json("../Bin/Data/TestDebug.json", Out_Json);

	DEBUGDESC tDesc;

	json In_Json;

	CJson_Utility::Load_Json("../Bin/Data/TestDebug.json", In_Json);

	if (In_Json.contains("Int"))
	{
		tDesc.iInt = In_Json["Int"];
	}
	if (In_Json.contains("Float"))
	{
		tDesc.fFloat = In_Json["Float"];
	}
	if (In_Json.contains("Float3"))
	{
		auto Float3 = In_Json["Float3"];
		tDesc.vFloat3.x = Float3[0];
		tDesc.vFloat3.y = Float3[1];
		tDesc.vFloat3.z = Float3[2];
	}
	if (In_Json.contains("Float3_2"))
	{
		auto Float3_2 = In_Json["Float3_2"];
		tDesc.vFloat3_2.x = Float3_2[0];
		tDesc.vFloat3_2.y = Float3_2[1];
		tDesc.vFloat3_2.z = Float3_2[2];
	}
	if (In_Json.contains("Bool"))
	{
		tDesc.bBool = In_Json["Bool"];
	}
	if (In_Json.contains("String"))
	{
		tDesc.strString = In_Json["String"];
	}
	if (In_Json.contains("WString"))
	{
		tDesc.wstrString = StrToWstr(In_Json["WString"]);
	}

	if (In_Json.contains("WorldMatrix"))
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				tDesc.matWorld.m[i][j] = In_Json["WorldMatrix"][i][j];
			}
		}
	}

	return tDesc;
}

HRESULT CData_Manager::Save_InstanceData(string strPath, vector<shared_ptr<CGameObject>> pObjectList)
{
	ofstream fout;
	fout.open(strPath, std::ios::binary);

	if (fout.is_open())
	{
		_uint iNumObjects = pObjectList.size();
		fout.write(reinterpret_cast<char*>(&iNumObjects), sizeof(_uint));
		for (auto& pObject : pObjectList)
		{
			string strModelTag = WstrToStr(dynamic_pointer_cast<CInstance_Object>(pObject)->Get_ModelTag());

			if (strModelTag.empty())
				continue;

			_uint istrSize = strModelTag.size();
			fout.write(reinterpret_cast<char*>(&istrSize), sizeof(_uint));
			fout.write(strModelTag.c_str(), istrSize);

			auto iter = pObject->Get_Componentss()->find(L"Com_Inst");
			vector<float4x4> matWorld = dynamic_pointer_cast<CModel_Inst>(iter->second)->Get_InstanceeVertex();
			_uint iMatSize = matWorld.size();
			fout.write(reinterpret_cast<char*>(&iMatSize), sizeof(_uint));

			for (auto& iter : matWorld)
			{
				fout.write(reinterpret_cast<char*>(&iter), sizeof(float4x4));
			}
		}

	}
	else
		RETURN_EFAIL;

	fout.close();

	return S_OK;
}

HRESULT CData_Manager::Load_InstanceData(string strPath, LEVEL eLevel)
{
	ifstream fin;
	fin.open(strPath, ios::binary);

	if (!fin.is_open())
	{
		RETURN_EFAIL;
	}

	_uint iNumObject;
	fin.read(reinterpret_cast<_char*>(&iNumObject), sizeof(_uint));

	for (_uint i = 0; i < iNumObject; ++i)
	{
		_uint iStrSize;
		fin.read(reinterpret_cast<_char*>(&iStrSize), sizeof(_uint));

		string strModelTag(iStrSize, '\0');
		fin.read(&strModelTag[0], iStrSize);

		_uint iMatSize;
		fin.read(reinterpret_cast<_char*>(&iMatSize), sizeof(_uint));

		vector<float4x4> matWorld(iMatSize);
		for (_uint i = 0; i < iMatSize; ++i)
		{
			fin.read(reinterpret_cast<_char*>(&matWorld[i]), sizeof(float4x4));
		}

		CInstance_Object::INSTANCE_OBJECT_DESC tDesc;
		tDesc.strName = strModelTag;
		tDesc.vecInstanceVertex = matWorld;

		if (FAILED(m_pGameInstance->Add_CloneObject(eLevel, L_INSTANCE,
			TEXT("Prototype_GameObject_Instance_Object"), &tDesc)))
		{
			fin.close();
			RETURN_EFAIL;
		}
	}


	fin.close();

	return S_OK;
}


void CData_Manager::Free()
{
}

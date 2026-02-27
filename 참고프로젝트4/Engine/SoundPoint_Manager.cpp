#include "SoundPoint_Manager.h"
#include "SoundPoint.h"

#include "GameInstance.h"

CSoundPoint_Manager::CSoundPoint_Manager()
{
}

HRESULT CSoundPoint_Manager::Initialize()
{

	return S_OK;
}

void CSoundPoint_Manager::Tick(_cref_time fTimeDelta)
{
	//for each 문은 중간에 삭제가 일어나면 터짐

	for (auto iter = m_SoundPoints.begin(); iter != m_SoundPoints.end(); )
	{
		if ((*iter)->Get_Dead())
		{
			(*iter).reset();
			iter = m_SoundPoints.erase(iter);
		}
		else {
			(*iter)->Tick(fTimeDelta);
			iter++;
		}
	}
}

HRESULT CSoundPoint_Manager::Render()
{
	for (auto& pSoundPoint : m_SoundPoints)
	{
		pSoundPoint->Render();
	}

	return S_OK;
}

HRESULT CSoundPoint_Manager::Add_SoundPoint(const string& strName, const TSoundPointDesc& Desc, shared_ptr<CSoundPoint>* ppOut)
{
	shared_ptr<CSoundPoint> pSoundPoint = CSoundPoint::Create(strName, Desc);
	if (nullptr == pSoundPoint)
		RETURN_EFAIL;

	shared_ptr<CSoundPoint> pFindSoundPoint = Find_SoundPoint(pSoundPoint->Get_Name());

	if (pFindSoundPoint != nullptr)
	{
		pFindSoundPoint->Set_SoundPointDesc(Desc);
		Release_SoundPoint(pSoundPoint);
	}
	else
	{
		m_SoundPoints.push_back(pSoundPoint);
	}
	if (ppOut != nullptr)
		*ppOut = pSoundPoint;

	return S_OK;
}

HRESULT CSoundPoint_Manager::Add_SoundPoint(const wstring& FilePath, vector<shared_ptr<CSoundPoint>>* SoundPointVector)
{
	FRapidJson InData;
	if (FAILED(InData.Input_Json(FilePath)))
		return E_FAIL;

	shared_ptr<CSoundPoint>	pSoundPoint = { nullptr };

	_uint iSize = InData.Read_ArraySize("SoundPoints");
	for (_uint i = 0; i < iSize; i++)
	{
		TSoundPointDesc Desc = {};

		FRapidJson SoundData;
		InData.Read_ObjectFromArray("SoundPoints", i, SoundData);

		_uint iGroupSize = SoundData.Read_ArraySize("GroupNames");
		string strName;

		for (_uint j = 0; j < iGroupSize; j++)
		{
			string strGroupName;
			string strSoundName;
			_uint	iSoundGroup;

			SoundData.Read_Data("Name", strName);
			SoundData.Read_DataFromArray("GroupNames", j, strGroupName);
			SoundData.Read_DataFromArray("SoundNames", j, strSoundName);
			SoundData.Read_Data("SoundGroup", iSoundGroup);
			SoundData.Read_Data("RollOffMode", Desc.eRollOffMode);
			SoundData.Read_Data("Volume", Desc.fVolume);
			SoundData.Read_Data("3DMin", Desc.f3DMin);
			SoundData.Read_Data("3DMax", Desc.f3DMax);
			SoundData.Read_Data("Position", Desc.vPosition);

			Desc.GroupNames.emplace_back(strGroupName);
			Desc.SoundNames.emplace_back(strSoundName);
			Desc.eSoundGroup = (ESoundGroup)iSoundGroup;
		}

		Add_SoundPoint(strName, Desc, &pSoundPoint);
	}

	return S_OK;
}

_bool CSoundPoint_Manager::Exists_SoundPointName(const string& strName)
{
	shared_ptr<CSoundPoint> pFindSoundPoint = Find_SoundPoint(strName);

	return (pFindSoundPoint != nullptr);
}

void CSoundPoint_Manager::Release_SoundPoint(shared_ptr<CSoundPoint> pSoundPoint)
{
	// pLight가 m_Lights에 존재하는지 확인
	auto iter = find(m_SoundPoints.begin(), m_SoundPoints.end(), pSoundPoint);
	if (iter == m_SoundPoints.end()) {
		// pLight가 m_Lights에 존재하지 않음

		pSoundPoint.reset();
		return;
	}

	// m_Lights에서 pLight를 제거
	(*iter).reset();
	m_SoundPoints.erase(iter);
}

void CSoundPoint_Manager::Reset_SoundPoint()
{
	if (!m_SoundPoints.empty()) {
		for (auto& pLight : m_SoundPoints)
			pLight.reset();
		m_SoundPoints.clear();
	}
}



shared_ptr<CSoundPoint> CSoundPoint_Manager::Make_SoundPoint_On_Owner(weak_ptr<CGameObject> _pOwner)
{
	//사용 예
	shared_ptr<CSoundPoint> pSoundPoint = nullptr;

	if (_pOwner.lock() != nullptr)
	{
		
	} // 성공하면 빛을 반환 실패하면 null 반환

	return pSoundPoint;
}

shared_ptr<CSoundPoint> CSoundPoint_Manager::Find_SoundPoint(const string& Name)
{
	if (Name == "")
		return nullptr;

	for (auto& pSoundPoint : m_SoundPoints)
	{
		if (pSoundPoint->Get_Name() == Name)
		{
			return pSoundPoint;
		}
	}

	return nullptr;
}

CSoundPoint_Manager* CSoundPoint_Manager::Create()
{
	CSoundPoint_Manager* pInstance = new CSoundPoint_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSoundPoint_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundPoint_Manager::Free()
{
	for (auto& pSoundPoint : m_SoundPoints)
		pSoundPoint.reset();

	m_SoundPoints.clear();
}

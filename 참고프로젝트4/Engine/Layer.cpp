#include "Layer.h"
#include "GameObject.h"
#include "ObjPool_Manager.h"
#include "GameInstance.h"

CLayer::CLayer()
{

}

HRESULT CLayer::Add_GameObject(shared_ptr<CGameObject> pGameObject)
{
	if (nullptr == pGameObject)
		RETURN_EFAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Tick(_cref_time fTimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != *iter)
		{
			if ((*iter)->IsState(OBJSTATE::Begin_Play))
				(*iter)->Begin_Play(fTimeDelta);

			if ((*iter)->IsState(OBJSTATE::Active) && (*iter)->IsState(OBJSTATE::Tick) && !(*iter)->IsState(OBJSTATE::Begin_Play))
			{
				if ((*iter)->IsState(OBJSTATE::DynamicObject))
				{
					if(!CGameInstance::m_bPauseGame)
						(*iter)->Priority_Tick(fTimeDelta * CGameInstance::m_fAdjustTimeDelta);
				}
				else
					(*iter)->Priority_Tick(fTimeDelta);
			}
			iter++;			
		}
	}	
}

void CLayer::Tick(_cref_time fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->IsState(OBJSTATE::Active) && pGameObject->IsState(OBJSTATE::Tick) && !pGameObject->IsState(OBJSTATE::Begin_Play))
			{
				if (pGameObject->IsState(OBJSTATE::DynamicObject))
				{
					if (!CGameInstance::m_bPauseGame)
						pGameObject->Tick(fTimeDelta * CGameInstance::m_fAdjustTimeDelta);
				}
				else
					pGameObject->Tick(fTimeDelta);
			}
		}
		
	}
}

void CLayer::Late_Tick(_cref_time fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->IsState(OBJSTATE::Active) && pGameObject->IsState(OBJSTATE::Tick) && !pGameObject->IsState(OBJSTATE::Begin_Play))
			{
				if (pGameObject->IsState(OBJSTATE::DynamicObject))
				{
					if (!CGameInstance::m_bPauseGame)
						pGameObject->Late_Tick(fTimeDelta * CGameInstance::m_fAdjustTimeDelta);
				}
				else
					pGameObject->Late_Tick(fTimeDelta);
			}

			if (pGameObject->IsState(OBJSTATE::End_Play))
				pGameObject->End_Play(fTimeDelta);
		}
	}
}

void CLayer::Before_Render(_cref_time fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->IsState(OBJSTATE::Active) && pGameObject->IsState(OBJSTATE::Render))
				pGameObject->Before_Render(fTimeDelta);
		}
	}
}

shared_ptr<CComponent> CLayer::Get_Component(const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag, strPartTag);
}

void CLayer::Clear_DeadObjects()
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != *iter)
		{
			if ((*iter)->IsState(OBJSTATE::WillRemoved))
			{
				if ((*iter)->Get_IsPoolObject())
					CObjPool_Manager::GetInstance()->Return_Object(*iter);
				else
				{
					iter->reset();
				}
				iter = m_GameObjects.erase(iter);
			}
			else
				iter++;
		}
	}
}

shared_ptr<CGameObject> CLayer::Get_Object(_uint iIndex)
{
	_uint i = 0;
	for (auto iter : m_GameObjects)
	{
		if (i == iIndex)
			return iter;
		i++;
	}
	return nullptr;
}

CLayer * CLayer::Create()
{
	return new CLayer;
}

void CLayer::Free()
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != *iter)
		{
				if ((*iter)->Get_IsPoolObject())
					CObjPool_Manager::GetInstance()->Return_Object(*iter);
		
				iter++;
		}
	}

	m_GameObjects.clear();
}

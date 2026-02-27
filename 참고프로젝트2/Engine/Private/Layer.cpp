#include "..\Public\Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter)->Get_Component(strComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Add_List(list<CGameObject*>* pList)
{
	if (pList == nullptr)
		return E_FAIL;

	//얕은복사
	*pList = m_GameObjects;

	return S_OK;
}


CGameObject* CLayer::Get_GameObject(_uint iIndex)
{
	auto iter = m_GameObjects.begin();

	// 유효한 오브젝트만 세기 위한 카운터
	size_t validIndex = 0;

	while (iter != m_GameObjects.end())
	{
		// 현재 오브젝트가 유효하지 않다면 건너뛰기
		if ((*iter)->m_bDead)
		{
			++iter;
			continue;
		}

		// 유효한 오브젝트를 찾은 경우
		if (validIndex == iIndex)
		{
			return (*iter);  // 유효한 오브젝트 반환
		}

		// 유효한 오브젝트일 때만 인덱스를 증가시킴
		++validIndex;
		++iter;
	}

	// 유효한 오브젝트를 찾지 못하면 nullptr 반환
	return nullptr;
}

void CLayer::Player_Update(_float fTimeDelta)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if ((*it)->m_bDead)  // 객체가 사망 상태라면
			it = m_GameObjects.erase(it);  // 목록에서 삭제 후 iterator 업데이트
		else if (!(*it)->m_bIsActive)
		{
			++it;
		}
		else
		{
			(*it)->Player_Update(fTimeDelta);  // 업데이트 호출
			++it;  // 다음 객체로 이동
		}
	}
}

void CLayer::Camera_Update(_float fTimeDelta)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if ((*it)->m_bDead)  // 객체가 사망 상태라면
			it = m_GameObjects.erase(it);  // 목록에서 삭제 후 iterator 업데이트
		else if (!(*it)->m_bIsActive)
		{
			++it;
		}
		else
		{
			(*it)->Camera_Update(fTimeDelta);  // 업데이트 호출
			++it;  // 다음 객체로 이동
		}
	}
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if ((*it)->m_bDead)
		{
			Safe_Release(*it);
			it = m_GameObjects.erase(it);
		}
		else if (!(*it)->m_bIsActive)
		{
			++it;
		}
		else
		{
			(*it)->Update(fTimeDelta);
			++it;
		}

		//(*it)->Update(fTimeDelta);
		//++it;
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if ((*it)->m_bDead)
			it = m_GameObjects.erase(it);
		else if (!(*it)->m_bIsActive)
		{
			++it;
		}
		else
		{
			(*it)->Late_Update(fTimeDelta);
			++it;
		}

		//(*it)->Late_Update(fTimeDelta);
		//++it;


	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();

}


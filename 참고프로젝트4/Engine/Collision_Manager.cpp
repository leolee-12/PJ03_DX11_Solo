#include "Collision_Manager.h"
#include "Collider.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize(_uint& iNumLayer)
{
	m_pColliderList.reserve(iNumLayer);
	for (_uint i = 0; i < iNumLayer; ++i)
		m_pColliderList.push_back({});

	return S_OK;
}


void CCollision_Manager::Tick()
{
	map<_uint, _bool> mapCheckSameLayer;

	for (_uint iRow = 0; iRow < m_pColliderList.size(); iRow++)
	{
		for (_uint iCol = 0; iCol < m_pColliderList.size(); iCol++)
		{
			if (!Check_LayerGroup(iRow,iCol))
				continue;

			if (iRow == iCol)
			{
				if (mapCheckSameLayer.count(iRow) == 0)
					mapCheckSameLayer.emplace(iRow, true);
				else
					continue;
			}

			Update_CollisionGroup(iCol, iRow);
		}
	}

	Remove_DeadCollider();
}

void CCollision_Manager::Update_CollisionGroup(const _uint& iLeftLayer, const _uint& iRightLayer)
{
	map<COLLIDER_ID, _bool>::iterator	CollisionInfo;

	for (auto& pLeftCol : m_pColliderList[iLeftLayer])
	{
		for (auto& pRightCol : m_pColliderList[iRightLayer])
		{
			
			if (pLeftCol->Get_Owner().lock()->IsState(OBJSTATE::Active) && pRightCol->Get_Owner().lock()->IsState(OBJSTATE::Active))
			{
				Set_CollisionInfo(CollisionInfo, pLeftCol, pRightCol);
				if ((pLeftCol->Get_IsEnable() && pRightCol->Get_IsEnable()) && pLeftCol->Collision(pRightCol))
				{
					if (!CollisionInfo->second) //처음 충돌
					{
						if ((pLeftCol->Get_IsEnable() && pRightCol->Get_IsEnable()))
						{
							pLeftCol->OnCollision_Enter(pLeftCol, pRightCol);
							pRightCol->OnCollision_Enter(pRightCol, pLeftCol);
							CollisionInfo->second = true;
						}
					}
					else
					{
						pLeftCol->OnCollision_Stay(pLeftCol, pRightCol);
						pRightCol->OnCollision_Stay(pRightCol, pLeftCol);

					}

				}
				else
				{
					if (CollisionInfo->second)
					{
						pLeftCol->OnCollision_Exit(pLeftCol, pRightCol);
						pRightCol->OnCollision_Exit(pRightCol, pLeftCol);
						CollisionInfo->second = false;
					}
				}
			}
			
		}
	}
}


_bool CCollision_Manager::Check_LayerGroup(const _uint& iLeftLayer, const _uint& iRightLayer)
{
	vector<pair<_uint, _uint>> ::iterator iter;
	iter = find_if(m_vecCheckLayer.begin(), m_vecCheckLayer.end(), [=](pair<_uint, _uint>& iLayerGroup)
		{
			if ((iLayerGroup.first == iLeftLayer && iLayerGroup.second == iRightLayer))
				return true;
			return false;
		});

	if (iter == m_vecCheckLayer.end())
		return false;
	return true;
}

void CCollision_Manager::Add_Collider(const _uint& iLayer, CCollider* pCollider)
{
	m_pColliderList[iLayer].push_back(pCollider);
	m_mapColliderID.emplace(pCollider->Get_ColliderID(),pCollider);
	pCollider->Set_ColliderLayer(iLayer);

}

void CCollision_Manager::Add_LayerGroup(const _uint& iLeftLayer, const _uint& iRightLayer)
{
	if (!Check_LayerGroup(iLeftLayer, iRightLayer))
		m_vecCheckLayer.push_back({ iLeftLayer,iRightLayer });
}

void CCollision_Manager::Remove_DeadCollider()
{
	for (_uint i = 0 ; i < m_pColliderList.size(); i++)
	{
		for (auto iter = m_pColliderList[i].begin(); iter != m_pColliderList[i].end();)
		{
			if ((*iter)->Get_Owner().expired() || (*iter)->Get_Owner().lock()->IsState(OBJSTATE::WillRemoved))
			{
				_uint ColliderID = (*iter)->Get_ColliderID();
				for (map<COLLIDER_ID, _bool> :: iterator iterInfo = m_mapCollisionInfo.begin(); iterInfo!= m_mapCollisionInfo.end();)
				{
					if ((*iterInfo).first.iLeft_ID == ColliderID || (*iterInfo).first.iRight_ID == ColliderID)
					{
						if (m_mapColliderID.find((*iterInfo).first.iLeft_ID) != m_mapColliderID.end() && m_mapColliderID.find((*iterInfo).first.iRight_ID) != m_mapColliderID.end())
						{
							m_mapColliderID[(*iterInfo).first.iRight_ID]->OnCollision_Exit(m_mapColliderID[(*iterInfo).first.iRight_ID], m_mapColliderID[(*iterInfo).first.iLeft_ID]);
							m_mapColliderID[(*iterInfo).first.iLeft_ID]->OnCollision_Exit(m_mapColliderID[(*iterInfo).first.iLeft_ID], m_mapColliderID[(*iterInfo).first.iRight_ID]);
						}
						m_mapCollisionInfo.erase(iterInfo++);
					}
					else
						++iterInfo;
				}


				m_mapColliderID.erase((*iter)->Get_ColliderID());
				iter = m_pColliderList[i].erase(iter);
			}
			else
				iter++;
		}
	}
}

void CCollision_Manager::Set_CollisionInfo(map<COLLIDER_ID, _bool>::iterator& iter, CCollider* pLeftCol, CCollider* pRightCol)
{
	COLLIDER_ID ID;

	ID.iLeft_ID = pLeftCol->Get_ColliderID();
	ID.iRight_ID = pRightCol->Get_ColliderID();

	iter = m_mapCollisionInfo.find(ID);

	if (m_mapCollisionInfo.end() == iter)
	{
		m_mapCollisionInfo.insert({ ID, FALSE });
		iter = m_mapCollisionInfo.find(ID);
	}
}

void CCollision_Manager::Clear()
{
	Free();
}

CCollision_Manager* CCollision_Manager::Create(_uint& iNumLayer)
{
	CCollision_Manager* pInstance = new CCollision_Manager;

	if (FAILED(pInstance->Initialize(iNumLayer)))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollision_Manager::Free()
{
	for (auto& iter : m_pColliderList)
	{
		iter.clear();
	}
	m_mapCollisionInfo.clear();
	m_mapColliderID.clear();
	m_vecCheckLayer.clear();
}

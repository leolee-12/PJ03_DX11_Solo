#include "QuadTree.h"

#include "GameInstance.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
    m_iCornerIndices[ENUM_CLASS(CORNER::LT)] = iLT;
    m_iCornerIndices[ENUM_CLASS(CORNER::RT)] = iRT;
    m_iCornerIndices[ENUM_CLASS(CORNER::RB)] = iRB;
    m_iCornerIndices[ENUM_CLASS(CORNER::LB)] = iLB;

    if (1 == m_iCornerIndices[ENUM_CLASS(CORNER::RT)] - m_iCornerIndices[ENUM_CLASS(CORNER::LT)])
        return S_OK;

    m_iCenterIndex = (m_iCornerIndices[ENUM_CLASS(CORNER::LT)] + m_iCornerIndices[ENUM_CLASS(CORNER::RB)]) >> 1;

    _uint       iLC, iTC, iRC, iBC;

    iLC = (m_iCornerIndices[ENUM_CLASS(CORNER::LT)] + m_iCornerIndices[ENUM_CLASS(CORNER::LB)]) >> 1;
    iTC = (m_iCornerIndices[ENUM_CLASS(CORNER::LT)] + m_iCornerIndices[ENUM_CLASS(CORNER::RT)]) >> 1;
    iRC = (m_iCornerIndices[ENUM_CLASS(CORNER::RT)] + m_iCornerIndices[ENUM_CLASS(CORNER::RB)]) >> 1;
    iBC = (m_iCornerIndices[ENUM_CLASS(CORNER::LB)] + m_iCornerIndices[ENUM_CLASS(CORNER::RB)]) >> 1;

    m_pChildren[ENUM_CLASS(CORNER::LT)] = CQuadTree::Create(m_iCornerIndices[ENUM_CLASS(CORNER::LT)], iTC, m_iCenterIndex, iLC);
    m_pChildren[ENUM_CLASS(CORNER::RT)] = CQuadTree::Create(iTC, m_iCornerIndices[ENUM_CLASS(CORNER::RT)], iRC, m_iCenterIndex);
    m_pChildren[ENUM_CLASS(CORNER::RB)] = CQuadTree::Create(m_iCenterIndex, iRC, m_iCornerIndices[ENUM_CLASS(CORNER::RB)], iBC);
    m_pChildren[ENUM_CLASS(CORNER::LB)] = CQuadTree::Create(iLC, m_iCenterIndex, iBC, m_iCornerIndices[ENUM_CLASS(CORNER::LB)]);

    return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVertexPositions, _uint* pIndices, _uint* pNumIndices)
{
    if (nullptr == m_pChildren[0] || 
        true == isDraw(pGameInstance, pVertexPositions))
    {
        _bool       isIn[4] = {
             pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVertexPositions[m_iCornerIndices[0]])),
             pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVertexPositions[m_iCornerIndices[1]])),
             pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVertexPositions[m_iCornerIndices[2]])),
             pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVertexPositions[m_iCornerIndices[3]])),
        };

        _bool      isDraw[4] = { true, true, true, true };

        for (size_t i = 0; i < 4; i++)
        {
            if(nullptr != m_pNeighbors[i])
                isDraw[i] = m_pNeighbors[i]->isDraw(pGameInstance, pVertexPositions);
        }

        if (true == isDraw[0] &&
            true == isDraw[1] &&
            true == isDraw[2] &&
            true == isDraw[3])
        {
            if (true == isIn[0] ||
                true == isIn[1] ||
                true == isIn[2])
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[0];
                pIndices[(*pNumIndices)++] = m_iCornerIndices[1];
                pIndices[(*pNumIndices)++] = m_iCornerIndices[2];
            }

            if (true == isIn[0] ||
                true == isIn[2] ||
                true == isIn[3])
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[0];
                pIndices[(*pNumIndices)++] = m_iCornerIndices[2];
                pIndices[(*pNumIndices)++] = m_iCornerIndices[3];
            }

            return;
        }

        _uint       iLC, iTC, iRC, iBC;

        iLC = (m_iCornerIndices[ENUM_CLASS(CORNER::LT)] + m_iCornerIndices[ENUM_CLASS(CORNER::LB)]) >> 1;
        iTC = (m_iCornerIndices[ENUM_CLASS(CORNER::LT)] + m_iCornerIndices[ENUM_CLASS(CORNER::RT)]) >> 1;
        iRC = (m_iCornerIndices[ENUM_CLASS(CORNER::RT)] + m_iCornerIndices[ENUM_CLASS(CORNER::RB)]) >> 1;
        iBC = (m_iCornerIndices[ENUM_CLASS(CORNER::LB)] + m_iCornerIndices[ENUM_CLASS(CORNER::RB)]) >> 1;

        if (true == isIn[0] ||
            true == isIn[2] ||
            true == isIn[3])
        {
            if (false == isDraw[ENUM_CLASS(NEIGHBOR::LEFT)])
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LT)];
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = iLC;

                pIndices[(*pNumIndices)++] = iLC;
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LB)];
            }
            else
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LT)];
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LB)];
            }

            if (false == isDraw[ENUM_CLASS(NEIGHBOR::BOTTOM)])
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LB)];
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = iBC;

                pIndices[(*pNumIndices)++] = iBC;
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RB)];
            }
            else
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LB)];
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RB)];
            }
        }


        if (true == isIn[0] ||
            true == isIn[1] ||
            true == isIn[2])
        {
            if (false == isDraw[ENUM_CLASS(NEIGHBOR::TOP)])
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LT)];
                pIndices[(*pNumIndices)++] = iTC;
                pIndices[(*pNumIndices)++] = m_iCenterIndex;

                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = iTC;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RT)];
            }
            else
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::LT)];;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RT)];;
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
            }

            if (false == isDraw[ENUM_CLASS(NEIGHBOR::RIGHT)])
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RT)];
                pIndices[(*pNumIndices)++] = iRC;
                pIndices[(*pNumIndices)++] = m_iCenterIndex;

                pIndices[(*pNumIndices)++] = m_iCenterIndex;
                pIndices[(*pNumIndices)++] = iRC;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RB)];
            }
            else
            {
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RT)];;
                pIndices[(*pNumIndices)++] = m_iCornerIndices[ENUM_CLASS(CORNER::RB)];;
                pIndices[(*pNumIndices)++] = m_iCenterIndex;
            }
        }

        return;
    }

    _float      fRadius = XMVectorGetX(
        XMVector3Length(
            XMLoadFloat3(&pVertexPositions[m_iCenterIndex]) - 
            XMLoadFloat3(&pVertexPositions[m_iCornerIndices[ENUM_CLASS(CORNER::LT)]])));

    if (true == pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVertexPositions[m_iCenterIndex]), fRadius))
    {
        for (_int i = 0; i < ENUM_CLASS(CORNER::END); ++i)
        {
            if (nullptr != m_pChildren[i])
                m_pChildren[i]->Culling(pGameInstance, pVertexPositions, pIndices, pNumIndices);

        }
    }

    
}

void CQuadTree::SetUp_Neighbors()
{
    if (nullptr == m_pChildren[0]->m_pChildren[0])
        return;

    m_pChildren[ENUM_CLASS(CORNER::LT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::RIGHT)] = m_pChildren[ENUM_CLASS(CORNER::RT)];
    m_pChildren[ENUM_CLASS(CORNER::LT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::BOTTOM)] = m_pChildren[ENUM_CLASS(CORNER::LB)];

    m_pChildren[ENUM_CLASS(CORNER::RT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::LEFT)] = m_pChildren[ENUM_CLASS(CORNER::LT)];
    m_pChildren[ENUM_CLASS(CORNER::RT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::BOTTOM)] = m_pChildren[ENUM_CLASS(CORNER::RB)];

    m_pChildren[ENUM_CLASS(CORNER::RB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::LEFT)] = m_pChildren[ENUM_CLASS(CORNER::LB)];
    m_pChildren[ENUM_CLASS(CORNER::RB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::TOP)] = m_pChildren[ENUM_CLASS(CORNER::RT)];

    m_pChildren[ENUM_CLASS(CORNER::LB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::RIGHT)] = m_pChildren[ENUM_CLASS(CORNER::RB)];
    m_pChildren[ENUM_CLASS(CORNER::LB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::TOP)] = m_pChildren[ENUM_CLASS(CORNER::LT)];

    if (nullptr != m_pNeighbors[ENUM_CLASS(NEIGHBOR::RIGHT)])
    {
        m_pChildren[ENUM_CLASS(CORNER::RT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::RIGHT)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::RIGHT)]->m_pChildren[ENUM_CLASS(CORNER::LT)];
        m_pChildren[ENUM_CLASS(CORNER::RB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::RIGHT)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::RIGHT)]->m_pChildren[ENUM_CLASS(CORNER::LB)];
    }

    if (nullptr != m_pNeighbors[ENUM_CLASS(NEIGHBOR::BOTTOM)])
    {
        m_pChildren[ENUM_CLASS(CORNER::LB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::BOTTOM)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::BOTTOM)]->m_pChildren[ENUM_CLASS(CORNER::LT)];
        m_pChildren[ENUM_CLASS(CORNER::RB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::BOTTOM)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::BOTTOM)]->m_pChildren[ENUM_CLASS(CORNER::RT)];
    }

    if (nullptr != m_pNeighbors[ENUM_CLASS(NEIGHBOR::LEFT)])
    {
        m_pChildren[ENUM_CLASS(CORNER::LT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::LEFT)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::LEFT)]->m_pChildren[ENUM_CLASS(CORNER::RT)];
        m_pChildren[ENUM_CLASS(CORNER::LB)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::LEFT)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::LEFT)]->m_pChildren[ENUM_CLASS(CORNER::RB)];
    }

    if (nullptr != m_pNeighbors[ENUM_CLASS(NEIGHBOR::TOP)])
    {
        m_pChildren[ENUM_CLASS(CORNER::LT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::TOP)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::TOP)]->m_pChildren[ENUM_CLASS(CORNER::LB)];
        m_pChildren[ENUM_CLASS(CORNER::RT)]->m_pNeighbors[ENUM_CLASS(NEIGHBOR::TOP)] = m_pNeighbors[ENUM_CLASS(NEIGHBOR::TOP)]->m_pChildren[ENUM_CLASS(CORNER::RB)];
    }

    for (auto& pChild : m_pChildren)
        pChild->SetUp_Neighbors();
}

_bool CQuadTree::isDraw(CGameInstance* pGameInstance, const _float3* pVertexPositions)
{
    _float      fCamDistance = 
        XMVectorGetX(XMVector3Length(XMLoadFloat4(pGameInstance->Get_CamPosition()) - 
        XMLoadFloat3(&pVertexPositions[m_iCenterIndex])));

    _float      fWidth = m_iCornerIndices[ENUM_CLASS(CORNER::RT)] - m_iCornerIndices[ENUM_CLASS(CORNER::LT)];

    return fCamDistance * 0.2f > fWidth;
}

CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
    CQuadTree* pInstance = new CQuadTree();

    if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
    {
        MSG_BOX("Failed to Created : CQuadTree");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CQuadTree::Free()
{
	__super::Free();
    
    for (auto& pChild : m_pChildren)
        Safe_Release(pChild);

}

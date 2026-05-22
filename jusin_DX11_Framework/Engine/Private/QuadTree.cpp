#include "QuadTree.h"
#include "GameInstance.h"

CQuadTree::CQuadTree()
{

}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER::LT] = iLT;
	m_iCorners[CORNER::RT] = iRT;
	m_iCorners[CORNER::RB] = iRB;
	m_iCorners[CORNER::LB] = iLB;

	if (1 == m_iCorners[CORNER::RT] - m_iCorners[CORNER::LT])
		return S_OK;

	m_iCenter = (m_iCorners[CORNER::LT] + m_iCorners[CORNER::RB]) >> 1;

	_uint iLC, iTC, iRC, iBC;

	iLC = (m_iCorners[CORNER::LT] + m_iCorners[CORNER::LB]) >> 1;
	iTC = (m_iCorners[CORNER::LT] + m_iCorners[CORNER::RT]) >> 1;
	iRC = (m_iCorners[CORNER::RT] + m_iCorners[CORNER::RB]) >> 1;
	iBC = (m_iCorners[CORNER::LB] + m_iCorners[CORNER::RB]) >> 1;

	m_Children[CORNER::LT] = CQuadTree::Create(m_iCorners[CORNER::LT], iTC, m_iCenter, iLC);
	m_Children[CORNER::RT] = CQuadTree::Create(iTC, m_iCorners[CORNER::RT], iRC, m_iCenter);
	m_Children[CORNER::RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER::RB], iBC);
	m_Children[CORNER::LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER::LB]);

	return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGI, const _float3* pVtxPos, _uint* pIndices, _uint* pNumIndices)
{
	if (nullptr == m_Children[CORNER::LT])
	{
		_bool isIn[4] =
		{
			pGI->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVtxPos[m_iCorners[0]]), 1.f)),
			pGI->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVtxPos[m_iCorners[1]]), 1.f)),
			pGI->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVtxPos[m_iCorners[2]]), 1.f)),
			pGI->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVtxPos[m_iCorners[3]]), 1.f)),
		};

		if (true == isIn[0] &&
			true == isIn[1] &&
			true == isIn[2])
		{
			pIndices[(*pNumIndices)++] = m_iCorners[0];
			pIndices[(*pNumIndices)++] = m_iCorners[1];
			pIndices[(*pNumIndices)++] = m_iCorners[2];
		}


		if (true == isIn[0] &&
			true == isIn[2] &&
			true == isIn[3])
		{
			pIndices[(*pNumIndices)++] = m_iCorners[0];
			pIndices[(*pNumIndices)++] = m_iCorners[2];
			pIndices[(*pNumIndices)++] = m_iCorners[3];
		}

		return;
	}

	_float fRange = XMVector3Length(XMLoadFloat3(&pVtxPos[m_iCorners[CORNER::LT]]) - XMLoadFloat3(&pVtxPos[m_iCenter])).m128_f32[0];

	if (true == pGI->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVtxPos[m_iCenter]), fRange))
	{
		for (auto& pChild : m_Children)
		{
			if (nullptr != pChild)
				pChild->Culling(pGI, pVtxPos, pIndices, pNumIndices);
		}
	}
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
	for (auto& pChild : m_Children)
		Safe_Release(pChild);

	__super::Free();
}
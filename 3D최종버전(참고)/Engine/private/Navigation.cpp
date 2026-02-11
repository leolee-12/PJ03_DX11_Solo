#include "Navigation.h"
#include "Cell.h"

#include "Shader.h"
#include "GameInstance.h"

const _float4x4* CNavigation::m_pParentMatrix = { nullptr };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
    , m_iCurrentCellIndex { Prototype.m_iCurrentCellIndex }
    , m_Cells { Prototype.m_Cells }
#ifdef _DEBUG
    , m_pShader { Prototype.m_pShader }
#endif
{
    for (auto& pCell : m_Cells)
        Safe_AddRef(pCell);

#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile)
{
    _ulong			dwByte = { 0 };
    HANDLE			hFile = CreateFile(pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    _float3         vPoints[ENUM_CLASS(POINT::END)] = {};

    while(true)
    {
        ReadFile(hFile, vPoints, sizeof(_float3) * ENUM_CLASS(POINT::END), &dwByte, nullptr);
        if (0 == dwByte)
            break;

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
        if (nullptr == pCell)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

    SetUp_Neighbors();

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if (nullptr != pArg)
        m_iCurrentCellIndex = static_cast<NAVIGATION_DESC*>(pArg)->iCurrentCellIndex;

	return S_OK;
}

_bool CNavigation::isMove(_fvector vResultPos)
{
    if (-1 == m_iCurrentCellIndex)
        return false;

    _vector     vPosition = XMVector3TransformCoord(vResultPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    _int        iNeighborIndex = { -1 };

    if (true == m_Cells[m_iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex))
    {
        return true;
    }
    else
    {
        if (-1 != iNeighborIndex)
        {
            for (;;)
            {
                if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
                    break;

                if (-1 == iNeighborIndex)
                    return false;
            }

            m_iCurrentCellIndex = iNeighborIndex;

            return true;
        }
        else
            return false;        
    }  
}

_vector CNavigation::SetUp_OnNavigation(_fvector vWorldPos)
{
    _vector     vPosition = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    return XMVectorSetY(vPosition, m_Cells[m_iCurrentCellIndex]->Compute_Height(vPosition));    
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{


    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;

    _float4x4 WorldMatrix = *m_pParentMatrix;

    _float4     vColor = {};

    if (-1 == m_iCurrentCellIndex)    
    {
        vColor = _float4(0.f, 1.f, 0.f, 1.f);    
        m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;

        m_pShader->Begin(0);
        for (auto& pCell : m_Cells)
        {
            pCell->Render();
        }
    }
    else
    {
        vColor = _float4(1.f, 0.0f, 0.f, 1.f);
        m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

        WorldMatrix._42 += 0.1f;
        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;

        m_pShader->Begin(0);
        m_Cells[m_iCurrentCellIndex]->Render();
    }
        

    

    

    
    

    return S_OK;
}

#endif

void CNavigation::SetUp_Neighbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell)
                continue;

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT::A), pSourCell->Get_Point(POINT::B)))
            {
                pSourCell->Set_Neighbor(LINE::AB, pDestCell);
            }

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT::B), pSourCell->Get_Point(POINT::C)))
            {
                pSourCell->Set_Neighbor(LINE::BC, pDestCell);
            }

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT::C), pSourCell->Get_Point(POINT::A)))
            {
                pSourCell->Set_Neighbor(LINE::CA, pDestCell);
            }
        }
    }
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFile)))
    {
        MSG_BOX("Failed to Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

    for (auto& pCell : m_Cells)
        Safe_Release(pCell);

    m_Cells.clear();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif
}

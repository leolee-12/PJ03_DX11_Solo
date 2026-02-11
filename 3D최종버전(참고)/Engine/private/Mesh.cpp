#include "Mesh.h"
#include "Bone.h"

#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
    : CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    strcpy_s(m_szName, pAIMesh->mName.data);
    m_iMaterialIndex = pAIMesh->mMaterialIndex;
    m_iNumVertexBuffers = 1;
    m_iNumVertices = pAIMesh->mNumVertices;


    m_iNumIndices = pAIMesh->mNumFaces * 3;
    m_iIndexStride = 4;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER


    HRESULT         hr = MODELTYPE::NONANIM == eType ? Ready_VertexBuffer_For_NonAnim(pAIMesh, PreTransformMatrix) : Ready_VertexBuffer_For_Anim(pAIMesh, Bones);
    if (FAILED(hr))
        return E_FAIL;   

#pragma endregion

#pragma region INDEX_BUFFER 
    D3D11_BUFFER_DESC           IBDesc{};
    IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    IBDesc.Usage = D3D11_USAGE_DEFAULT;
    IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IBDesc.CPUAccessFlags = 0;
    IBDesc.MiscFlags = 0;
    IBDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint       iNumIndices = { 0 };

    for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
    {
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
    }

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

#pragma endregion


    return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones)
{
    for (size_t i = 0; i < m_iNumBones; i++)
    {
        XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
    }

    return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    m_iVertexStride = sizeof(VTXMESH);
    D3D11_BUFFER_DESC           VBDesc{};
    VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition,
            XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vNormal,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

        memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vBinormal,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix));


        
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones)
{
    m_iVertexStride = sizeof(VTXSKINMESH);
   
    D3D11_BUFFER_DESC           VBDesc{};
    VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXSKINMESH* pVertices = new VTXSKINMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXSKINMESH) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));     
        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));       
        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
    }

    /* 모델에 존재하는 전체 뼈들 중, 이 메시에 영향을 주는 뼈는 몇개인가? */
    m_iNumBones = pAIMesh->mNumBones;

    m_OffsetMatrices.reserve(m_iNumBones);

    for (size_t i = 0; i < m_iNumBones; i++)
    {        
        aiBone*     pAIBone = pAIMesh->mBones[i];

        _float4x4       OffsetMatrix = {};
        memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
        m_OffsetMatrices.push_back(OffsetMatrix);

        _uint       iBoneIndex = {};    

        auto    iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
            if(true == pBone->Compare_Name(pAIBone->mName.data))
                return true;         

            ++iBoneIndex;
            return false;
        });

        if (iter == Bones.end())
            return E_FAIL;

        m_BoneIndices.push_back(iBoneIndex);  

        /* i번째 뼈가 어떤 정점들에게 영향을 주는가? */
        for (size_t j = 0; j < pAIBone->mNumWeights; j++)
        {
            /* i번째 뼈가 영향을 주는 j번째 정점관련된 정보(몇번째 정점이다, 얼마나 영향을 준다.) */
            aiVertexWeight     AIWeight = pAIBone->mWeights[j];

            if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.x = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
            }

            else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.y = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
            }

            else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.z = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
            }

            else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.w)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.w = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
            }
        }
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    if (0 == m_iNumBones)
    {
        m_iNumBones = 1;

        _uint       iBoneIndex = {};

        auto    iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
            if (true == pBone->Compare_Name(m_szName))
                return true;

            ++iBoneIndex;
            return false;
            });

        if (iter == Bones.end())
            return E_FAIL;

        _float4x4       OffsetMatrix;
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

        m_OffsetMatrices.push_back(OffsetMatrix);

        m_BoneIndices.push_back(iBoneIndex);
    }

    return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, Bones, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CComponent* CMesh::Clone(void* pArg)
{
    CMesh* pInstance = new CMesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMesh::Free()
{
    __super::Free();


}

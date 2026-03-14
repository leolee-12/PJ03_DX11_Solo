# O06-C: Mesh 본 바인딩 + 스키닝

## 1. NonAnim vs Anim 메시

| 구분 | NonAnim (VTXMESH) | Anim (VTXSKINMESH) |
|------|-------------------|-------------------|
| **본 데이터** | 없음 | BlendIndex + BlendWeight |
| **PreTransform** | CPU에서 정점 변환 | 셰이더에서 본 행렬로 변환 |
| **정점 크기** | Position+Normal+UV+Tangent+Binormal | Position+Normal+UV+Tangent+BlendIndex+BlendWeight |
| **변환 주체** | 빌드 타임 (CPU) | 런타임 (GPU) |

---

## 2. NonAnim 메시 — Ready_VertexBuffer_For_NonAnim

```cpp
HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(
    const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        // Position: PreTransformMatrix로 좌표계 보정
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition,
            XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition),
                                    PreTransformMatrix));

        // Normal/Tangent/Binormal: 방향 벡터이므로 TransformNormal 사용
        XMStoreFloat3(&pVertices[i].vNormal,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal),
                                     PreTransformMatrix));
        // Tangent, Binormal도 동일...

        // UV: 변환 없이 그대로 복사
        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
    }
}
```

**TransformCoord vs TransformNormal**:
- `TransformCoord`: w=1로 변환 → 이동 포함 (위치)
- `TransformNormal`: w=0으로 변환 → 이동 제외 (방향 벡터)

NonAnim은 본이 없으므로 **빌드 시 CPU에서 좌표계 보정**을 완료한다.

---

## 3. Anim 메시 — Ready_VertexBuffer_For_Anim

### 3-1. 기본 정점 데이터 복사

```cpp
VTXSKINMESH* pVertices = new VTXSKINMESH[m_iNumVertices];

for (size_t i = 0; i < m_iNumVertices; i++)
{
    memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
    memcpy(&pVertices[i].vNormal,   &pAIMesh->mNormals[i],  sizeof(_float3));
    memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
    memcpy(&pVertices[i].vTangent,  &pAIMesh->mTangents[i], sizeof(_float3));
}
```

Anim 메시는 **PreTransformMatrix를 적용하지 않는다**. 좌표계 보정은
본의 Combined 행렬에 포함된 PreTransformMatrix가 담당한다.

### 3-2. 본 정보 수집

```cpp
m_iNumBones = pAIMesh->mNumBones;

for (size_t i = 0; i < m_iNumBones; i++)
{
    aiBone* pAIBone = pAIMesh->mBones[i];

    // ① Offset 행렬: 바인드 포즈의 역행렬
    _float4x4 OffsetMatrix{};
    memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
    XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
    m_OffsetMatrices.push_back(OffsetMatrix);

    // ② 본 이름으로 전체 본 배열에서 인덱스 검색
    _uint iBoneIndex = 0;
    auto iter = find_if(Bones.begin(), Bones.end(),
        [&](CBone* pBone) -> _bool {
            if (pBone->Compare_Name(pAIBone->mName.data)) return true;
            ++iBoneIndex;
            return false;
        });
    m_BoneIndices.push_back(iBoneIndex);

    // ③ 정점별 가중치 기록
    for (size_t j = 0; j < pAIBone->mNumWeights; j++)
    {
        aiVertexWeight AIWeight = pAIBone->mWeights[j];
        // BlendWeight의 빈 슬롯(xyzw)에 순서대로 채움
        if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.x) {
            pVertices[AIWeight.mVertexId].vBlendIndex.x = i;
            pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
        }
        else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.y) { ... }
        else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.z) { ... }
        else if (0.0f == pVertices[AIWeight.mVertexId].vBlendWeight.w) { ... }
    }
}
```

### 3-3. 본이 없는 메시 처리

```cpp
if (0 == m_iNumBones)
{
    m_iNumBones = 1;
    // 메시 이름과 같은 본을 찾아 단일 본으로 연결
    // OffsetMatrix = Identity
    m_OffsetMatrices.push_back(Identity);
    m_BoneIndices.push_back(iBoneIndex);
}
```

일부 메시는 `aiBone`이 없지만 `aiNode` 계층에는 존재한다.
이 경우 **메시 이름과 같은 본**을 찾아 단위 행렬 Offset으로 연결한다.

---

## 4. Offset 행렬의 의미

```
Offset 행렬 = 바인드 포즈에서의 본 공간 → 메시 공간 역변환

정점이 메시 공간(바인드 포즈)에서 정의됨
  → Offset으로 본 공간으로 이동
  → Combined로 현재 포즈 위치로 이동

최종 스키닝 행렬 = Offset × Combined
```

### 시각화

```
바인드 포즈 (T-Pose):          현재 포즈 (팔 굽힘):
    ■ 머리                        ■ 머리
    │                              │
  ──┼── 어깨                     ──┤  어깨
    │   │                          │ ╲
    │   │ 팔 (바인드 위치)          │  ╲ 팔 (현재 위치)
    │                              │

① 정점 × Offset = 본 로컬 공간으로 이동 (바인드 포즈 기준)
② 본 로컬 × Combined = 현재 포즈 월드 공간
```

---

## 5. Bind_BoneMatrices — 셰이더 전달

```cpp
HRESULT CMesh::Bind_BoneMatrices(CShader* pShader,
    const _char* pConstantName, const vector<CBone*>& Bones)
{
    for (size_t i = 0; i < m_iNumBones; i++)
    {
        // 스키닝 행렬 = Offset × Combined
        m_BoneMatrices[i] = m_OffsetMatrices[i] * Bones[m_BoneIndices[i]]->Combined;
    }

    // 상수 버퍼로 배열 전달
    return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}
```

### 데이터 흐름

```
m_OffsetMatrices[i]     ← 초기화 시 고정 (aiBone.mOffsetMatrix)
m_BoneIndices[i]        ← 이 메시의 i번째 본 → 전체 본 배열에서의 인덱스
Bones[m_BoneIndices[i]] ← 전체 본 배열에서 참조 (애니메이션 갱신됨)

m_BoneMatrices[i] = Offset[i] × Bones[BoneIndices[i]].Combined
```

### 셰이더 인덱스 매핑

```
메시의 본 인덱스 (i):     0      1      2      3
전체 본 배열 인덱스:       5      12     8      23
                          ↑      ↑      ↑      ↑
                      Shoulder  Elbow   Wrist   Hand

정점의 BlendIndex.x = 2  →  m_BoneMatrices[2]  →  Wrist의 스키닝 행렬
```

**BlendIndex는 메시 로컬 인덱스**이다. 전체 본 배열 인덱스가 아니다.
`m_BoneIndices`가 메시→전체 매핑을 담당한다.

---

## 6. VTXSKINMESH 정점 구조

```cpp
typedef struct tagVertexSkinMesh {
    XMFLOAT3 vPosition;       // 바인드 포즈 위치
    XMFLOAT3 vNormal;
    XMFLOAT2 vTexcoord;
    XMFLOAT3 vTangent;
    XMUINT4  vBlendIndex;     // 영향 본 인덱스 4개 (메시 로컬)
    XMFLOAT4 vBlendWeight;    // 가중치 4개 (합 = 1.0)
} VTXSKINMESH;
```

**최대 4개 본 영향**: 한 정점은 최대 4개 본의 영향을 받는다.
`BlendWeight.xyzw`가 0인 슬롯을 순서대로 채우는 방식.

### 셰이더에서 스키닝

```hlsl
// 정점 셰이더 (개념적 의사 코드)
float4x4 BoneMatrix =
    g_BoneMatrices[BlendIndex.x] * BlendWeight.x +
    g_BoneMatrices[BlendIndex.y] * BlendWeight.y +
    g_BoneMatrices[BlendIndex.z] * BlendWeight.z +
    g_BoneMatrices[BlendIndex.w] * BlendWeight.w;

float4 SkinnedPos = mul(float4(vPosition, 1), BoneMatrix);
```

---

## 7. g_iMaxNumBones

```cpp
_float4x4 m_BoneMatrices[g_iMaxNumBones] = {};
```

고정 크기 배열로 셰이더 상수 버퍼와 크기를 맞춘다.
셰이더 쪽에서도 `float4x4 g_BoneMatrices[MAX_BONES]`로 선언한다.

---

## 8. 핵심 정리

| 항목 | 설명 |
|------|------|
| **NonAnim** | CPU에서 PreTransform 적용, 본 데이터 없음 |
| **Anim** | PreTransform 미적용, 본이 런타임 변환 담당 |
| **BlendIndex/Weight** | 정점당 최대 4개 본, 가중치 합 1.0 |
| **Offset 행렬** | aiBone.mOffsetMatrix (바인드 포즈 역변환) |
| **스키닝 행렬** | `Offset × Combined` → 셰이더 상수 버퍼로 전달 |
| **인덱스 매핑** | BlendIndex = 메시 로컬 → BoneIndices로 전체 배열 참조 |
| **본 없는 메시** | 메시 이름 = 본 이름, Identity Offset으로 연결 |
| **Clone** | CVIBuffer 복사 (VB/IB AddRef 공유), 본 데이터는 벡터 값 복사 |

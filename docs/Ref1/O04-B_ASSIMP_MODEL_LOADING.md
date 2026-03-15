# O04-B: Assimp Model 로딩 파이프라인

## 1. CModel 개요

### 클래스 구조

```cpp
class CModel final : public CComponent {
    // Assimp
    const aiScene*       m_pAIScene;          // Assimp 씬 데이터
    Assimp::Importer     m_Importer;          // Assimp 임포터 (수명 관리)
    MODELTYPE            m_eType;             // ANIM or NONANIM
    _float4x4            m_PreTransformMatrix; // 사전 변환 행렬

    // 서브 리소스
    vector<CMesh*>       m_Meshes;            // 메시 목록
    vector<CMaterial*>   m_Materials;          // 재질 목록
    vector<CBone*>       m_Bones;             // 본 계층
    vector<CAnimation*>  m_Animations;        // 애니메이션 목록

    // 애니메이션 상태
    _uint  m_iCurrentAnimIndex;
    _bool  m_isAnimLoop;
    _bool  m_isAnimFinished;
};
```

### MODELTYPE enum

```cpp
enum class MODELTYPE { ANIM, NONANIM, END };
```

| 타입 | 용도 | 특징 |
|------|------|------|
| `ANIM` | 스켈레탈 모델 (Fiona) | 본 계층 + 애니메이션 유지 |
| `NONANIM` | 스태틱 모델 (ForkLift) | aiProcess_PreTransformVertices로 평탄화 |

---

## 2. 로딩 흐름 (Initialize_Prototype)

```
CModel::Create(pDevice, pContext, eType, filePath, PreTransformMatrix)
  └── Initialize_Prototype()
        ├── ① Assimp::ReadFile     → aiScene 로드
        ├── ② Ready_Bones          → 본 계층 구축 (재귀)
        ├── ③ Ready_Meshes         → 메시별 VB/IB 생성
        ├── ④ Ready_Materials      → 텍스처 로드
        └── ⑤ Ready_Animations     → 키프레임 파싱
```

### ① Assimp ReadFile

```cpp
unsigned int iFlag = aiProcess_ConvertToLeftHanded
                   | aiProcessPreset_TargetRealtime_Fast;

if (MODELTYPE::NONANIM == m_eType)
    iFlag |= aiProcess_PreTransformVertices;  // 스태틱: 노드 변환을 정점에 적용

m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
```

**주요 플래그:**

| 플래그 | 효과 |
|--------|------|
| `aiProcess_ConvertToLeftHanded` | 오른손 좌표계 → 왼손 좌표계 (DX용) |
| `aiProcessPreset_TargetRealtime_Fast` | 삼각화, 노멀 생성 등 기본 처리 |
| `aiProcess_PreTransformVertices` | 모든 노드 변환을 정점에 사전 적용 |

**PreTransformVertices의 의미**: 스태틱 모델은 본이 필요 없으므로
계층 변환을 정점에 미리 적용(bake)한다. 런타임에 본 계산이 불필요해져 성능이 향상된다.

### PreTransformMatrix — 사전 변환

```cpp
// Loader.cpp에서:
// Fiona: Y축 180도 회전 (모델이 반대를 보고 있으므로)
PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

// ForkLift: 0.01배 스케일 + Y축 180도 (너무 크므로)
PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)
                   * XMMatrixRotationY(XMConvertToRadians(180.0f));
```

이 행렬은 **모델 좌표계를 게임 좌표계로 맞추는 보정 행렬**이다.
DCC 도구(3ds Max, Maya)마다 좌표 규약이 다르므로 로드 시 보정한다.

---

## 3. Ready_Bones — 본 계층 구축

```cpp
HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    // 현재 노드로 본 생성
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    m_Bones.push_back(pBone);

    // 현재 본의 인덱스 = 벡터의 마지막 인덱스
    _int iPIndex = m_Bones.size() - 1;

    // 자식 노드를 재귀적으로 처리
    for (_uint i = 0; i < pAINode->mNumChildren; ++i)
        Ready_Bones(pAINode->mChildren[i], iPIndex);

    return S_OK;
}
```

**재귀 순회 결과**: 깊이 우선(DFS) 순서로 m_Bones 벡터에 저장된다.
부모 인덱스(`iParentIndex`)는 벡터 내 부모의 위치이다.

```
aiScene 노드 트리:                m_Bones 벡터:
    Root                          [0] Root (parent: -1)
    ├── Spine                     [1] Spine (parent: 0)
    │   ├── Arm_L                 [2] Arm_L (parent: 1)
    │   └── Arm_R                 [3] Arm_R (parent: 1)
    └── Hip                       [4] Hip (parent: 0)
        ├── Leg_L                 [5] Leg_L (parent: 4)
        └── Leg_R                 [6] Leg_R (parent: 4)
```

---

## 4. Ready_Materials — 텍스처 로드

### CMaterial 구조

```cpp
class CMaterial : public CBase {
    // AI_TEXTURE_TYPE_MAX 종류별 텍스처 SRV 배열
    vector<ID3D11ShaderResourceView*> m_Textures[AI_TEXTURE_TYPE_MAX];
};
```

### 텍스처 경로 재구성

```cpp
HRESULT CMaterial::Intialize(const _char* pModelFilePath,
    const aiMaterial* pAIMaterial)
{
    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        _uint iNumTextures = pAIMaterial->GetTextureCount(
            static_cast<aiTextureType>(i));

        for (size_t j = 0; j < iNumTextures; j++)
        {
            aiString strTextureFilePath;
            pAIMaterial->GetTexture(
                static_cast<aiTextureType>(i), j, &strTextureFilePath);

            // 모델 경로에서 드라이브+디렉토리 추출
            _splitpath_s(pModelFilePath, szDrive, szDir, nullptr, nullptr);
            // 텍스처 경로에서 파일명+확장자 추출
            _splitpath_s(strTextureFilePath.data, nullptr, nullptr,
                szFileName, szExt);

            // 모델 디렉토리 + 텍스처 파일명으로 전체 경로 조합
            sprintf(szFullPath, "%s%s%s%s", szDrive, szDir, szFileName, szExt);
        }
    }
}
```

**왜 경로를 재구성하는가?**
FBX 파일에 저장된 텍스처 경로는 원본 PC의 **절대 경로**인 경우가 많다.
모델 파일의 디렉토리 + 텍스처 파일명으로 재구성하면 상대 경로처럼 동작한다.

### 확장자별 로드 함수

```cpp
if (strcmp(".dds", szExt) == 0)
    DirectX::CreateDDSTextureFromFile(m_pDevice, szWFullPath, nullptr, &pSRV);
else if (strcmp(".tga", szExt) == 0)
    hr = E_FAIL;  // TGA 미지원
else
    DirectX::CreateWICTextureFromFile(m_pDevice, szWFullPath, nullptr, &pSRV);
```

| 확장자 | 로드 함수 | 라이브러리 |
|--------|----------|-----------|
| `.dds` | CreateDDSTextureFromFile | DirectXTK |
| `.png/.jpg` | CreateWICTextureFromFile | DirectXTK (WIC) |
| `.tga` | 미지원 | - |

### aiTextureType 종류

| 타입 | 용도 |
|------|------|
| `aiTextureType_DIFFUSE` | 기본 색상 맵 |
| `aiTextureType_NORMALS` | 노멀 맵 |
| `aiTextureType_SPECULAR` | 스펙큘러 맵 |
| 기타 | Emissive, Height, Opacity 등 |

---

## 5. Clone 시 리소스 관리

### 복사 생성자 — 핵심 분석

```cpp
CModel::CModel(const CModel& Prototype)
    : CComponent{ Prototype }
    , m_iNumMeshes { Prototype.m_iNumMeshes }
    , m_Meshes { Prototype.m_Meshes }            // Mesh 벡터 복사 (포인터 공유)
    , m_iNumMaterials { Prototype.m_iNumMaterials }
    , m_Materials { Prototype.m_Materials }        // Material 벡터 복사 (포인터 공유)
    , m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
    , m_iNumAnimations { Prototype.m_iNumAnimations }
{
    // Animation: 딥 카피 (각 인스턴스가 독립적 재생 상태)
    for (auto& pPrototypeAnim : Prototype.m_Animations)
        m_Animations.push_back(pPrototypeAnim->Clone());

    // Bone: 딥 카피 (각 인스턴스가 독립적 본 행렬)
    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    // Mesh: 얕은 복사 (AddRef)
    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    // Material: 얕은 복사 (AddRef)
    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);
}
```

### 공유 vs 딥 카피 판단 기준

| 리소스 | 복사 방식 | 이유 |
|--------|----------|------|
| **Mesh** | 얕은 복사 (AddRef) | GPU 버퍼는 불변 → 공유 가능 |
| **Material** | 얕은 복사 (AddRef) | 텍스처 SRV는 불변 → 공유 가능 |
| **Bone** | 딥 카피 (Clone) | 각 인스턴스마다 다른 본 포즈 필요 |
| **Animation** | 딥 카피 (Clone) | 각 인스턴스마다 다른 재생 진행도 필요 |

**핵심 원칙**: **상태를 가지는 것은 딥 카피, 불변 데이터는 공유**

---

## 6. 렌더링 흐름

### 모델 렌더 패턴

```cpp
// Body.cpp (스켈레탈 메시 렌더 예시)
void CBody::Render()
{
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // ① 본 행렬 배열 바인딩 (스키닝용)
        m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices");

        // ② 메시의 재질에서 텍스처 바인딩
        m_pModelCom->Bind_ShaderResource(i, m_pShaderCom,
            "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_ShaderResource(i, m_pShaderCom,
            "g_NormalTexture", aiTextureType_NORMALS, 0);

        // ③ 셰이더 패스 실행
        m_pShaderCom->Begin(0);

        // ④ 메시의 VB/IB 바인딩 + 드로우
        m_pModelCom->Render(i);
    }
}
```

### Bind_ShaderResource — 메시→재질 연결

```cpp
HRESULT CModel::Bind_ShaderResource(_uint iMeshIndex,
    CShader* pShader, const _char* pConstantName,
    aiTextureType eType, _uint iIndex)
{
    // 메시가 참조하는 재질 인덱스 획득
    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    // 해당 재질의 텍스처를 셰이더에 바인딩
    return m_Materials[iMaterialIndex]->Bind_ShaderResource(
        pShader, pConstantName, eType, iIndex);
}
```

```
Mesh[0] → MaterialIndex=0 → Material[0] → Textures[DIFFUSE][0] → SRV
Mesh[1] → MaterialIndex=1 → Material[1] → Textures[DIFFUSE][0] → SRV
```

---

## 7. Play_Animation — 애니메이션 재생

```cpp
void CModel::Play_Animation(_float fTimeDelta)
{
    // ① 현재 애니메이션으로 각 채널의 TransformationMatrix 갱신
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(
        fTimeDelta, m_Bones, m_isAnimLoop, &m_isAnimFinished);

    // ② 갱신된 TransformationMatrix로 CombinedMatrix 계산
    for (auto& pBone : m_Bones)
        pBone->Update_CombinedTransformationMatrix(
            m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}
```

**2단계 분리:**
1. Animation이 본의 로컬 변환(TransformationMatrix)을 키프레임 보간으로 갱신
2. 각 본이 부모를 따라가며 CombinedTransformationMatrix를 누적 계산

(상세는 O06-A, O06-B 문서에서 분석)

---

## 8. 소켓 본 시스템

```cpp
const _float4x4* CModel::Get_SocketBoneMatrix_Ptr(
    const _char* pBoneName) const
{
    auto iter = find_if(m_Bones.begin(), m_Bones.end(),
        [&](CBone* pBone) {
            return pBone->Compare_Name(pBoneName);
        });

    if (iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrix_Ptr();
}
```

무기(Weapon)를 캐릭터의 손에 부착할 때 사용한다.
특정 본의 CombinedMatrix **포인터**를 반환하여
Part 오브젝트가 매 프레임 자동으로 최신 행렬을 참조한다.

---

## 9. 핵심 정리

| 항목 | 설명 |
|------|------|
| **Assimp** | FBX/OBJ → aiScene 변환, ConvertToLeftHanded 필수 |
| **ANIM vs NONANIM** | 스켈레탈은 본 유지, 스태틱은 PreTransformVertices로 평탄화 |
| **PreTransformMatrix** | DCC 좌표계 → 게임 좌표계 보정 (스케일, 회전) |
| **텍스처 경로** | 모델 디렉토리 + 파일명으로 재구성 (절대 경로 회피) |
| **Clone 전략** | Mesh/Material=공유(AddRef), Bone/Animation=딥카피(Clone) |
| **메시별 렌더** | 각 메시마다 본 행렬 + 텍스처 + 패스 + 드로우 |
| **소켓 본** | 이름으로 본을 찾아 CombinedMatrix 포인터 반환 |

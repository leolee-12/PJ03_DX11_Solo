# O06-A: Bone 계층 구축 + Combined 행렬

## 1. Assimp의 본 관련 타입

| Assimp 타입 | 역할 | 엔진 대응 |
|-------------|------|----------|
| `aiNode` | 본(노드) 트리 구조 | `CBone` |
| `aiBone` | 메시에 영향 주는 본 (가중치) | `CMesh`에서 사용 |
| `aiNodeAnim` | 특정 애니메이션의 키프레임 | `CChannel` |

**핵심**: 세 타입 모두 **본 이름(문자열)**으로 연결된다.

---

## 2. CBone 클래스 구조

```cpp
class CBone final : public CBase {
    _char     m_szName[MAX_PATH];                  // 본 이름
    _float4x4 m_TransformationMatrix;              // 로컬 변환 (부모 기준)
    _float4x4 m_CombinedTransformationMatrix;      // 누적 변환 (루트 기준)
    _int      m_iParentBoneIndex;                  // 부모 인덱스 (-1 = 루트)
};
```

### 두 행렬의 관계

```
TransformationMatrix     = 부모 → 자신 (로컬 변환)
CombinedTransformationMatrix = 루트 → 자신 (누적 변환)

Combined = Local × Parent.Combined
```

---

## 3. Ready_Bones — DFS 순회로 플랫 배열 구축

### Assimp 트리 구조 (입력)

```
RootNode
  ├── Armature
  │     ├── Hips
  │     │     ├── Spine
  │     │     │     └── Chest
  │     │     ├── Left_UpperLeg
  │     │     └── Right_UpperLeg
  │     ...
```

### DFS 순회 코드

```cpp
HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    // ① 현재 노드로 CBone 생성
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    m_Bones.push_back(pBone);

    // ② 자신의 인덱스 = 현재 배열 크기 - 1
    _int iPIndex = m_Bones.size() - 1;

    // ③ 자식 노드들 재귀 (자신을 부모로 전달)
    for (_uint i = 0; i < pAINode->mNumChildren; ++i)
        Ready_Bones(pAINode->mChildren[i], iPIndex);

    return S_OK;
}
```

### 결과: 플랫 배열 (DFS 순서)

```
인덱스:  0        1         2       3       4         5              6
이름:   RootNode  Armature  Hips    Spine   Chest     Left_UpperLeg  Right_UpperLeg
부모:   -1        0         1       2       3         2              2
```

**DFS 순서의 장점**: 부모가 항상 자식보다 앞에 위치.
배열을 순서대로 순회하면 Combined 행렬을 한 번에 계산할 수 있다.

---

## 4. CBone 초기화

```cpp
HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{
    m_iParentBoneIndex = iParentIndex;

    // 본 이름 복사
    strcpy_s(m_szName, pAINode->mName.data);

    // Assimp의 로컬 변환 행렬 복사
    memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

    // Assimp은 행 기준(row-major) → DirectX도 행 기준이지만
    // Assimp은 전치된 형태로 저장하므로 Transpose 필요
    XMStoreFloat4x4(&m_TransformationMatrix,
        XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    // Combined 행렬은 단위 행렬로 초기화
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
}
```

### 왜 Transpose인가?

Assimp은 **열 기준(column-major)** 행렬을 사용한다:
```
Assimp (column-major):    DirectXMath (row-major):
[m00 m10 m20 m30]         [m00 m01 m02 m03]
[m01 m11 m21 m31]   →     [m10 m11 m12 m13]
[m02 m12 m22 m32]         [m20 m21 m22 m23]
[m03 m13 m23 m33]         [m30 m31 m32 m33]
```

`memcpy`로 바이트를 그대로 복사하면 전치된 상태이므로 `XMMatrixTranspose`로 보정한다.

---

## 5. Update_CombinedTransformationMatrix

```cpp
void CBone::Update_CombinedTransformationMatrix(
    const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    if (-1 == m_iParentBoneIndex)
    {
        // 루트 본: Local × PreTransformMatrix
        Combined = Local × PreTransform;
    }
    else
    {
        // 자식 본: Local × 부모.Combined
        Combined = Local × Parent.Combined;
    }
}
```

### PreTransformMatrix란?

```cpp
// CModel::Play_Animation에서 호출
for (auto& pBone : m_Bones)
    pBone->Update_CombinedTransformationMatrix(m_Bones,
        XMLoadFloat4x4(&m_PreTransformMatrix));
```

`m_PreTransformMatrix`는 좌표계 보정 행렬이다:
- FBX → DX: Y-Up 변환, 스케일 조정 등
- 모든 본의 최종 좌표에 일괄 적용

### Combined 행렬 계산 순서

```
배열 순회 (인덱스 0 → N-1):

[0] RootNode:  Combined = Local × PreTransform
[1] Armature:  Combined = Local × Bones[0].Combined  ← 부모가 이미 계산됨
[2] Hips:      Combined = Local × Bones[1].Combined
[3] Spine:     Combined = Local × Bones[2].Combined
[4] Chest:     Combined = Local × Bones[3].Combined
...
```

**DFS 순서이므로 부모가 항상 먼저 계산된다** — 별도의 위상 정렬 불필요.

---

## 6. 애니메이션 적용 흐름

```
Play_Animation(fTimeDelta)
  │
  ├── ① Animation::Update_TransformationMatrix(fTimeDelta, Bones, ...)
  │       └── 각 Channel이 해당 Bone의 m_TransformationMatrix를 갱신
  │           (키프레임 보간 결과로 Local 행렬 교체)
  │
  └── ② for (pBone : m_Bones)
          pBone->Update_CombinedTransformationMatrix(Bones, PreTransform)
              └── Combined = (갱신된 Local) × Parent.Combined
```

**1단계**: 애니메이션이 각 본의 **로컬 변환**을 키프레임에 맞게 수정.
**2단계**: 수정된 로컬 변환을 **루트까지 누적**하여 Combined 생성.

---

## 7. Clone 패턴

```cpp
CBone* CBone::Clone() {
    return new CBone(*this);  // 기본 복사 생성자 (멤버별 복사)
}
```

**Bone은 딥카피**: 각 오브젝트 인스턴스가 독립적인 본 상태를 가져야 한다.
같은 캐릭터 모델이라도 각자 다른 애니메이션 포즈를 취할 수 있어야 하므로,
TransformationMatrix와 CombinedTransformationMatrix가 독립이어야 한다.

```
프로토타입:                클론 A (걷기):          클론 B (달리기):
Bone[0] "Hips"            Bone[0] "Hips"          Bone[0] "Hips"
  Local = 초기값            Local = 걷기 포즈         Local = 달리기 포즈
  Combined = 초기값         Combined = 걷기 결과      Combined = 달리기 결과
```

---

## 8. 본 이름 검색

```cpp
_bool Compare_Name(const _char* pName) {
    return !strcmp(pName, m_szName);
}
```

**선형 검색**: 본 배열을 순회하며 이름 비교. 사용처:
- `CChannel`이 자신이 제어할 본을 찾을 때
- `CMesh`가 스키닝 본을 찾을 때
- 소켓 본 검색 (무기 부착점 등)

---

## 9. 핵심 정리

| 항목 | 설명 |
|------|------|
| **데이터 소스** | aiNode 트리 (Assimp) |
| **저장 구조** | DFS 순서 플랫 배열, 부모 인덱스로 트리 표현 |
| **로컬 행렬** | aiNode.mTransformation → Transpose 보정 |
| **Combined** | Local × Parent.Combined (루트는 × PreTransform) |
| **계산 순서** | 배열 순차 순회 (DFS로 부모 선행 보장) |
| **애니메이션** | Channel이 Local 수정 → Combined 재계산 |
| **Clone** | 딥카피 (인스턴스별 독립 포즈) |
| **이름 매칭** | strcmp로 Bone/Channel/aiBone 연결 |

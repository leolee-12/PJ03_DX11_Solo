# 애니메이션 시스템 심화 분석

> 참고프로젝트2 — Dragon Ball FighterZ 모작
> 분석 대상: CModel, CAnimation, CChannel, CBone, CMesh (Engine DLL)

---

## 1. 핵심 책임과 경계

| 클래스 | 책임 | 위치 |
|--------|------|------|
| `CModel` | 모델 컴포넌트. 메시/본/애니메이션/머티리얼 통합 관리. 애니메이션 재생 오케스트레이션 | Engine (CComponent 상속) |
| `CAnimation` | 단일 애니메이션 클립. 채널 기반 재생 + 사운드 이벤트 | Engine (CBase 상속) |
| `CChannel` | 단일 본의 키프레임 시퀀스. 보간 연산 담당 | Engine (CBase 상속) |
| `CBone` | 본 계층 노드. TransformationMatrix (로컬) + CombinedMatrix (월드) | Engine (CBase 상속) |
| `CMesh` | 메시 VB/IB. 본 인덱스 + 오프셋 행렬로 스키닝 데이터 보유 | Engine (CVIBuffer 상속) |

### 시스템 경계

```
[Client]                        [Engine]
CCharacter ──uses──> CModel ──owns──> CAnimation[]
 │                     │                └──> CChannel[]
 │ Play_Animation()    │──owns──> CBone[] (Clone)
 │ SetUp_Animation()   │──owns──> CMesh[] (AddRef)
 │                     │──owns──> MESHMATERIAL[]
 └─ Bind_BoneMatrices──┘
      ↓
   CShader → GPU (g_BoneMatrices)
```

- **Engine은 애니메이션 재생/보간만** 담당
- **Client는 전투 로직에서 전환/속도/정지 제어** (SetUp_Animation, AnimationLock 등)
- **셰이더 바인딩**이 Engine과 GPU의 경계

---

## 2. 클래스 간 소유/참조 관계

```
CModel (CComponent → CBase, RefCounting)
 │
 ├─ vector<CBone*>        m_Bones         [소유, Clone]
 │   └─ 각 CBone은 m_iParentIndex로 부모 참조 (인덱스 기반)
 │
 ├─ vector<CMesh*>        m_Meshes        [공유, AddRef]
 │   ├─ vector<_uint>      m_BoneIndices   (본 배열 인덱스)
 │   └─ vector<_float4x4>  m_OffsetMatrices (바인드포즈 역행렬)
 │
 ├─ vector<CAnimation*>  m_Animations    [공유, AddRef]
 │   ├─ vector<CChannel*>  m_Channels     [소유]
 │   │   └─ m_iBoneIndex (본 배열 인덱스)
 │   │   └─ vector<KEYFRAME> m_KeyFrames
 │   └─ vector<SoundEvent>  m_SoundEvents
 │
 ├─ vector<MESHMATERIAL>  m_Materials     [공유, AddRef per texture]
 │
 └─ vector<vector<_uint>> m_KeyFrameIndices  [값, 채널별 현재 키프레임 인덱스]
```

### Clone 시 복제 전략

| 리소스 | Clone 시 동작 | 이유 |
|--------|--------------|------|
| `CBone` | **Deep Clone** (`pBone->Clone()`) | 인스턴스마다 독립적인 본 상태(TransformationMatrix) 필요 |
| `CMesh` | **Shallow (AddRef)** | VB/IB는 GPU 리소스 공유 가능 |
| `CAnimation` | **Shallow (AddRef)** | 키프레임 데이터는 읽기 전용 |
| `MESHMATERIAL` | **Shallow (AddRef per texture)** | 텍스처 SRV 공유 |
| `m_KeyFrameIndices` | **값 복사** | 인스턴스별 재생 위치 추적 필요 |

이 설계의 핵심: **본만 Deep Clone하고 나머지는 공유**. 본은 애니메이션 재생 시 매 프레임 TransformationMatrix가 변경되므로 인스턴스별 독립 필요.

---

## 3. 데이터 구조

### KEYFRAME (키프레임 단위)

```cpp
struct KEYFRAME {
    XMFLOAT3  vScale;      // 스케일
    XMFLOAT4  vRotation;   // 회전 (쿼터니언)
    XMFLOAT3  vPosition;   // 위치
    float     fTime;       // 이 키프레임의 시간 (TickPerSecond 기준)
};
```

### 바이너리 모델 파일 구조

```
ModelHeader { isAnim, numMeshes, numMaterials, numAnimations }
├─ [isAnim] Bones (재귀 계층)
│   └─ nameLength + name + XMFLOAT4X4 + numChildren + children...
├─ Meshes × numMeshes
│   └─ name + vertexCount + indexCount + vertices + indices + materialIndex
│   └─ [isAnim] bones + offsetMatrices
├─ Materials × numMaterials
│   └─ texturePaths[AI_TEXTURE_TYPE_MAX]
└─ [isAnim] Animations × numAnimations
    └─ name + duration + ticksPerSecond
    └─ Channels × numChannels
        └─ name + keyframes[]
```

오프라인 도구(AssimpSaveAndLoad)가 FBX → 이 바이너리 포맷으로 변환.

---

## 4. 한 프레임 기준 호출 흐름

### A. 애니메이션 재생 (Play_Animation)

```
CModel::Play_Animation(dt)
│
├─ 1. CAnimation::Update_TransformationMatrix(&pos, dt, bones, isLoop, keyFrameIndices)
│     ├─ pos += tickPerSecond × dt          (시간 전진)
│     ├─ if pos >= duration → bEnd=true     (루프면 pos=0)
│     └─ for each Channel:
│         └─ CChannel::Compute_TransformationMatrix(pos, bones, &keyFrameIndex)
│             ├─ while pos >= keyFrames[idx+1].fTime → idx++   (이진검색 아닌 순차)
│             ├─ ratio = (pos - keys[idx].fTime) / (keys[idx+1].fTime - keys[idx].fTime)
│             ├─ Scale    = Lerp(src.Scale, dst.Scale, ratio)
│             ├─ Rotation = Slerp(src.Rotation, dst.Rotation, ratio)   ★ 쿼터니언
│             ├─ Position = Lerp(src.Position, dst.Position, ratio)
│             └─ bones[boneIndex]->SetUp_TransformationMatrix(S×R×T)
│
├─ 2. [블렌딩 활성 시] 블렌드 처리
│     ├─ blendFactor = blendTime / blendDuration
│     ├─ if factor >= 1.0 → 블렌딩 완료, 다음 애니메이션으로 전환
│     └─ for each Bone:
│         ├─ currentMatrix = bone->TransformationMatrix (현재 애니메이션 결과)
│         ├─ nextMatrix = Animation[next]->Compute_FirstKeyFrameMatrixForBone()
│         ├─ Decompose → Lerp(Scale), Slerp(Rotation), Lerp(Position)
│         └─ bone->SetUp(blendedMatrix)
│
└─ 3. for each Bone:
      └─ CBone::Update_CombinedTransformationMatrix(bones, preTransformMatrix)
          ├─ if root: Combined = PreTransform × Transformation
          └─ else:    Combined = Transformation × parent->Combined
```

### B. 고정 타임스텝 변형 (Play_Animation_Lick)

```
Play_Animation_Lick(dt)
│
├─ accTime += dt
└─ while accTime > maxUpdateTime (기본 0.1초):
    ├─ Update_TransformationMatrix(maxUpdateTime)   ← 고정 스텝
    ├─ Update_CombinedTransformationMatrix()
    └─ accTime -= maxUpdateTime
```

**목적:** 가변 dt로 인한 키프레임 건너뛰기 방지. 격투게임에서 프레임 일관성 보장.

### C. 셰이더 바인딩

```
Render 시점:
CModel::Bind_BoneMatrices(shader, "g_BoneMatrices", meshIndex)
│
├─ _float4x4 BoneMatrices[800]                          ← 800본 상한
├─ CMesh::SetUp_Matrices(BoneMatrices, bones)
│   └─ for each bone in mesh:
│       BoneMatrices[i] = OffsetMatrix[i] × bones[boneIndex[i]]->CombinedMatrix
└─ CShader::Bind_Matrices("g_BoneMatrices", BoneMatrices, 800)
    └─ ID3DX11EffectMatrixVariable::SetMatrixArray()     ← GPU 전송
```

**최종 스키닝 공식 (셰이더):**

```
VertexWorldPos = Σ(weight[i] × BoneMatrix[boneIndex[i]] × localPos)
where BoneMatrix = OffsetMatrix × CombinedTransformMatrix
```

---

## 5. 핵심 메커니즘 상세

### 5.1 애니메이션 전환 (SetUp_Animation)

```cpp
SetUp_Animation(animIndex, isLoop, blendDuration=0)
│
├─ 다른 애니메이션일 때:
│   ├─ pos=0, tickPerSecond=25(기본값 리셋)
│   ├─ blendDuration > 0:
│   │   ├─ m_isBlending = true
│   │   ├─ m_iNextAnimationIndex = animIndex
│   │   ├─ 다음 애니메이션 첫 키프레임 계산
│   │   └─ Play_Animation에서 매 프레임 Lerp/Slerp
│   └─ blendDuration == 0:
│       └─ 즉시 전환 (현재 pos=0에서 시작)
│
└─ 같은 애니메이션: 루프 설정만 변경
```

### 5.2 블렌딩 처리

```
매 프레임 Play_Animation 내부:
  blendFactor = blendTime / blendDuration  (0→1 선형)

  for each bone:
    current = 현재 애니메이션 결과 TransformationMatrix
    next    = 다음 애니메이션 첫 키프레임 행렬

    Decompose(current) → curScale, curRot, curTrans
    Decompose(next)    → nextScale, nextRot, nextTrans

    blendedScale = Lerp(curScale, nextScale, factor)
    blendedRot   = Slerp(curRot, nextRot, factor)     ← 쿼터니언 구면보간
    blendedTrans = Lerp(curTrans, nextTrans, factor)

    ★ DummyBone → Scale 보간 건너뛰기 (장비 메시 흔들림 방지)

    bone->SetUp(AffineTransformation(S, R, T))
```

### 5.3 프레임 점프 (CurrentAnimationPositionJump)

```cpp
void CurrentAnimationPositionJump(_float fPosition) {
    m_fCurrentAnimPosition = fPosition;   // 직접 시간 설정
    Update_FrameIndex();                  // 키프레임 인덱스 재계산
    Play_Animation(0);                    // dt=0으로 본 행렬 갱신
}
```

격투게임에서 피격/가드 시 특정 프레임으로 즉시 이동할 때 사용.

### 5.4 Root Motion 제거 (Play_Animation_Lick2)

```
Play_Animation_Lick2(dt, pTransform)
│
├─ m_bNoMoveXZ == true:
│   ├─ "G_root" 본을 찾으면 CombinedMatrix 갱신 건너뜀
│   └─ 나머지 본은 정상 갱신
│
└─ m_bNoMoveXZ == false:
    └─ 모든 본 정상 갱신
```

루트 본의 이동을 무시하여 캐릭터 위치를 게임 로직(CTransform)으로만 제어.

### 5.5 본 계층 갱신 (Update_CombinedTransformationMatrix)

```
root (parentIndex == -1):
  Combined = PreTransformMatrix × Transformation

child:
  Combined = Transformation × parent->Combined
```

**PreTransformMatrix:** 모델 임포트 시 좌표계/스케일 변환 행렬 (한 번만 설정).
본 배열은 부모가 자식보다 앞에 오도록 정렬되어, 단일 순차 순회로 전체 계층 갱신 가능.

---

## 6. 바이너리 로드 파이프라인

### 전체 흐름

```
CModel::Initialize_Prototype(filePath)
│
├─ ".bin" 확인 → InitializeFromBinary()
│
├─ 1. ModelHeader 읽기 (isAnim, numMeshes, numMaterials, numAnimations)
│
├─ 2. [isAnim] 본 계층 로드 (재귀)
│     ReadBonesRecursive(numBones, parentIndex=-1)
│     └─ nameLength → name → transformationMatrix → numChildren → 재귀
│     └─ CBone::Create(name, parentIndex, matrix)
│        └─ matrix 전치 (파일은 행전치 상태로 저장)
│
├─ 3. 메시 로드 × numMeshes
│     ├─ name, vertexCount, indexCount
│     ├─ [isAnim] VTXANIMMESH[] / [noAnim] VTXMESH[]
│     ├─ indices[]
│     ├─ materialIndex
│     ├─ [isAnim] bones + offsetMatrices
│     └─ CMesh::Create() → VB/IB 생성 (D3D11_USAGE_DEFAULT)
│
├─ 4. 머티리얼 로드 × numMaterials
│     └─ texturePath[AI_TEXTURE_TYPE_MAX] → CTexture::Create()
│
└─ 5. [isAnim] 애니메이션 로드 × numAnimations
      ├─ name, duration, ticksPerSecond
      ├─ Channels × numChannels
      │   └─ name + KEYFRAME[]
      └─ CAnimation::Create(animData, bones, keyFrameIndices)
          └─ CChannel::Create(channelData, bones)
              └─ 이름으로 본 인덱스 매칭 (find_if)
```

---

## 7. 사용된 디자인 패턴

### 1) 프로토타입-클론 (CModel : CComponent)

```
Prototype                          Clone (인스턴스)
CModel                             CModel (copy ctor)
 ├─ CBone[] (원본)                  ├─ CBone[] (Deep Clone)
 ├─ CMesh[] (원본)                  ├─ CMesh[] (AddRef)
 ├─ CAnimation[] (원본)            ├─ CAnimation[] (AddRef)
 └─ m_KeyFrameIndices (원본)       └─ m_KeyFrameIndices (값 복사)
```

- **공유/독립 분리가 핵심**: GPU 리소스(메시/텍스처)는 공유, CPU 상태(본 행렬/키프레임 인덱스)는 독립
- Clone 시 `new CModel(Prototype)` → 복사 생성자에서 선택적 Deep/Shallow 수행

### 2) 컴포지트 패턴 (본 계층)

- CBone은 `m_iParentIndex`로 트리 구조 형성
- 인덱스 기반 참조 (포인터 대신) → Clone 시 배열 복사만으로 관계 유지
- 단일 배열 순차 순회로 전체 계층 갱신 (부모-우선 정렬 전제)

### 3) 전략 패턴 (재생 모드)

| 함수 | 전략 | 용도 |
|------|------|------|
| `Play_Animation` | 가변 dt + 블렌딩 | 일반 재생 |
| `Play_Animation_Lick` | 고정 타임스텝 | 격투 프레임 일관성 |
| `Play_Animation_Lick2` | 고정 스텝 + 루트모션 제거 | 루트본 위치 무시 |

### 4) 매개 변수 객체 (KEYFRAME, AnimationData 등)

바이너리 직렬화/역직렬화를 위한 POD 구조체 그룹.

---

## 8. DirectX API 호출 지점과 래핑

| 래핑 함수 | DX API | 위치 |
|-----------|--------|------|
| `CMesh::Create_Buffer(&m_pVB)` | `ID3D11Device::CreateBuffer` (VB) | Mesh.cpp — VB 생성 |
| `CMesh::Create_Buffer(&m_pIB)` | `ID3D11Device::CreateBuffer` (IB) | Mesh.cpp — IB 생성 |
| `CVIBuffer::Bind_Buffers()` | `ID3D11DeviceContext::IASetVertexBuffers / IASetIndexBuffer` | 렌더 시 |
| `CVIBuffer::Render()` | `ID3D11DeviceContext::DrawIndexed` | 렌더 시 |
| `CShader::Bind_Matrices()` | `ID3DX11EffectMatrixVariable::SetMatrixArray` | 본 행렬 GPU 전송 |
| `CTexture::Bind_ShaderResource()` | `ID3DX11EffectShaderResourceVariable::SetResource` | 머티리얼 텍스처 바인딩 |

### 본 행렬 전송 경로

```
CBone::m_CombinedTransformationMatrix  (CPU, per-bone)
    ↓ SetUp_Matrices
_float4x4 BoneMatrices[800]            (CPU, per-mesh 합산)
    ↓ CShader::Bind_Matrices
ID3DX11EffectMatrixVariable            (GPU, cbuffer)
    ↓ Vertex Shader
스키닝 연산                            (GPU)
```

- 본 행렬 상한: **800개** (배열 고정 크기)
- Effects11 프레임워크의 `SetMatrixArray`로 한 번에 전송
- 메시별로 해당 메시가 사용하는 본만 `OffsetMatrix × CombinedMatrix`로 합산

---

## 9. 설계 판단과 채택 가치

### 1) 본만 Deep Clone, 나머지 공유

**구조:** CBone만 Clone(), CMesh/CAnimation은 AddRef
**장점:** 같은 모델의 N개 인스턴스가 GPU 리소스(VB/IB/텍스처)를 공유하면서 각자 독립적인 애니메이션 상태 유지
**채택 가치:** ★★★ — 메모리 효율과 독립성의 최적 균형

### 2) 인덱스 기반 본 계층

**구조:** `m_iParentIndex`로 부모 참조 (포인터 대신 정수 인덱스)
**장점:**
- Clone 시 배열 복사만으로 계층 관계 자동 유지 (포인터 재매핑 불필요)
- 캐시 친화적 순차 순회 가능
- 직렬화/역직렬화 간단
**채택 가치:** ★★★ — 본 계층 관리의 모범 패턴

### 3) 고정 타임스텝 애니메이션 (Play_Animation_Lick)

**구조:** 가변 dt를 고정 스텝(0.1초)으로 분할하여 while 루프로 반복 갱신
**장점:** 프레임 드롭 시에도 키프레임을 건너뛰지 않아 격투게임의 프레임 정밀한 판정 보장
**주의:** 극단적 dt(>1초) 방어 코드 있음 → accTime을 0.1로 제한
**채택 가치:** ★★★ — 격투/액션 게임에서 필수적인 프레임 일관성 보장

### 4) 바이너리 모델 포맷

**구조:** Assimp FBX → 커스텀 바이너리(.bin) → 런타임 로드
**장점:** Assimp 의존성 제거 (런타임에 불필요), 로드 속도 향상 (파싱 없이 memcpy 수준)
**채택 가치:** ★★★ — 상용 엔진의 표준 접근법

### 5) 채널-본 인덱스 매칭 (이름 기반)

**구조:** CChannel 생성 시 이름으로 본 배열에서 인덱스 검색 → m_iBoneIndex 저장
**장점:** 런타임 재생 시 이름 검색 없이 인덱스 직접 접근 (O(1))
**단점:** 초기화 시 O(N×M) 검색 (N=채널수, M=본수) — 바이너리 저장 시 인덱스 선계산으로 개선 가능
**채택 가치:** ★★☆ — 방향은 맞으나 초기화 비용 최적화 여지 있음

### 6) DummyBone 스케일 블렌딩 예외

**구조:** 본이 없는 메시 → 메시 이름으로 대리 본 생성 → isDummyBone=true → 블렌딩 시 스케일 보간 스킵
**장점:** 장비/무기 등 본 가중치 없는 메시가 애니메이션 전환 시 스케일 튐 방지
**채택 가치:** ★★☆ — 실용적 해결책이나 에지 케이스

### 7) 키프레임 인덱스 외부 관리

**구조:** `m_KeyFrameIndices`를 CModel이 보유하고 CAnimation에 참조 전달
**장점:** CAnimation은 상태 없는 데이터(공유 가능) 유지. 재생 상태는 CModel(Clone된 인스턴스)이 관리
**채택 가치:** ★★★ — 데이터/상태 분리의 좋은 사례

### 8) 프레임 점프 (CurrentAnimationPositionJump)

**구조:** 시간 직접 설정 → 키프레임 인덱스 재계산 → dt=0으로 행렬 갱신
**장점:** 격투게임에서 피격/가드/체이스 시 특정 프레임으로 즉시 이동 필요
**채택 가치:** ★★☆ — 격투게임 특화 기능

---

## 부록: 본 행렬 변환 체인 요약

```
[애니메이션 키프레임]
    ↓ CChannel::Compute_TransformationMatrix (Lerp/Slerp 보간)
CBone::m_TransformationMatrix (로컬 행렬)
    ↓ CBone::Update_CombinedTransformationMatrix (부모 계층 합성)
CBone::m_CombinedTransformationMatrix (월드 행렬)
    ↓ CMesh::SetUp_Matrices (오프셋 행렬 적용)
BoneMatrices[i] = OffsetMatrix × CombinedMatrix
    ↓ CShader::Bind_Matrices (GPU 전송)
g_BoneMatrices[800] (셰이더 상수 버퍼)
    ↓ Vertex Shader
최종 스키닝 위치 = Σ(weight × BoneMatrix × localPos)
```

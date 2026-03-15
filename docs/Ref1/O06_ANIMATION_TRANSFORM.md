# O06. 애니메이션 & Transform

## 1. CTransform - 변환 컴포넌트

### 월드 행렬 직접 조작 방식
```cpp
_float4x4 m_WorldMatrix;  // 4×4 행렬이 곧 상태 그 자체

// 행렬의 각 행이 상태를 의미
enum STATE { RIGHT, UP, LOOK, POSITION, END };
// m_WorldMatrix.r[0] = Right 벡터 (X축, 스케일 포함)
// m_WorldMatrix.r[1] = Up 벡터    (Y축, 스케일 포함)
// m_WorldMatrix.r[2] = Look 벡터  (Z축, 스케일 포함)
// m_WorldMatrix.r[3] = Position   (월드 위치)
```

> **핵심 개념**: 별도의 Position/Rotation/Scale 변수 없이, **행렬 자체가 모든 변환 정보**를 담고 있음. Get/Set으로 행렬 행을 직접 읽고/씀.

### 초기화
```cpp
TRANSFORM_DESC { fSpeedPerSec, fRotationPerSec };
// Initialize에서 항등 행렬로 시작, 속도/회전속도 설정
// Clone하지 않음 (Clone → nullptr 반환) → GameObject마다 직접 생성
```

### 이동 함수
```cpp
Go_Straight(fTimeDelta, pNavigation):
  pos += normalize(Look) × speed × dt
  Navigation이 있으면 isMove() 판정 후 이동

Go_Left/Right/Backward(fTimeDelta):
  Right 또는 Look 방향으로 이동
```

### 회전 함수
```cpp
Rotation(vAxis, fRadian):      // 절대 회전 (기본축에서 회전)
  기본 Right/Up/Look 생성 → 회전 행렬 적용 → 스케일 보존

Rotation(fRotX, fRotY, fRotZ): // 오일러 → 쿼터니언 → 행렬
  RollPitchYaw → Quaternion → RotationMatrix

Turn(vAxis, fTimeDelta):       // 상대 회전 (현재 상태에서 누적)
  현재 Right/Up/Look에 회전 행렬 적용
```

### 유틸리티
```cpp
LookAt(vFocus):   // 특정 점을 바라보도록 회전
  Look = Focus - Position → Right = Cross(WorldUp, Look) → Up = Cross(Look, Right)

Chase(vDest, fTimeDelta, fLimitDistance):  // 대상 추적
  거리 > 제한거리 일 때만 이동

Bind_ShaderResource(pShader, "g_WorldMatrix"):
  월드 행렬을 셰이더에 바인딩 (디바이스 SetTransform 대체)
```

### 스케일 추출/설정
```cpp
Get_Scaled():     // 각 축 벡터의 길이 = 스케일
  length(Right), length(Up), length(Look)

Set_Scale(x, y, z):  // 방향 유지, 크기만 변경
  Normalize(Right) × x, Normalize(Up) × y, Normalize(Look) × z
```

### 현재 프로젝트와 비교
| 현재 (DX9) | 참고 (DX11) |
|-----------|------------|
| `m_vInfo[INFO_END]` (별도 Position/Right/Up/Look) | **월드 행렬 단일 관리** |
| `Move_Pos(&dir, speed, dt)` | `Go_Straight(dt, pNav)` |
| `Rotation(ROT_Y, radians)` | `Rotation(vAxis, fRadian)` / `Turn(vAxis, dt)` |
| `Chase_Target(&pos, speed, dt)` | `Chase(vDest, dt, fLimit)` |
| D3DX 수학 함수 | **DirectXMath SIMD** |
| SetTransform(D3DTS_WORLD) | `Bind_ShaderResource(pShader, name)` |

---

## 2. 스켈레탈 애니메이션 시스템

### Assimp 본 관련 개념 정리
```
aiNode     : 씬 계층 구조의 노드 (본을 포함한 모든 노드)
aiBone     : 메시에 영향을 주는 본 (오프셋 행렬 + 가중치)
aiNodeAnim : 특정 애니메이션에서 본의 시간별 키프레임
```

### 시스템 구성
```
CModel
├── m_Bones: vector<CBone*>           ← 전체 본 계층 (플랫 배열)
├── m_Animations: vector<CAnimation*> ← 애니메이션 목록
└── m_Meshes: vector<CMesh*>          ← 각 메시가 본 인덱스/오프셋 보유

CAnimation
├── m_fDuration                ← 전체 길이 (트랙 단위)
├── m_fTickPerSecond           ← 초당 틱 수
├── m_fCurrentTrackPosition    ← 현재 재생 위치
├── m_Channels: vector<CChannel*> ← 본별 키프레임 채널
└── m_CurrentKeyFrameIndex     ← 채널별 현재 키프레임 인덱스

CChannel
├── m_szName                   ← 대상 본 이름
├── m_iBoneIndex               ← 대상 본의 배열 인덱스
└── m_KeyFrames: vector<KEYFRAME> ← 시간별 SRT 데이터

CBone
├── m_szName                            ← 본 이름
├── m_iParentBoneIndex                  ← 부모 본 인덱스 (-1 = 루트)
├── m_TransformationMatrix              ← 로컬 변환 (애니메이션에 의해 갱신)
└── m_CombinedTransformationMatrix      ← 결합 변환 (루트→자신 누적)
```

---

## 3. 본 계층 구축 (CModel::Ready_Bones)

```
aiNode 트리를 DFS로 순회:
  각 노드 → CBone 생성 (이름, 부모 인덱스, 로컬 변환)
  → 플랫 배열(m_Bones)에 순서대로 추가

결과: m_Bones[0] = 루트, m_Bones[1] = 루트의 첫째 자식, ...
부모-자식 관계: m_iParentBoneIndex로 표현 (배열 인덱스 참조)
```

> **플랫 배열의 장점**: 부모 인덱스가 항상 자식보다 작음 → 앞에서부터 순회하면 부모 먼저 계산 보장.

---

## 4. 애니메이션 재생 흐름

### CModel::Play_Animation(fTimeDelta)
```
1. m_Animations[m_iCurrentAnimIndex]
     →Update_TransformationMatrix(dt, m_Bones, isLoop, &finished)
2. 전체 본 순회:
     Bones[i]→Update_CombinedTransformationMatrix(Bones, PreTransformMatrix)
```

### CAnimation::Update_TransformationMatrix
```
1. m_fCurrentTrackPosition += m_fTickPerSecond × fTimeDelta
2. 종료 판정:
   if (trackPos >= duration)
     isLoop → 0으로 리셋
     !isLoop → *pFinished = true
3. 각 채널(Channel):
   Channel→Update_TransformationMatrix(trackPos, Bones, &keyFrameIndex)
```

### CChannel::Update_TransformationMatrix
```
1. 현재 트랙 위치에 해당하는 키프레임 구간 찾기
   while (trackPos >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
     ++(*pCurrentKeyFrameIndex)

2. 두 키프레임 사이 보간 비율(fRatio) 계산

3. SRT 보간:
   vScale    = XMVectorLerp(curScale, nextScale, fRatio)
   vRotation = XMQuaternionSlerp(curRot, nextRot, fRatio)  ← 구면 보간
   vTranslation = XMVectorLerp(curTrans, nextTrans, fRatio)

4. 변환 행렬 조합:
   Matrix = S × R × T
   Bones[m_iBoneIndex]→Set_TransformationMatrix(Matrix)
```

### CBone::Update_CombinedTransformationMatrix
```
CombinedMatrix = LocalMatrix × Parent.CombinedMatrix × PreTransformMatrix
// 루트 본: Parent가 없으므로 LocalMatrix × PreTransformMatrix
// PreTransformMatrix: 모델 로드 시 적용한 스케일/회전 보정
```

---

## 5. 키프레임 구조 (KEYFRAME)

```cpp
struct KEYFRAME {
    XMFLOAT3  vScale;         // 스케일
    XMFLOAT4  vRotation;      // 회전 (쿼터니언)
    XMFLOAT3  vTranslation;   // 위치
    float     fTrackPosition; // 시간 (틱)
};
```

> SRT 분리 저장 → 보간 시 스케일/위치는 선형(Lerp), 회전은 구면(Slerp) → 자연스러운 애니메이션.

---

## 6. 메시-본 바인딩 (스키닝)

### CMesh의 본 관련 데이터
```cpp
vector<_uint>      m_BoneIndices;    // 이 메시에 영향을 주는 본들의 전역 인덱스
vector<_float4x4>  m_OffsetMatrices; // 각 본의 오프셋 행렬 (바인드포즈 역행렬)
_float4x4          m_BoneMatrices[512]; // 셰이더에 전달할 최종 행렬 배열
```

### Bind_BoneMatrices 계산
```
for (i = 0; i < m_iNumBones; i++)
  m_BoneMatrices[i] = OffsetMatrix[i] × Bones[m_BoneIndices[i]].CombinedMatrix

// 셰이더에서:
// FinalPos = Σ (BoneMatrix[BlendIndex[j]] × Position) × BlendWeight[j]
// 최대 4개 본 영향 (BlendIndex: XMUINT4, BlendWeight: XMFLOAT4)
```

---

## 7. 소켓 본 (파츠 부착)

```cpp
CModel::Get_SocketBoneMatrix_Ptr("Bip001_R_Hand"):
  본 배열에서 이름 검색 → CombinedTransformationMatrix 포인터 반환

// 사용 (CPartObject):
PART_OBJECT_DESC desc;
desc.pParentMatrix = pModel→Get_SocketBoneMatrix_Ptr("Bip001_R_Hand");

// 매 프레임:
m_CombinedWorldMatrix = 자신의 WorldMatrix × *m_pParentMatrix
→ 무기가 캐릭터 손에 따라다님
```

---

## 8. 애니메이션 제어 API

```cpp
CModel::Set_Animation(iIndex, isLoop):
  m_iCurrentAnimIndex = iIndex;
  m_isAnimLoop = isLoop;
  현재 애니메이션 Reset()

CModel::Play_Animation(fTimeDelta):
  Update → 본 갱신 → 완료 시 m_isAnimFinished = true

CModel::is_AnimFinished():
  루프가 아닌 애니메이션 종료 판정
```

---

## 9. 현재 → 참고 비교 요약

| 항목 | 현재 (DX9) | 참고 (DX11) |
|------|-----------|------------|
| Transform 관리 | 별도 Info 배열 + D3DX | **월드 행렬 직접 조작 + SIMD** |
| 회전 | D3DXMatrixRotation* | **XMMatrixRotationAxis / Quaternion** |
| 셰이더 바인딩 | SetTransform(D3DTS_WORLD) | **Bind_ShaderResource(name)** |
| 스켈레탈 애니메이션 | 없음 | **Assimp Bone/Channel/KeyFrame** |
| 키프레임 보간 | 없음 | **Lerp(위치/스케일) + Slerp(회전)** |
| 본 계층 | 없음 | **플랫 배열 + 부모 인덱스** |
| 파츠 부착 | 없음 | **소켓 본 행렬 포인터** |
| 애니메이션 제어 | 없음 | **Set/Play/isFinished** |

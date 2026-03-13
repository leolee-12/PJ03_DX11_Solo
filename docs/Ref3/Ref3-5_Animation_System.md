# 참고프로젝트3 — 애니메이션 시스템 심화 분석

> **분석 대상**: CMesh, CDynamicMesh, CAnimationCtrl, CHierarchyLoader, CDynamicObject(클라이언트) + D3DXFRAME_DERIVED, D3DXMESHCONTAINER_DERIVED, ANI_INFO 구조체
> **모드**: [분석]

---

## 1. 핵심 책임과 시스템 경계

### 이 시스템이 담당하는 것
- **X파일 스키닝 메시 로드**: HierarchyLoader가 `ID3DXAllocateHierarchy` 구현 → 본 계층/메시 컨테이너 생성
- **본 계층 행렬 갱신**: DynamicMesh가 재귀적으로 `CombinedTransformationMatrix` 계산
- **애니메이션 재생/전환**: AnimationCtrl이 `ID3DXAnimationController` 래핑 → 트랙 블렌딩
- **상/하체 분리 애니메이션**: DynamicMesh가 2개의 AnimationCtrl(HIGHER/LOWER) 보유 → 본 이름 기반 분리 갱신
- **하드웨어 스키닝 준비**: `ConvertToIndexedBlendedMesh` → 본 팔레트 기반 렌더링

### 시스템 경계 — 이것은 하지 않는다
- **렌더 큐 등록**: DynamicObject(클라이언트)가 Renderer에 Add
- **이동/충돌**: Transform + NavMgr에서 처리
- **상태 머신**: 각 캐릭터(Player/Monster)가 자체 상태 관리 → 애니메이션 인덱스 결정
- **셰이더 패스 선택**: DynamicMesh::Render_Mesh()가 텍스처 유무로 패스 분기하지만, 셰이더 자체는 별도 시스템

---

## 2. 클래스 간 소유/참조 관계

```
CBase
├── CComponent
│   └── CResources (m_pGraphicDev 소유)
│       └── CMesh (m_isClone 플래그)
│           └── CDynamicMesh ★ 핵심
│               ├── 소유: m_pLoader → CHierarchyLoader* (AddRef, 클론 시 공유)
│               ├── 소유: m_pAnimationCtrl[2] → CAnimationCtrl* (클론 시 독립 생성)
│               ├── 참조: m_pRootFrame → LPD3DXFRAME (원본만 소유, 클론은 공유)
│               ├── 참조: m_MeshContainerList → list<D3DXMESHCONTAINER_DERIVED*>
│               │         (원본에서 수집, 클론은 리스트만 복사 — 포인터 공유)
│               └── 값: m_matParent (Y축 180도 회전 고정)
│
├── CAnimationCtrl ★
│   ├── 소유: m_pGraphicDev (AddRef)
│   ├── 소유: m_pAnimationCtrl → LPD3DXANIMATIONCONTROLLER
│   │         (클론 시 CloneAnimationController로 독립 복제)
│   └── 값: m_dwCurrentTrack, m_dwNewTrack, m_dTimeAcc, m_dPeriod 등
│
└── CHierarchyLoader (ID3DXAllocateHierarchy 구현)
    ├── 참조: m_pGraphicDev (AddRef)
    ├── 참조: m_pFilePath (외부 포인터, 소유 안 함)
    └── 자체 레퍼런스 카운팅 (CBase 미상속, 직접 구현)
```

### 구조체 (D3DX 확장)

```
D3DXFRAME_DERIVED (extends D3DXFRAME)
  └── CombinedTransformationMatrix : _matrix  ← 최종 월드 행렬

D3DXMESHCONTAINER_DERIVED (extends D3DXMESHCONTAINER)
  ├── ppTextures[N] / ppNormalTextures[N] / ppSpecTextures[N] / ppEmissiveTextures[N]
  ├── pOriMesh : LPD3DXMESH  ← 원본 메시 (SW 스키닝용, 현재 미사용)
  ├── pOffsetMatrices[bone] : _matrix  ← 본 오프셋 (바인드 포즈의 역행렬)
  ├── ppCombinedTransformationMatrices[bone] : _matrix**  ← Frame의 Combined 포인터
  ├── pRenderingMatrices[bone] : _matrix  ← Offset × Combined 결과
  ├── pBoneCombinationBuf : LPD3DXBUFFER  ← HW 스키닝 본 조합 정보
  ├── dwMartixPaletteCnt : DWORD  ← 버텍스당 최대 영향 본 수
  └── szTextureName[256]  ← 텍스처 파일명 (렌더 시 특수 분기용)

ANI_INFO (애니메이션 전환 파라미터)
  ├── uiAniIndex : _uint  ← 애니메이션 세트 인덱스
  ├── fAniSpeedFront/End : _float  ← 이전/다음 트랙 속도
  ├── fAniWeightFront/End : _float  ← 이전/다음 트랙 가중치
  ├── dMagicNumber : _double  ← 블렌딩 전환 시간
  └── dEndAniCount : _double  ← 종료 판정 카운트
```

### 소유 원칙 요약

| 관계 | 원본 | 클론 |
|------|------|------|
| HierarchyLoader | 생성 + 소유 | AddRef 공유 |
| RootFrame (본 계층) | 소유 (Free에서 DestroyFrame) | 공유 (해제 안 함) |
| MeshContainerList | 수집 + 소유 | 리스트만 복사 (포인터 공유) |
| AnimationCtrl[2] | 생성 + 소유 | **독립 복제** (CloneAnimationController) |
| DX 디바이스 | AddRef | AddRef |

**핵심 판단**: 본 계층/메시 데이터는 공유, 애니메이션 컨트롤러만 인스턴스별 독립 → 같은 모델의 여러 캐릭터가 각자 다른 애니메이션 재생 가능

---

## 3. 한 프레임 호출 흐름

### 3-1. 로드 시점 (씬 초기화)

```
Scene::Ready_Scene()
  └─ Component_Manager::Add_Component(씬ID, "Proto_DynMesh_Player",
         CDynamicMesh::Create(pDev, filePath, fileName))
             │
             ├─ CHierarchyLoader::Create(pDev, filePath)
             ├─ CAnimationCtrl::Create(pDev)  ← HIGHER용
             │
             ├─ D3DXLoadMeshHierarchyFromX(...)  ★ DX9 핵심 호출
             │   ├─ HierarchyLoader::CreateFrame() × N (본 개수만큼)
             │   │   └─ new D3DXFRAME_DERIVED, Combined=Identity
             │   └─ HierarchyLoader::CreateMeshContainer() × M (서브메시 개수만큼)
             │       ├─ 메시 클론 + 노말 계산
             │       ├─ Diffuse/Normal/Spec/Emissive 텍스처 로드 (파일명 규칙 기반)
             │       └─ ConvertToIndexedBlendedMesh() ← HW 스키닝 변환
             │
             ├─ D3DXMatrixRotationY(&m_matParent, 180°)  ← 모델 방향 보정
             ├─ Update_CombinedMatrices(root, matParent)  ← 초기 본 행렬 계산
             ├─ SetUp_MatrixPointer(root)  ← MeshContainer → Frame 본 포인터 연결
             └─ AnimationCtrl LOWER = Clone(HIGHER)  ← 하체용 복제
```

### 3-2. 매 프레임 (Update + Render)

```
DynamicObject::Update_GameObject(fTimeDelta)
  ├─ 상태 머신 → 애니 인덱스 결정
  ├─ DynamicMesh::Set_AnimationSet(m_vecAnimationInfo[idx])
  │   └─ AnimationCtrl[HIGHER]::Set_AnimationSet(ANI_INFO)
  │       ├─ GetAnimationSet(uiAniIndex)  ← DX에서 애니 세트 획득
  │       ├─ SetTrackAnimationSet(newTrack, pAS)
  │       ├─ UnkeyAllTrackEvents (양쪽 트랙)
  │       ├─ KeyTrackEnable/Weight/Speed (이전 트랙 페이드 아웃)
  │       ├─ SetTrackEnable/KeyTrackWeight/Speed (새 트랙 페이드 인)
  │       └─ ResetTime() + SetTrackPosition(0.0)
  │
  ├─ [상/하체 분리 시] Set_LowAnimationSet(...)
  │   └─ AnimationCtrl[LOWER]::Set_AnimationSet(...)
  │
  └─ Renderer::Add_RenderList(DYNAMICMESH_NONEALPHA, this)

DynamicObject::Render_GameObject(pEffect, uPassIdx)
  │
  ├─ [전신 애니] DynamicMesh::Play_AnimationSet(fTimeDelta)
  │   ├─ AnimationCtrl[HIGHER]::Play_AnimationSet(fTimeDelta)
  │   │   └─ m_pAnimationCtrl->AdvanceTime(fTimeDelta, NULL) ★
  │   └─ Update_CombinedMatrices(root, matParent)  ← 본 계층 갱신
  │
  ├─ [상/하체 분리] DynamicMesh::Play_AnimationSet(fTimeDelta, camAngle, lowBones, highBones)
  │   ├─ AnimationCtrl[LOWER]::Play_AnimationSet(fTimeDelta)
  │   ├─ Update_CombinedMatrices(root, matParent, highBoneNames)  ← 상체 본 제외
  │   ├─ AnimationCtrl[HIGHER]::Play_AnimationSet(fTimeDelta)
  │   ├─ 카메라 앵글 기반 상체 회전 행렬 생성
  │   └─ Update_CombinedMatrices(root, matRotX, lowBoneNames)  ← 하체 본 제외
  │
  ├─ Transform::SetUp_OnShader(pEffect, "g_matWorld")
  │
  └─ DynamicMesh::Render_Mesh(pEffect, uPassIdx)
      └─ 각 MeshContainer에 대해:
          ├─ Offset × Combined → RenderingMatrices 계산
          ├─ pEffect->SetMatrixArray("MatrixPalette", ...) ★ HW 스키닝
          ├─ pEffect->SetInt("numBoneInfluences", ...)
          ├─ 텍스처 세팅 (Base/Normal/Spec/Emissive)
          ├─ 패스 분기 (Normal+Spec=0, Normal만=1, 없음=2)
          ├─ pEffect->CommitChanges() + BeginPass/EndPass
          └─ pMesh->DrawSubset(i) ★
```

---

## 4. 사용된 디자인 패턴

### 4-1. 프로토타입 (CDynamicMesh Clone)
- **원본**: `Create()` → X파일 로드, 본 계층 구축, HW 스키닝 변환
- **클론**: 복사 생성자 → 본 계층/메시 데이터 **공유**, 애니메이션 컨트롤러 **독립 복제**

```cpp
CDynamicMesh(const CDynamicMesh& rhs)
  : CMesh(rhs)                          // 디바이스 AddRef, m_isClone=true
  , m_pLoader(rhs.m_pLoader)            // 공유 (AddRef)
  , m_pRootFrame(rhs.m_pRootFrame)      // 공유 (포인터만 복사)
  , m_MeshContainerList(rhs.m_MeshContainerList)  // 공유 (포인터 리스트 복사)
{
    m_pLoader->AddRef();
    // 애니메이션 컨트롤러만 독립 생성 (각 인스턴스가 다른 애니 재생)
    m_pAnimationCtrl[HIGHER] = CAnimationCtrl::Create(*rhs.m_pAnimationCtrl[HIGHER]);
    m_pAnimationCtrl[LOWER]  = CAnimationCtrl::Create(*rhs.m_pAnimationCtrl[LOWER]);
}
```

### 4-2. 전략 패턴 (ID3DXAllocateHierarchy 구현)
- DX9의 `D3DXLoadMeshHierarchyFromX`가 로딩 전략을 콜백으로 요구
- `CHierarchyLoader`가 `CreateFrame`, `CreateMeshContainer`, `DestroyFrame`, `DestroyMeshContainer` 구현
- 프레임/메시 컨테이너 구조를 커스텀 확장 가능

### 4-3. 팩토리 메서드 (Create 정적 함수)
- `CDynamicMesh::Create(pDev, path, file)` → `new` + `Ready_Meshes` + 실패 시 `Safe_Release`
- `CAnimationCtrl::Create(pDev)` — 빈 컨트롤러 생성
- `CAnimationCtrl::Create(const CAnimationCtrl& rhs)` — 복제 전용 오버로드

### 4-4. 데이터 주도 설계 (ANI_INFO + XML)
- 애니메이션 파라미터를 코드에 하드코딩하지 않고 XML로 관리
- `DynamicObject::Load_AnimationInfo(szFilePath)` → tinyxml2로 파싱 → `m_vecAnimationInfo` 벡터에 저장
- 상태 머신에서 인덱스로 참조: `Set_AnimationSet(m_vecAnimationInfo[STATE_ATTACK])`

---

## 5. DirectX API 호출 지점과 래핑 방식

### CHierarchyLoader (로드 시점)

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `D3DXLoadMeshHierarchyFromX` | DynamicMesh::Ready_Meshes | X파일 전체 로드 (본+메시+애니) |
| `CloneMeshFVF` | HierarchyLoader::CreateMeshContainer | FVF 추가(노말) 위한 메시 복제 |
| `D3DXComputeNormals` | 〃 | 노말 자동 계산 |
| `D3DXCreateTextureFromFile` | 〃 | Diffuse/Normal/Spec/Emissive 텍스처 로드 |
| `ConvertToIndexedBlendedMesh` | 〃 | SW→HW 스키닝 메시 변환 |
| `GetBoneOffsetMatrix` | 〃 | 본 오프셋 행렬(바인드 포즈 역변환) 획득 |

### CAnimationCtrl (재생 시점)

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `CloneAnimationController` | 복사 생성자 | 독립 컨트롤러 복제 |
| `GetAnimationSet` | Set_AnimationSet | 인덱스로 애니 세트 획득 |
| `SetTrackAnimationSet` | 〃 | 새 트랙에 애니 세트 할당 |
| `KeyTrackEnable/Weight/Speed` | 〃 | 트랙 블렌딩 키 설정 |
| `UnkeyAllTrackEvents` | 〃 | 기존 이벤트 초기화 |
| `ResetTime` | 〃 | 글로벌 타이머 리셋 |
| `SetTrackPosition` | 〃 / Set_TrackPosition | 트랙 위치 강제 설정 |
| `AdvanceTime` | Play_AnimationSet | 시간 진행 → 본 행렬 갱신 |
| `GetTrackDesc` | GetTrackPos | 현재 트랙 재생 위치 조회 |

### CDynamicMesh (렌더 시점)

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `D3DXFrameFind` | Get_FrameMatrix | 이름으로 본 프레임 검색 |
| `LockVertexBuffer/UnlockVertexBuffer` | Render_Mesh(void) | SW 스키닝: 원본→결과 복사 |
| `UpdateSkinnedMesh` | 〃 | CPU 스키닝 수행 (SW 경로) |
| `SetTexture` | 〃 | 서브셋 텍스처 바인딩 |
| `DrawSubset` | 〃 | 서브메시 드로우 |
| `SetMatrixArray("MatrixPalette")` | Render_Mesh(pEffect) | HW 스키닝: 본 행렬 팔레트 전송 |
| `SetInt("numBoneInfluences")` | 〃 | 버텍스당 영향 본 수 |
| `CommitChanges` | 〃 | 셰이더 파라미터 커밋 |
| `BeginPass/EndPass` | 〃 | 렌더 패스 실행 |
| `D3DXComputeBoundingBox` | Compute_MinMax | AABB 계산 (충돌용) |

---

## 6. 상/하체 분리 애니메이션 상세

### 원리
- AnimationCtrl 2개: `ANICTRL_HIGHER`(상체), `ANICTRL_LOWER`(하체)
- 각각 독립적으로 `Set_AnimationSet` → 다른 애니메이션 재생 가능
- `Play_AnimationSet(fTimeDelta, fCamAngle, pLowBoneName, pHighBoneName)`에서 본 이름 기반 분리

### 실행 순서
```
1. LOWER의 AdvanceTime(fTimeDelta)    ← 하체 애니 시간 진행
2. Update_CombinedMatrices(root, matParent, pHighBoneName)
   → 상체 본 이름과 일치하면 재귀 중단 → 하체만 갱신

3. HIGHER의 AdvanceTime(fTimeDelta)   ← 상체 애니 시간 진행
4. 카메라 앵글 기반 X축 회전 행렬 계산 (상체 조준)
5. Update_CombinedMatrices(root, matRotX, pLowBoneName)
   → 하체 본 이름과 일치하면 재귀 중단 → 상체만 갱신
```

### 본 이름 필터링 (Update_CombinedMatrices 오버로드)
```cpp
void Update_CombinedMatrices(D3DXFRAME_DERIVED* pFrame,
                             const _matrix* pMatrix,
                             const char** pBoneName)  // 3개 본 이름 배열
{
    pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix * *pMatrix;

    if (pFrame->pFrameSibling) {
        // 형제가 제외 대상이면 건너뜀
        if (strcmp(pBoneName[0], pFrame->pFrameSibling->Name) &&
            strcmp(pBoneName[1], ...)) // 3개 모두 불일치하면 재귀
            Update_CombinedMatrices(sibling, pMatrix, pBoneName);
    }
    if (pFrame->pFrameFirstChild) {
        // 자식이 제외 대상이면 return
        if (!strcmp(pBoneName[0], pFrame->pFrameFirstChild->Name) || ...)
            return;
        Update_CombinedMatrices(child, &Combined, pBoneName);
    }
}
```
- **제외 본 3개**를 하드코딩 배열로 전달 → 해당 본의 하위 트리 전체가 갱신 제외
- 상체 갱신 시 하체 루트 본 3개 제외, 하체 갱신 시 상체 루트 본 3개 제외

---

## 7. 설계 판단 분석 — 참고할 점과 한계

### 참고할 점

**1) 본 계층 공유 + 애니컨 독립 복제**
- 가장 비싼 리소스(메시 데이터, 본 계층, 텍스처)는 모든 인스턴스가 **공유**
- 인스턴스별로 달라야 하는 것(재생 위치, 트랙 상태)만 `CloneAnimationController`로 **독립 복제**
- 메모리 효율 극대화: 같은 캐릭터 100마리 → 메시 1벌 + 애니 컨트롤러 100개

**2) m_isClone 플래그로 해제 분기**
```cpp
void CDynamicMesh::Free(void) {
    if (false == m_isClone)          // 원본만
        m_pLoader->DestroyFrame(m_pRootFrame);  // 본 계층 해제
    // 클론은 AnimationCtrl만 해제, 본 계층/메시 터치 안 함
}
```
- CMesh 레벨에서 `m_isClone` 관리 → 공유 리소스의 이중 해제 방지

**3) HW 스키닝 + SW 스키닝 양쪽 경로**
- `Render_Mesh(void)`: SW 스키닝 — `UpdateSkinnedMesh` (CPU에서 정점 변환)
- `Render_Mesh(pEffect, passIdx)`: HW 스키닝 — `MatrixPalette` 셰이더 전송
- 셰이더 지원 여부에 따라 폴백 가능

**4) 텍스처 자동 탐지 (파일명 규칙)**
```
model_d.tga → Diffuse
model_n.tga → Normal
model_s.tga → Specular
model_e.tga → Emissive
```
- HierarchyLoader가 Diffuse 파일명에서 `_` 앞 접두사 추출 → 자동으로 n/s/e 텍스처 탐색
- 없으면 nullptr → 렌더 시 패스 자동 분기 (노말+스펙=패스0, 노말만=패스1, 없음=패스2)

**5) ANI_INFO 데이터 주도**
- 애니메이션 블렌딩 파라미터(속도, 가중치, 전환시간)를 XML로 관리
- 게임 디자이너가 코드 수정 없이 애니 느낌 조절 가능
- `dMagicNumber`: 전환 소요 시간 — 클수록 부드러운 블렌딩, 작으면 즉시 전환

**6) 2트랙 교대 블렌딩**
```cpp
m_dwNewTrack = m_dwCurrentTrack == 0 ? 1 : 0;  // 0↔1 교대
// 이전 트랙: 비활성화 예약 + 가중치 0으로 페이드
// 새 트랙: 활성화 + 가중치 1로 페이드
```
- 트랙 2개만 사용하여 단순화 → 추가 트랙 관리 오버헤드 없음
- `KeyTrackWeight`의 `D3DXTRANSITION_LINEAR`: 선형 보간으로 자연스러운 전환

### 한계/개선 가능 포인트

**1) 상/하체 분리의 본 이름 하드코딩**
- 제외 본 3개를 `const char**` 배열로 전달 — 모델마다 다른 본 이름이면 수정 필요
- 개선: 본 태깅 시스템, 또는 본 인덱스 기반 마스크

**2) 렌더 함수 내 특정 텍스처명 분기**
```cpp
if (!lstrcmp(L"missleTower_d.tga", (*iter)->szTextureName) || ...)
    uPassIdx = passTemp;  // 타워류 특수 처리
```
- 텍스처 파일명으로 렌더 패스를 분기 → 하드코딩, 확장성 부족
- 개선: 머티리얼에 렌더 타입 플래그 추가

**3) CombinedTransformationMatrix를 Frame에 직접 갱신**
- 클론이 원본의 RootFrame을 **공유**하므로, Update_CombinedMatrices 호출 시 공유 데이터 **덮어쓰기**
- 같은 프레임에 같은 메시의 다른 인스턴스가 다른 포즈를 가지면 마지막 갱신이 승리
- 실제로는 렌더 직전에 갱신+즉시 렌더하므로 문제 없지만, 멀티스레드 시 위험

**4) SW 스키닝 경로에서 pOriMesh 미사용**
- `CreateMeshContainer`에서 `pOriMesh` 할당 코드가 주석 처리됨
- `Render_Mesh(void)` SW 경로에서 `pOriMesh`를 원본으로 사용 → 주석 해제 필요
- HW 스키닝 경로(`Render_Mesh(pEffect)`)에서는 문제없음

**5) HierarchyLoader가 CBase를 상속하지 않음**
- `ID3DXAllocateHierarchy`가 COM 인터페이스 → CBase 다중 상속 회피
- `AddRef()`/`Release()`를 직접 구현 (CBase와 동일 로직이지만 별도)
- Free() 대신 Release()에서 직접 `Safe_Release(m_pGraphicDev)` + `delete this`

**6) AdvanceTime이 Render에서 호출**
- 일반적으로 Update에서 시간 진행, Render에서 그리기만 하는 것이 원칙
- 이 프로젝트는 Play_AnimationSet(AdvanceTime + Update_CombinedMatrices)을 Render 시점에 호출
- 이유: 상/하체 분리 시 CombinedMatrix를 갱신 직후 바로 렌더해야 공유 데이터 충돌 방지

# O05. 물리 & 충돌 / 내비게이션

## 1. 충돌 시스템 구조

### 클래스 계층
```
CComponent
└── CCollider (충돌 컴포넌트)
        │ m_eType: COLLIDER (AABB / OBB / SPHERE)
        │ m_pBounding: CBounding*
        │
        └── CBounding (abstract, CBase 상속)
              ├── CBounding_AABB   ← BoundingBox (DirectXCollision)
              ├── CBounding_OBB    ← BoundingOrientedBox
              └── CBounding_Sphere ← BoundingSphere
```

### CCollider - 전략 패턴
```cpp
// 충돌체 타입에 따라 다른 Bounding 객체를 내부에 보유
CCollider::Initialize_Prototype(COLLIDER::AABB);  → CBounding_AABB 생성
CCollider::Initialize_Prototype(COLLIDER::OBB);   → CBounding_OBB 생성
CCollider::Initialize_Prototype(COLLIDER::SPHERE); → CBounding_Sphere 생성

// 매 프레임 월드 행렬로 갱신
CCollider::Update(WorldMatrix);  → m_pBounding->Update(WorldMatrix);

// 교차 판정
CCollider::Intersect(pOtherCollider);
  → m_pBounding->Intersect(otherType, otherBounding);
```

---

## 2. Bounding 볼륨 상세

### 공통 인터페이스 (CBounding)
```cpp
struct BOUNDING_DESC { _float3 vCenter; };  // 기본 오프셋
virtual void Update(_fmatrix WorldMatrix) = 0;       // 변환 적용
virtual _bool Intersect(COLLIDER eType, CBounding*);  // 교차 판정
#ifdef _DEBUG
virtual HRESULT Render(PrimitiveBatch*, vColor);       // 디버그 시각화
#endif
```

### CBounding_AABB (축 정렬 바운딩 박스)
```cpp
struct AABB_DESC : BOUNDING_DESC { _float3 vSize; };
BoundingBox* m_pOriginalDesc;  // 초기 상태 (불변)
BoundingBox* m_pDesc;          // 변환된 상태 (매 프레임 갱신)

// DirectXCollision의 BoundingBox 활용
Update(): m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
Intersect_AABB(): m_pDesc->Intersects(*pTarget->m_pDesc);
```

### CBounding_OBB (방향 바운딩 박스)
```cpp
struct OBB_DESC : BOUNDING_DESC {
    _float3 vSize;
    _float3 vRadians;  // 초기 회전 (X, Y, Z)
};
BoundingOrientedBox* m_pOriginalDesc;
BoundingOrientedBox* m_pDesc;

// OBB 자체 구조체 (분리축 판정용)
struct tagOBB {
    _float3 vCenter;
    _float3 vCenterDir[3];  // 중심→면 벡터 (절반 크기)
    _float3 vAlignDir[3];   // 정규화된 축 방향
};
```

### CBounding_Sphere (구 충돌체)
```cpp
struct SPHERE_DESC : BOUNDING_DESC { _float fRadius; };
BoundingSphere* m_pOriginalDesc;
BoundingSphere* m_pDesc;
```

### 교차 판정 조합
```
AABB  ↔ AABB  : BoundingBox::Intersects(BoundingBox)
OBB   ↔ OBB   : 분리축 정리(SAT) 직접 구현
Sphere↔ Sphere : BoundingSphere::Intersects(BoundingSphere)
```

> **현재와 차이**: 현재 프로젝트는 `CRcCol`(사각형), `CGridCol`(그리드) 등 자체 구현. 참고에서는 **DirectXCollision** 라이브러리의 `BoundingBox/OBB/Sphere` 활용 + 디버그 시각화.

### 디버그 시각화
```cpp
#ifdef _DEBUG
BasicEffect*                          m_pEffect;  // DirectXTK
PrimitiveBatch<VertexPositionColor>*  m_pBatch;
// 와이어프레임으로 충돌 볼륨 시각화
// 충돌 시 색상 변경 (m_isColl 플래그)
#endif
```

---

## 3. 내비게이션 메시 시스템

### 구성
```
CNavigation (CComponent)
├── m_Cells: vector<CCell*>       ← 삼각형 셀 배열
├── m_iCurrentCellIndex: _int     ← 현재 위치한 셀 (-1 = 없음)
└── m_pParentMatrix: static       ← 부모 변환 (지형 오프셋)

CCell (CBase)
├── m_vPoints[3]        ← 삼각형 꼭짓점 (A, B, C)
├── m_vNormals[3]       ← 각 변의 바깥 방향 노멀 (AB, BC, CA)
├── m_iNeighborIndices[3] ← 인접 셀 인덱스 (-1 = 없음)
└── m_iIndex            ← 자신의 인덱스
```

### 내비메시 데이터 로드
```cpp
Initialize_Prototype(pNavigationDataFile):
  // 바이너리 파일에서 삼각형 데이터 읽기
  while ReadFile:
    CCell::Create(pDevice, pContext, vPoints[3], index)
  SetUp_Neighbors();  // 공유 꼭짓점으로 인접 관계 설정
```

### 인접 셀 설정 (SetUp_Neighbors)
```
모든 셀 쌍 비교:
  셀 A의 변 AB의 두 꼭짓점이 셀 B의 어떤 변과 일치하면
  → A.Neighbor[AB] = B, B.Neighbor[해당변] = A
```

### 이동 가능 판정 (isMove)
```cpp
_bool isMove(_fvector vResultPos):
  현재 셀(m_iCurrentCellIndex)에서 결과 위치가:
  1. 셀 내부 → true (이동 허용)
  2. 셀 외부 → 나간 변의 이웃 셀 확인
     - 이웃 있음 → m_iCurrentCellIndex 갱신, true
     - 이웃 없음 → false (이동 불가, 벽)
```

### 셀 내부 판정 (CCell::isIn)
```
각 변(AB, BC, CA)의 노멀 방향으로:
  점이 변의 안쪽에 있는지 내적으로 판정
  바깥으로 나간 변이 있으면 → 해당 변의 이웃 인덱스 반환
```

### 높이 보간 (CCell::Compute_Height)
```cpp
_float Compute_Height(_fvector vResultPos):
  삼각형 3점으로 평면 방정식 구성
  → 주어진 XZ 좌표에서 Y값 계산
  → 캐릭터가 내비메시 표면 위에 정확히 위치
```

### SetUp_OnNavigation
```cpp
_vector SetUp_OnNavigation(_fvector vWorldPos):
  현재 셀에서 높이 계산 → Y좌표를 내비메시 높이로 보정
  → 캐릭터가 지형을 뚫고 내려가지 않도록 보장
```

---

## 4. CPicking - 마우스 피킹

### DX11 방식 (렌더타겟 기반)
```cpp
ID3D11Texture2D* m_pTexture2D;    // CPU 읽기용 스테이징 텍스처
_float4* m_pPixelPositions;        // 월드 좌표 데이터

Update():
  Target_World 렌더타겟의 내용을 m_pTexture2D로 복사
  → Map으로 CPU 접근 → m_pPixelPositions에 저장

Picking(_float3* pOut):
  마우스 좌표 → 해당 픽셀의 월드 좌표 반환
```

> **현재와 차이**: 현재는 `CCalculator::Picking_OnTerrain`에서 레이캐스트(역투영 → 삼각형 교차). 참고에서는 **G-Buffer의 Target_World에서 직접 월드 좌표를 읽는** 방식 → 모든 오브젝트에 대해 피킹 가능, 성능도 일정.

---

## 5. 현재 → 참고 비교 요약

| 항목 | 현재 (DX9) | 참고 (DX11) |
|------|-----------|------------|
| 충돌 볼륨 | 자체 구현 (RcCol, GridCol) | **DirectXCollision** (AABB/OBB/Sphere) |
| 디버그 시각화 | 없음 | **PrimitiveBatch** 와이어프레임 |
| 내비게이션 | 없음 | **NavMesh** (삼각형 셀, 이웃 탐색) |
| 높이 보간 | CCalculator::Compute_HeightOnTerrain | **CCell::Compute_Height** (셀 기반) |
| 피킹 | 레이캐스트 (역투영+삼각형교차) | **G-Buffer 월드좌표 읽기** |
| 이동 제한 | 없음 (지형 위만) | **셀 기반 이동 가능 판정** |
| 충돌 조합 | 사각형/그리드 한정 | **AABB×AABB, OBB×OBB, Sphere×Sphere** |

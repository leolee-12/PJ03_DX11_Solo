# 참고프로젝트3 — 충돌 시스템 심화 분석

> 분석 범위: `CCollision`, `CCollisionSphere`, `CCollisionManager`, `CNavMgr`, `CNaviCell`, `CLine2D`, `CMouseCol`
> 분석 대상: 헤더 + cpp 구현부 전체

---

## 1. 시스템 핵심 책임과 경계

이 프로젝트의 충돌 시스템은 **세 개의 독립적인 서브시스템**으로 구성된다:

| 서브시스템 | 핵심 클래스 | 위치 | 책임 |
|-----------|-----------|------|------|
| **구체 충돌** | `CCollisionSphere` + `CCollisionManager` | Engine + Client | 전투 공격 판정, 밀어내기(Push) |
| **NavMesh 이동** | `CNavMgr` + `CNaviCell` + `CLine2D` | Engine | 지형 위 이동 제한, 슬라이드, 낙하, 높이 보정 |
| **마우스 피킹** | `CMouseCol` + `CCollisionManager` | Client | 메시/NavMesh/지형 레이캐스트 |

### 경계

```
┌──────────────────────────────────────────────────────────┐
│  Client                                                   │
│                                                           │
│  CCollisionManager (싱글톤)                                │
│    ├─ 구체 충돌 저장소: unordered_map<tag, list<CSphere*>> │
│    ├─ 마우스 피킹: list<CMouseCol*>                        │
│    └─ NavMesh 피킹: vector<NAVMESH> (읽기 전용 복사본)     │
│                                                           │
│  CMouseCol : CCollision                                   │
│    └─ 메시 삼각형 순회 → D3DXIntersectTri                  │
│                                                           │
│  각 GameObject (Player, Monster, Tower...)                 │
│    └─ Update() 내에서 직접 충돌 판정 호출                   │
├──────────────────────────────────────────────────────────┤
│  Engine                                                   │
│                                                           │
│  CCollision : CComponent (추상 베이스)                     │
│    └─ CCollisionSphere                                    │
│       └─ D3DXCreateSphere() + 구체-구체 거리 판정          │
│                                                           │
│  CNavMgr (싱글톤)                                          │
│    └─ vector<CNaviCell*>                                  │
│       └─ CNaviCell                                        │
│          └─ CLine2D[3] (변의 2D 법선 벡터)                 │
│          └─ D3DXPlaneFromPoints() (평면 방정식)             │
└──────────────────────────────────────────────────────────┘
```

**핵심 경계 원칙**:
- Engine은 **충돌 프리미티브**(Sphere, NavCell, Line2D)와 **수학적 판정 로직**만 제공
- Client의 `CCollisionManager`가 **등록/조회/렌더**를 중앙 관리
- 충돌 **반응**(데미지, 넉백 등)은 각 GameObject가 자체 처리 — 매니저는 판정만 보조

---

## 2. 클래스 간 소유/참조 관계

```
CCollisionManager (Client 싱글톤)
  ├── [소유] unordered_map<const _tchar*, list<CComponent*>>  m_MapCollisionComponent
  │         └─ 태그별 CCollisionSphere 리스트 (참조만, 소유는 GameObject)
  ├── [소유] list<CMouseCol*>  m_listMousecol
  │         └─ CMouseCol → LPD3DXMESH (클론 메시)
  ├── [복사] vector<NAVMESH>  m_vecNavMesh (NavMgr에서 초기화 시 복사)
  ├── [복사] vector<NAVMESH>  m_vecTowerNavMesh (타워용 부분집합)
  ├── [참조] CDataManager*  (View/Proj 행렬, 카메라 위치)
  └── [참조] CMouse_Manager*  (마우스 좌표)

CNavMgr (Engine 싱글톤)
  ├── [소유] vector<CNaviCell*>  m_vecNaviMesh (전체 NavMesh)
  ├── [참조] vector<CNaviCell*>  m_vecNaviMeshSecondFloor (2층 셀 → 같은 포인터)
  ├── [소유] LPD3DXLINE  m_pLine (디버그 라인 렌더)
  └── [참조] LPDIRECT3DDEVICE9  m_pGraphicDev

CNaviCell (Engine)
  ├── [소유] CLine2D*  m_pLine2D[3]  (AB, BC, CA 변)
  ├── [참조] CNaviCell*  m_pNeighbor[3]  (이웃 셀 — 소유X)
  ├── [소유] CGameObject*  m_pFontNumber  (디버그 인덱스 표시)
  └── D3DXPLANE  m_tPlane  (평면 방정식)
  └── _vec3  m_vPoint[3]  (꼭짓점)

CCollisionSphere (Engine 컴포넌트)
  ├── [소유] ID3DXMesh*  m_pCollisionSphere  (D3DX 구체 메시)
  ├── [참조] const _matrix*  m_pCollisionBoneMatrix  (부모 뼈 행렬)
  └── _matrix  m_matWorldCollision  (월드 충돌 행렬 = Bone × World)

CLine2D (Engine)
  └── _vec2 m_vStartPoint, m_vEndPoint, m_vNormal  (2D 변 + 법선)
```

### 소유권 요약

| 소유자 | 소유 대상 | 해제 방식 |
|--------|----------|----------|
| `CGameObject` 파생 클래스 | `CCollisionSphere` (컴포넌트로 보유) | `Free()` 시 `Safe_Release` |
| `CCollisionManager` | 구체 리스트의 **참조만** | `Free()`에서 `list.clear()`만 (Release 안 함) |
| `CCollisionManager` | `CMouseCol` 인스턴스 | `Free()`에서 `Safe_Release` |
| `CNavMgr` | `CNaviCell` 인스턴스 | `Free()`에서 `CRelease_Single` |
| `CNaviCell` | `CLine2D` 3개 | `Free()`에서 `Safe_Release` |

---

## 3. 주요 함수의 호출 흐름 (한 프레임 기준)

### 3.1 구체 충돌 — 등록 및 판정

```
[초기화 시점 — 오브젝트 생성]
CPlayer::Ready_Object() / CTower::Ready_Object()
  ├─ CCollisionSphere::Create(pGraphicDev) → Clone()
  │   └─ Create_Sphere(fRadius, pBoneMatrix, fScale)
  │       └─ D3DXCreateSphere(dev, radius, 20, 20, &mesh, NULL)  ← DX9 API
  └─ CCollisionManager::Add_Component(L"Tag_Player", pSphere)
      └─ m_MapCollisionComponent["Tag_Player"].push_back(pSphere)

[매 프레임 — 위치 갱신]
CGameObject::Update_GameObject(fTimeDelta)
  └─ CCollisionSphere::Update_CollisionSphere(&matWorld)
      └─ m_matWorldCollision = *m_pCollisionBoneMatrix * matWorld
         (뼈 로컬행렬 × 오브젝트 월드행렬 = 충돌 구 월드 위치)

[매 프레임 — 판정 (각 오브젝트가 직접 수행)]
CPlayer::Update_GameObject(fTimeDelta)
  └─ [공격 판정]
      list<CComponent*> monsterSpheres =
          CCollisionManager::Find_Componentlist(L"Tag_Monster");
      for (pTarget : monsterSpheres)
          if (m_pAttackSphere->Check_Collision((CCollisionSphere*)pTarget))
              → 데미지 적용, 히트 플래그 설정

  └─ [밀어내기 판정]
      for (pTarget : playerSpheres)
          if (m_pBodySphere->Push_Collision((CCollisionSphere*)pTarget))
              → m_vDir 방향으로 위치 보정
```

### 3.2 구체 충돌 판정 수학

```cpp
// Check_Collision — 단순 구체-구체 교차
_bool CCollisionSphere::Check_Collision(CCollisionSphere* pTarget)
{
    vPos  = this->m_matWorldCollision[3]    // 내 구체 중심
    vPos2 = pTarget->Get_WorldMatrix()[3]   // 대상 구체 중심

    // 두 구의 반경 합 < 두 중심 간 거리 → 충돌 없음
    if (m_fRadius * m_fScale + fRadius * pTarget->Scale
        < D3DXVec3Length(&(vPos - vPos2)))
        return false;
    return true;
}

// Push_Collision — 충돌 + 밀기 방향 계산
_bool CCollisionSphere::Push_Collision(CCollisionSphere* pTarget)
{
    // Check_Collision과 동일한 거리 판정
    ...
    // 충돌 시 밀기 방향 = 정규화(내 위치 - 대상 위치)
    D3DXVec3Normalize(&m_vDir, &(vPos - vPos2));
    return true;
}

// Check_AuraCollision — 단방향 범위 체크 (대상 반경만 사용)
if (fRadius * pTarget->Scale < distance)  // 내 반경 무시
    return false;
```

### 3.3 NavMesh 이동 — 한 프레임 흐름

```
CPlayer::Update_GameObject(fTimeDelta)
  │
  ├─ 이동 방향 계산 → vDir = Normalize(targetPos - myPos) * speed * dt
  │
  ├─ CNavMgr::MoveOnNavMesh(&vPos, &vDir, dwCurrentCellIdx)
  │   │
  │   ├─ CNaviCell::CheckPass(&vPos, &vDir, &eNeighborID, bTwoPass)
  │   │   └─ for i in [AB, BC, CA]:
  │   │       CLine2D::CheckLinePass(&vec2(pos.x + dir.x, pos.z + dir.z))
  │   │         └─ dot(moveEnd - lineStart, lineNormal) > 0 → 선을 넘음
  │   │
  │   ├─ [셀을 넘지 않는 경우]
  │   │   vPos += vDir  (그냥 이동)
  │   │
  │   ├─ [셀을 넘는 경우 → 이웃이 있음]
  │   │   ├─ 이웃의 NOPASSAGE 옵션 체크 → 통과 불가면 슬라이드
  │   │   ├─ vPos += vDir
  │   │   └─ dwNextIndex = pNeighbor->GetIndex()
  │   │
  │   ├─ [셀을 넘는 경우 → 이웃이 없음 (경계)]
  │   │   ├─ NAV_OPTION_DROP → Drop(): dwIndex = -1, vPos += vDir (낙하)
  │   │   └─ 그 외 → Slide_Vector(): 벽 따라 미끄러짐
  │   │
  │   └─ return dwNextIndex
  │
  ├─ CNavMgr::FallOnNavMesh(pTransform, dwCellIdx, bDead)
  │   ├─ D3DXPLANE plane = cell->GetPlane()
  │   ├─ fY = (-a*x - c*z - d) / b  (평면 방정식으로 높이 계산)
  │   ├─ if (pos.y <= fY) → pos.y = fY  (지면에 붙임)
  │   └─ NAV_OPTION_DEATH → bDead = true (사망 영역)
  │
  └─ [2층 전환]
      if CNavMgr::IsAbleToUpStairs(dwIdx)
          dwIdx = CNavMgr::FindSecondFloor(vPos, dwIdx)
```

### 3.4 슬라이드 벡터 계산

```
CNavMgr::Slide_Vector(pPos, pDir, dwIndex, eNeighborID)
  │
  ├─ vec2Normal = cell->GetLine(eNeighborID)->GetNormal()   ← 변의 2D 법선
  ├─ vec3Normal = {normal.x, 0, normal.y}                   ← 3D로 확장
  │
  ├─ vecSlide = pDir - vec3Normal * dot(vec3Normal, pDir)    ← 벽 투영 제거
  │    (이동 벡터에서 법선 방향 성분을 제거 = 벽 방향만 남김)
  │
  ├─ cell->CheckNavPass(pPos, &vecSlide, &eNeighborID)
  │   └─ 슬라이드 벡터로도 다른 변을 넘는지 재검사
  │
  ├─ [다른 변의 이웃 있음 + 통과 가능]
  │   → dwIndex = 이웃 인덱스, vPos += vecSlide
  ├─ [이웃 없음 (모서리)]
  │   → 이동 차단
  └─ [안 넘음]
      → vPos += vecSlide (벽 따라 미끄러짐)
```

### 3.5 마우스 피킹 — 한 프레임 흐름

```
[타워 건설 시 — NavMesh 피킹]
CPlayer::Key_Input()
  └─ CCollisionManager::Pick_NavMesh(vecPick)
      ├─ Translation_ViewSpace()
      │   ├─ ptMouse = CMouse_Manager::Get_MousePoint()
      │   ├─ pmatProj = CDataManager::Get_ProjMatrix()
      │   ├─ vTemp.x = (mouseX / (WINCX/2) - 1) / proj._11   ← NDC → View
      │   ├─ vTemp.y = (-mouseY / (WINCY/2) + 1) / proj._22
      │   ├─ vTemp.z = 1.0
      │   ├─ m_vPivotPos = (0,0,0)     ← View 원점
      │   └─ m_vRayDir = Normalize(vTemp)
      │
      ├─ Translation_Local(Identity)
      │   ├─ matViewInv = CDataManager::Get_ViewInverseMatrix()
      │   ├─ m_vPivotPos = TransformCoord(pivot, ViewInv * WorldInv)
      │   └─ m_vRayDir = TransformNormal(dir, ViewInv * WorldInv)
      │
      └─ for each NAVMESH in m_vecNavMesh:
          D3DXIntersectTri(A, B, C, &pivot, &dir, &fU, &fV, &fDist)
            → 카메라와 가장 가까운 교차점 선택
            → return dwOption (타워 건설 가능 여부)

[메시 피킹]
CCollisionManager::Pick_MouseCol(vecPick)
  ├─ Translation_ViewSpace() → Translation_Local(Identity)
  └─ Picking(vecPick)
      └─ for each CMouseCol in m_listMousecol:
          CMouseCol::PickTerrain(&pivot, &dir, &out)
            └─ for each face:
                D3DXIntersectTri(v0, v1, v2, pos, dir, &fU, &fV, &fDist)
                  → 가장 가까운 교차점 반환
```

---

## 4. 사용된 디자인 패턴

### 4.1 싱글톤 매니저

```
CCollisionManager: DECLARE_SINGLETON / IMPLEMENT_SINGLETON
CNavMgr:           DECLARE_SINGLETON / IMPLEMENT_SINGLETON
```

- `CCollisionManager`는 Client 레이어, `CNavMgr`는 Engine 레이어
- 둘 다 `CBase`를 상속하므로 레퍼런스 카운팅 + `DestroyInstance()`로 해제

### 4.2 프로토타입/Clone 패턴

```cpp
// CCollisionSphere — 컴포넌트로서 Clone 지원
CComponent* CCollisionSphere::Clone(void) {
    return new CCollisionSphere(*this);  // 복사 생성자
}

// 복사 시 m_pCollisionSphere(D3DX 메시)는 포인터 공유
// → 원본과 클론이 같은 메시를 참조 (얕은 복사)
```

원본(프로토타입)은 `CComponent_Manager`에 등록되고, 각 `CGameObject`가 `Clone()`으로 복제하여 사용.

### 4.3 태그 기반 그룹 충돌

```cpp
// CCollisionManager — 태그(문자열)로 충돌 그룹을 분류
unordered_map<const _tchar*, list<CComponent*>>  m_MapCollisionComponent;

// 등록
Add_Component(L"Tag_Monster_Body",  pMonsterBodySphere);
Add_Component(L"Tag_Player_Attack", pPlayerAttackSphere);

// 조회 — 특정 태그의 모든 구체를 가져와서 판정
list<CComponent*> targets = Find_Componentlist(L"Tag_Monster_Body");
for (auto& target : targets)
    if (myAttackSphere->Check_Collision((CCollisionSphere*)target)) ...
```

**중앙 매니저가 판정하는 것이 아님** — 매니저는 저장소 역할만, 판정은 각 오브젝트가 수행.

### 4.4 하프-엣지 NavMesh (이웃 연결)

```cpp
// NavMgr::LinkCell() — O(n²) 이웃 연결
for each cell:
    for each other_cell:
        if (cell의 AB 변 점 == other의 어떤 변 점)
            cell->SetNeighbor(NEIGHBOR_AB, other_cell)
            // 양방향 연결 (ComparePoint에서 상대도 설정)

// 결과: 각 셀이 최대 3개 이웃 포인터를 가짐
CNaviCell::m_pNeighbor[NEIGHBOR_AB] → 인접 셀 (또는 nullptr = 경계)
CNaviCell::m_pNeighbor[NEIGHBOR_BC] → ...
CNaviCell::m_pNeighbor[NEIGHBOR_CA] → ...
```

### 4.5 2D 법선 기반 영역 판정

```
CLine2D — NaviCell의 각 변을 2D로 투영 (XZ 평면)

InitLine():
  vDir = endPoint - startPoint
  vNormal = (-vDir.y, vDir.x)  ← 시계방향 90도 회전 = 바깥쪽 법선

CheckLinePass(moveEndPoint):
  dot(moveEndPoint - startPoint, normal) > 0 → 변의 바깥쪽

CheckIn(pos):
  세 변 모두 안쪽(dot ≤ 0) → 셀 내부에 있음
```

---

## 5. DirectX API 호출 지점과 래핑 방식

### 5.1 CCollisionSphere — 구체 메시 생성 + 디버그 렌더

```cpp
// 구체 메시 생성 (초기화 시 1회)
void Create_Sphere(_float fRadius, const _matrix* pBoneMatrix, _float fScale) {
    D3DXCreateSphere(m_pGraphicDev, m_fRadius, 20, 20, &m_pCollisionSphere, NULL);
    //                 ↑ DX9 디바이스  ↑ 반경  ↑ 분할수    ↑ ID3DXMesh*
}

// 디버그 렌더 (DEBUG 빌드만)
void Render_CollisionSphere() {
#ifdef _DEBUG
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);  // 와이어프레임
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorldCollision);    // 월드 행렬
    m_pCollisionSphere->DrawSubset(0);                                  // 메시 렌더
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);     // 복원
#endif
}
```

**래핑 방식**: DX9 API를 직접 호출. 별도의 래퍼 없이 `LPDIRECT3DDEVICE9`를 멤버로 보유.

### 5.2 CNaviCell — 평면 생성 + 디버그 라인

```cpp
// 평면 방정식 생성 (셀 초기화 시)
D3DXPlaneFromPoints(&m_tPlane, &m_vPoint[A], &m_vPoint[B], &m_vPoint[C]);

// 디버그 렌더 — D3DXLine으로 삼각형 와이어프레임
void Render_NavCell(LPD3DXLINE pLine) {
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    // 수동 View × Proj 변환 후 DrawTransform
    pLine->Begin();
    pLine->DrawTransform(vPoint, 4, &matIdentity, LineColor);
    pLine->End();
}
```

### 5.3 CMouseCol — 메시 클론 + 삼각형 교차

```cpp
// 메시 클론 (초기화 시) — 피킹용 단순화 메시
pMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_VTXPOS, m_pGraphicDev, &m_pCloneMesh);
m_pCloneMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&m_pVertex);
m_pCloneMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&m_pIndex);

// 월드 변환 적용 (로컬 → 월드)
for each vertex:
    D3DXVec3TransformCoord(&m_pVertex[i].vPos, &m_pVertex[i].vPos, &matWorld);

// 삼각형 교차 판정
D3DXIntersectTri(&v0, &v1, &v2, vecPos, vecDir, &fU, &fV, &fDist);
```

### 5.4 NavMesh 피킹

```cpp
// CCollisionManager::Pick_NavMesh — NavMesh 삼각형 직접 교차
D3DXIntersectTri(&navMesh[i].A, &navMesh[i].B, &navMesh[i].C,
                 &m_vPivotPos, &m_vRayDir, &fU, &fV, &fDist);
```

### DX API 사용 요약

| API | 사용처 | 용도 |
|-----|--------|------|
| `D3DXCreateSphere` | `CCollisionSphere::Create_Sphere` | 디버그 구체 메시 생성 |
| `D3DXVec3Length` | `CCollisionSphere::Check_*` | 구체 간 거리 계산 |
| `D3DXVec3Normalize` | `CCollisionSphere::Push_Collision` | 밀기 방향 정규화 |
| `D3DXPlaneFromPoints` | `CNaviCell::InitCell` | 삼각형 → 평면 방정식 |
| `D3DXIntersectTri` | `CMouseCol::PickTerrain`, `CollisionMgr::Pick_NavMesh` | 레이-삼각형 교차 |
| `D3DXVec2Dot` | `CLine2D::CheckLinePass` | 2D 법선 내적 (영역 판정) |
| `D3DXVec3TransformCoord/Normal` | `CCollisionManager::Translation_*` | 좌표계 변환 |
| `D3DXMatrixInverse` | `CCollisionManager::Translation_Local` | 월드 역행렬 |
| `SetRenderState`, `SetTransform`, `DrawSubset` | `CCollisionSphere::Render` | 디버그 렌더 |
| `D3DXCreateLine`, `DrawTransform` | `CNavMgr/CNaviCell` | NavMesh 디버그 렌더 |
| `CloneMeshFVF`, `Lock/UnlockVertexBuffer` | `CMouseCol::Initialize` | 피킹용 메시 복제 |

---

## 6. NavMesh 옵션 시스템

### 비트 플래그 기반 옵션

```cpp
// Engine_Enum.h
enum NAV_OPTION {
    NAV_PLAYER_COLLISION,  // 0 — 플레이어 충돌 영역
    NAV_TOWER_ENABLE,      // 1 — 타워 건설 가능
    NAV_WALL,              // 2 — 벽
    NAV_SECOND_FLOOR,      // 3 — 2층
    NAV_UPFLOOR,           // 4 — 계단 (층 전환 가능)
    NAV_DROP,              // 5 — 낙하 가능
    NAV_NOPASSAGE,         // 6 — 통과 불가
    NAV_DEATH,             // 7 — 사망 영역
    NAV_OPTION_END
};

// Engine_Constant.h — 비트마스크
const DWORD NAV_OPTION_PLAYER_COLLISION = (DWORD)powf(2, NAV_PLAYER_COLLISION); // 1
const DWORD NAV_OPTION_TOWER_ENABLE     = (DWORD)powf(2, NAV_TOWER_ENABLE);     // 2
const DWORD NAV_OPTION_DROP             = (DWORD)powf(2, NAV_DROP);             // 32
const DWORD NAV_OPTION_NOPASSAGE        = (DWORD)powf(2, NAV_NOPASSAGE);       // 64
const DWORD NAV_OPTION_DEATH            = (DWORD)powf(2, NAV_DEATH);           // 128
```

### 옵션별 동작

```
MoveOnNavMesh() 내부 분기:

  셀 경계 넘음 + 이웃 없음 (경계):
    ├─ NAV_OPTION_DROP → Drop()       : dwIndex = -1, 자유 낙하
    └─ 그 외            → Slide_Vector(): 벽 따라 미끄러짐

  셀 경계 넘음 + 이웃 있음:
    └─ 이웃이 NOPASSAGE → Slide_Vector(): 통과 불가, 벽처럼 처리

  FallOnNavMesh():
    └─ NAV_OPTION_DEATH → bDead = true : 사망 처리

  FindSecondFloor():
    └─ NAV_OPTION_SECOND_FLOOR 셀만 대상으로 검색

  IsAbleToUpStairs():
    └─ NAV_OPTION_UPFLOOR → true : 층 전환 가능
```

---

## 7. 2층 NavMesh 구현

```
CNavMgr::AddCell()
  ├─ m_vecNaviMesh.push_back(pCell)           ← 전체 목록
  └─ if (NAV_OPTION_SECOND_FLOOR)
      m_vecNaviMeshSecondFloor.push_back(pCell) ← 2층 전용 목록 (같은 포인터)

CNavMgr::FindSecondFloor(vPos, dwCurrentIdx)
  └─ m_vecNaviMeshSecondFloor만 순회
      ├─ 평면과의 거리가 0 이상 (위에 있음)
      ├─ CheckIn으로 셀 내부 확인
      └─ 가장 가까운 셀의 인덱스 반환

사용 예:
CPlayer::Update()
  if (CNavMgr::IsAbleToUpStairs(dwCurrentIdx))
      dwCurrentIdx = CNavMgr::FindSecondFloor(vPos, dwCurrentIdx)
```

---

## 8. 프레임워크 참고 설계 판단

### 8.1 채택할 만한 설계

**태그 기반 충돌 그룹**
- `unordered_map<tag, list<CSphere*>>`으로 그룹별 관리
- "Tag_Player_Attack" vs "Tag_Monster_Body" 같은 의미 있는 태그
- 판정 시 특정 그룹만 조회 → N×M 전수 비교 회피

**NavMesh 이웃 연결 + 2D 법선 판정**
- `CLine2D`의 법선 내적으로 셀 내부/외부를 O(1)에 판정
- 이웃 포인터로 셀 간 이동을 O(1)에 처리
- `CheckPass` → `GetNeighbor` → 다음 셀 인덱스 반환하는 흐름이 깔끔

**슬라이드 벡터 구현**
- `vecSlide = dir - normal * dot(normal, dir)` — 정석적인 벽 미끄러짐
- 슬라이드 후에도 재검사(`CheckNavPass`)하여 모서리 처리까지 완벽

**NavMesh 옵션 비트 플래그**
- 하나의 셀에 복수 속성 부여 가능 (2층 + 타워 건설 가능 등)
- `MoveOnNavMesh` 내부에서 옵션에 따라 분기 → 하나의 함수로 모든 이동 처리

**구체 충돌의 뼈 행렬 연동**
- `m_matWorldCollision = BoneMatrix × WorldMatrix`
- 충돌 구가 캐릭터의 특정 뼈(손, 무기 등)를 추적
- 전투 게임에 필수적인 부위별 판정 지원

**2단계 높이 보정**
- `MoveOnNavMesh`: XZ 평면 이동 처리 (셀 전환, 슬라이드)
- `FallOnNavMesh`: Y축 높이 보정 (평면 방정식으로 지면 높이 계산)
- 분리함으로써 각 함수의 책임이 명확

### 8.2 개선 여지가 있는 부분

**충돌 판정이 중앙화되지 않음**
```
현재: 각 GameObject가 직접 Find_Componentlist() → 루프 돌며 Check_Collision()
     → 중복 판정 가능 (A가 B 체크 + B가 A 체크)
     → 판정 순서가 오브젝트 Update 순서에 의존

개선안: CollisionManager에서 프레임 시작 시 일괄 판정
       → 콜백/이벤트로 결과 전달
```

**NavMesh LinkCell()이 O(n²)**
```cpp
// 모든 셀 × 모든 셀 비교
for each cell:
    for each other_cell:
        ComparePoint(...)

개선안: 점 좌표를 키로 하는 맵을 구축하여 O(n) 연결 가능
```

**CCollisionManager에서 구체를 Release하지 않음**
```cpp
void Free(void) {
    for (...) iter->second.clear();  // list만 비움, Safe_Release 안 함
}
// → 소유권이 GameObject에 있으므로 정상이지만,
//    GameObject가 먼저 해제되면 댕글링 포인터 위험
```

**_tchar* 포인터를 unordered_map 키로 사용**
```cpp
unordered_map<const _tchar*, list<CComponent*>>  m_MapCollisionComponent;
// → 포인터 비교이므로 같은 문자열이라도 다른 주소면 다른 키로 취급
// → 리터럴 문자열은 보통 같은 주소지만, 동적 생성 시 문제 가능
// 개선안: wstring을 키로 사용
```

**디버그 렌더가 #ifdef _DEBUG로만 제어**
```
CCollisionSphere::Render_CollisionSphere()
  → #ifdef _DEBUG 블록 안에서만 동작
  → Release 빌드에서는 디버그 렌더 불가능

CollisionManager::m_bRender는 별도 토글이 있지만,
CCollisionSphere 내부 코드가 #ifdef로 컴파일 제외됨
```

### 8.3 내 프레임워크(DX9) 적용 시 고려사항

| 참고프로젝트3 | 내 프레임워크 | 적용 방향 |
|--------------|--------------|-----------|
| `CCollisionSphere` (구체만) | 미구현 | 구체 + AABB + OBB 계층 구조 고려 |
| 태그 기반 그룹 관리 | 미구현 | `unordered_map<wstring, vector<>>` 도입 |
| 판정은 각 오브젝트가 수행 | - | 중앙 매니저에서 일괄 판정으로 개선 |
| `CNavMgr` (셀 기반) | 미구현 | NavMesh 에디터 + 런타임 이동 시스템 구축 |
| `CLine2D` (2D 법선 내적) | 미구현 | 동일 방식 도입 — 가장 효율적인 영역 판정 |
| `FallOnNavMesh` (높이 보정) | 미구현 | 평면 방정식 기반 Y좌표 보정 구현 |
| `D3DXCreateSphere` (디버그) | 미구현 | 디버그 구체/박스 메시 공유 풀 구축 |
| 비트 플래그 옵션 | 미구현 | NavMesh 셀별 옵션 시스템 도입 |
| 레이 피킹 | 미구현 | `D3DXIntersectTri` 기반 피킹 시스템 |

### 8.4 권장 구현 우선순위

```
1단계: 구체 충돌 (CCollisionSphere 수준)
  → 게임 오브젝트에 구체 컴포넌트 부착
  → 태그 기반 그룹 + 중앙 매니저 판정

2단계: NavMesh 이동
  → CNaviCell + CLine2D (2D 법선 판정)
  → MoveOnNavMesh + FallOnNavMesh

3단계: 마우스 피킹
  → 뷰→로컬 좌표 변환 + D3DXIntersectTri

4단계: 슬라이드 + 옵션 시스템
  → 벽 미끄러짐, 낙하, 사망 영역 등
```

---

## 부록: 충돌 판정 수학 정리

### 구체-구체 교차

```
조건: distance(center1, center2) < radius1 + radius2

코드: D3DXVec3Length(&(vPos1 - vPos2)) < r1 * scale1 + r2 * scale2
```

### 2D 법선 내적 (Line2D)

```
변: Start → End
법선: Normal = Rotate90CW(End - Start) = (-dy, dx) → Normalize

판정: dot(Point - Start, Normal) > 0 → 변의 바깥쪽 (셀 밖)

세 변 모두 ≤ 0 → 셀 내부
```

### 평면 방정식 높이 보정

```
평면: ax + by + cz + d = 0
높이: y = (-ax - cz - d) / b

사용: FallOnNavMesh에서 캐릭터 Y좌표를 지면 높이로 보정
```

### 슬라이드 벡터

```
벽 법선: N
이동 벡터: D

슬라이드: S = D - N * dot(N, D)
  → D에서 N 방향 성분을 제거 = 벽 평행 성분만 남김
```

### 레이-삼각형 교차 (D3DXIntersectTri)

```
입력: 삼각형 (V0, V1, V2), 레이 원점 P, 방향 D
출력: U, V (무게중심 좌표), fDist (교차 거리)

교차점: P + D * fDist
      = V0 + U * (V1 - V0) + V * (V2 - V0)
```

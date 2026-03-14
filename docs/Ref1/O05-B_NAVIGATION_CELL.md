# O05-B: Navigation Mesh + Cell 이동 판정 / 높이 보간

## 1. 전체 구조

```
CNavigation (CComponent)          ← 네비게이션 컴포넌트
  ├── vector<CCell*> m_Cells      ← 삼각형 셀 배열
  ├── m_iCurrentCellIndex         ← 현재 위치한 셀 인덱스
  └── static m_pParentMatrix      ← 지형 월드 행렬 (전역 공유)

CCell (CBase)
  ├── m_vPoints[3]                ← 삼각형 꼭짓점 (A, B, C)
  ├── m_vNormals[3]               ← 각 변의 안쪽 법선
  └── m_iNeighborIndices[3]       ← AB/BC/CA 변의 이웃 셀 인덱스
```

**네비게이션 메시(NavMesh)**: 이동 가능 영역을 삼각형 집합으로 표현.
캐릭터가 삼각형 안에 있으면 이동 허용, 밖이면 차단.

---

## 2. 셀 데이터 — CCell

### 삼각형 정의

```cpp
enum class POINT { A, B, C, END };
enum class LINE  { AB, BC, CA, END };

_float3 m_vPoints[3];    // 삼각형 꼭짓점 (XZ 평면 기준)
_float3 m_vNormals[3];   // 각 변의 안쪽 법선 (XZ 평면)
_int    m_iNeighborIndices[3] = { -1, -1, -1 };  // 이웃 셀 (-1 = 없음)
```

### 변 법선 계산

```cpp
// AB 변의 안쪽 법선 (XZ 평면 90도 회전)
m_vNormals[AB] = _float3(
    -(B.z - A.z),   // -dz
    0.f,
    B.x - A.x       //  dx
);
```

**2D 법선 공식**: 벡터 `(dx, dz)`의 왼쪽 수직 벡터 = `(-dz, dx)`.
삼각형이 **반시계 방향(CCW)**으로 정의되었을 때, 이 법선은 삼각형 안쪽을 가리킨다.

```
        B
       /|
      / |
     /  |← AB 변
    /   |
   A────C

AB 변의 법선: 삼각형 안쪽(→ 방향)을 가리킴
```

---

## 3. isIn — 점이 삼각형 내부인지 판정

```cpp
_bool CCell::isIn(_fvector vResultPos, _int* pNeighborIndex)
{
    for (size_t i = 0; i < 3; i++)  // AB, BC, CA 각 변에 대해
    {
        _vector vNormal = Normalize(m_vNormals[i]);
        _vector vDir = Normalize(vResultPos - m_vPoints[i]);

        // 법선과 방향의 내적 > 0 → 변의 바깥쪽
        if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
        {
            *pNeighborIndex = m_iNeighborIndices[i];  // 넘어간 변의 이웃
            return false;
        }
    }
    return true;  // 모든 변의 안쪽 → 내부에 있음
}
```

**알고리즘**: 각 변의 안쪽 법선과 (꼭짓점→점) 벡터의 내적이 모두 ≤ 0이면 내부.
하나라도 > 0이면 해당 변을 넘어선 것이며, 그 변의 이웃 셀 인덱스를 반환한다.

```
시각화 (XZ 평면 위에서 내려다 봄):

    A ─────── B
     \       /
  CA변\  ●  / AB변   ← ● 지점에서 모든 변까지 내적 ≤ 0
       \   /            → isIn = true
        \ /
         C
        BC변

    A ─────── B
     \       /
      \     / AB변
       \   /
        \ ★              ← ★ 지점에서 AB 법선과 내적 > 0
         C                → isIn = false, 이웃 = AB의 이웃 셀
```

---

## 4. CNavigation — 이동 제어

### 초기화 — 바이너리 파일 로딩

```cpp
HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile)
{
    HANDLE hFile = CreateFile(pNavigationDataFile, GENERIC_READ, ...);

    _float3 vPoints[3] = {};
    while (true)
    {
        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
        if (0 == dwByte) break;

        CCell* pCell = CCell::Create(pDevice, pContext, vPoints, m_Cells.size());
        m_Cells.push_back(pCell);
    }

    SetUp_Neighbors();   // 이웃 관계 설정
}
```

**파일 포맷**: `_float3[3]` (A, B, C 꼭짓점 36바이트)이 반복. EOF까지 읽는다.

### 이웃 설정 — SetUp_Neighbors

```cpp
void CNavigation::SetUp_Neighbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell) continue;

            // Sour의 AB 변 꼭짓점 두 개가 Dest 셀에도 있으면 → AB의 이웃
            if (pDestCell->Compare_Points(pSourCell->Get_Point(A), pSourCell->Get_Point(B)))
                pSourCell->Set_Neighbor(LINE::AB, pDestCell);

            if (pDestCell->Compare_Points(pSourCell->Get_Point(B), pSourCell->Get_Point(C)))
                pSourCell->Set_Neighbor(LINE::BC, pDestCell);

            if (pDestCell->Compare_Points(pSourCell->Get_Point(C), pSourCell->Get_Point(A)))
                pSourCell->Set_Neighbor(LINE::CA, pDestCell);
        }
    }
}
```

**Compare_Points**: 두 점이 한 셀의 꼭짓점 중 2개와 일치하면 `true`.
순서는 무관하다 (A-B도 B-A도 매칭). 시간 복잡도: `O(N²)` (초기화 1회).

---

## 5. isMove — 이동 허용 판정

```cpp
_bool CNavigation::isMove(_fvector vResultPos)
{
    if (-1 == m_iCurrentCellIndex) return false;

    // ① 월드 좌표 → 네비 로컬 좌표 변환
    _vector vPosition = XMVector3TransformCoord(
        vResultPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    _int iNeighborIndex = -1;

    // ② 현재 셀 내부인가?
    if (m_Cells[m_iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex))
        return true;  // 그대로 이동 허용

    // ③ 외부로 나갔는데 이웃이 있다면
    if (-1 != iNeighborIndex)
    {
        // ④ 이웃 체인 따라가기 (고속 이동 시 여러 셀 건너뛸 수 있음)
        for (;;)
        {
            if (m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
                break;           // 이 셀 안에 있음
            if (-1 == iNeighborIndex)
                return false;    // 더 이상 이웃 없음 → 이동 불가
        }
        m_iCurrentCellIndex = iNeighborIndex;  // 셀 전환
        return true;
    }
    else
        return false;  // 이웃 없음 → 맵 경계, 이동 불가
}
```

### 흐름 다이어그램

```
vResultPos (새 위치)
    │
    ▼
현재 셀 안? ─── YES → return true
    │ NO
    ▼
이웃 있음? ─── NO → return false (경계)
    │ YES
    ▼
이웃 셀 안? ─── YES → 셀 전환, return true
    │ NO
    ▼
그 이웃 있음? ── NO → return false
    │ YES
    └── 반복... (for(;;) 루프)
```

**for(;;) 루프**: 한 프레임에 여러 셀을 건너뛰는 고속 이동을 처리한다.
이웃 체인을 따라가다가 포함되는 셀을 찾거나, 이웃이 없으면 실패한다.

### Transform 연동

```cpp
// CTransform::Go_Straight
void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
    _vector vPosition = Get_State(STATE::POSITION);
    _vector vLook = Get_State(STATE::LOOK);

    vPosition += Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    // Navigation이 없거나, 이동 가능하면 위치 갱신
    if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
        Set_State(STATE::POSITION, vPosition);
    // 이동 불가면 위치 변경 안 함 (벽에 막힘)
}
```

---

## 6. Compute_Height — 높이 보간

```cpp
_float CCell::Compute_Height(_fvector vResultPos)
{
    // 삼각형 3개 점으로 평면 방정식 생성
    _float4 vPlane = {};
    XMStoreFloat4(&vPlane, XMPlaneFromPoints(
        XMLoadFloat3(&m_vPoints[A]),
        XMLoadFloat3(&m_vPoints[B]),
        XMLoadFloat3(&m_vPoints[C])));

    // ax + by + cz + d = 0  →  y = (-ax - cz - d) / b
    float fy = (-vPlane.x * X - vPlane.z * Z - vPlane.w) / vPlane.y;
    return fy;
}
```

**평면 방정식으로 Y값 계산**: XZ 좌표를 대입하면 그 지점의 정확한 높이를 얻는다.
지형이 경사져 있어도 삼각형 평면 위의 정확한 Y를 반환한다.

### SetUp_OnNavigation

```cpp
_vector CNavigation::SetUp_OnNavigation(_fvector vWorldPos)
{
    _vector vPosition = XMVector3TransformCoord(
        vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    return XMVectorSetY(vPosition,
        m_Cells[m_iCurrentCellIndex]->Compute_Height(vPosition));
}
```

**용도**: 캐릭터의 Y 좌표를 현재 셀의 지형 높이에 맞춘다.
이것으로 캐릭터가 경사면을 따라 자연스럽게 오르내린다.

---

## 7. 디버그 렌더링

```cpp
#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    m_pShader->Bind_Matrix("g_ViewMatrix", ...);
    m_pShader->Bind_Matrix("g_ProjMatrix", ...);

    if (-1 == m_iCurrentCellIndex)
    {
        // 네비게이션 없는 오브젝트: 전체 메시 초록색
        vColor = _float4(0, 1, 0, 1);
        for (auto& pCell : m_Cells)
            pCell->Render();
    }
    else
    {
        // 현재 셀만 빨간색, 약간 위로 (+0.1) 올려서 표시
        vColor = _float4(1, 0, 0, 1);
        WorldMatrix._42 += 0.1f;
        m_Cells[m_iCurrentCellIndex]->Render();
    }
}
#endif
```

**현재 셀 하이라이트**: 캐릭터가 위치한 셀만 빨간색으로 표시.
Y를 0.1 올려서 지형과 겹치지 않게 한다.

---

## 8. Clone 패턴 — 셀 공유

```
프로토타입:                     클론 A:            클론 B:
m_Cells[0] ←── AddRef ──── m_Cells[0]      m_Cells[0] ── AddRef ──→
m_Cells[1] ←── AddRef ──── m_Cells[1]      m_Cells[1] ── AddRef ──→
m_Cells[2] ←── AddRef ──── m_Cells[2]      m_Cells[2] ── AddRef ──→
...
iCurrentCellIndex = -1      iCurrentCellIndex = 5   iCurrentCellIndex = 12
```

**셀 데이터는 공유, 현재 위치만 독립**: 모든 클론이 같은 Cell 배열을 참조.
`m_iCurrentCellIndex`만 각 클론(캐릭터)이 독립적으로 관리한다.

**static m_pParentMatrix**: 지형의 월드 행렬. 모든 네비게이션 인스턴스가 같은
지형 위에 있으므로 정적 멤버로 공유한다. `Update()`에서 매 프레임 갱신.

---

## 9. 핵심 정리

| 항목 | 설명 |
|------|------|
| **NavMesh 구조** | 삼각형 셀 배열, 변별 이웃 인덱스, 바이너리 파일 로딩 |
| **isIn 판정** | 각 변의 안쪽 법선과 내적 ≤ 0 → 내부 |
| **이웃 설정** | O(N²) 브루트포스, Compare_Points로 공유 변 탐색 |
| **isMove** | 현재 셀 → 이웃 체인 탐색 → 경계 도달 시 이동 차단 |
| **높이 보간** | XMPlaneFromPoints → 평면 방정식 Y 역산 |
| **Transform 연동** | Go_Straight에서 isMove가 false면 위치 변경 안 함 |
| **Clone 공유** | Cell 배열 AddRef 공유, CurrentCellIndex만 독립 |
| **static Parent** | 지형 월드 행렬 전역 공유 (모든 오브젝트 동일 지형) |

# O04-C: Terrain 하이트맵 + QuadTree 컬링

## 1. 전체 구조 개요

```
CTerrain (Client GameObject)
  └── CVIBuffer_Terrain (Engine Component)
        ├── 하이트맵 BMP → 정점 생성
        ├── 노멀 계산 (인접 삼각형 평균)
        ├── DYNAMIC 인덱스 버퍼 (매 프레임 갱신)
        └── CQuadTree
              ├── 재귀 4분할 (LT, RT, RB, LB)
              ├── 거리 기반 LOD (isDraw)
              ├── 프러스텀 컬링 (isIn_LocalSpace)
              └── 이웃 노드로 T-Junction 방지
```

**매 프레임 흐름:**
```
Terrain::Update()
  → VIBuffer_Terrain::Culling(WorldMatrix)
      → Frustum::Transform_ToLocalSpace(WorldInverse)  // 절두체를 로컬로 변환
      → Map(IB, WRITE_DISCARD)                         // IB 잠금
      → QuadTree::Culling(...)                          // 보이는 삼각형만 수집
      → m_iNumIndices = 실제 수집된 수
      → Unmap(IB)
```

---

## 2. 하이트맵 로딩 — Initialize_Prototype

### BMP 파일 읽기

```cpp
HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, ...);

BITMAPFILEHEADER fh{};
ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);

BITMAPINFOHEADER ih{};
ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

_uint* pPixels = new _uint[ih.biWidth * ih.biHeight];
ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, ...);
```

**BMP 구조:**
- `BITMAPFILEHEADER` (14 bytes): 파일 시그니처, 오프셋
- `BITMAPINFOHEADER` (40 bytes): `biWidth` × `biHeight` = 지형 해상도
- 픽셀 데이터: 32비트 BGRA 형식

### 높이값 추출

```cpp
pVertices[iIndex].vPosition = _float3(j, (pPixels[iIndex] & 0x000000ff) / 10.0f, i);
```

| 연산 | 값 | 의미 |
|------|----|------|
| `pPixels[iIndex]` | `0xff151515` | 32비트 BGRA 픽셀 |
| `& 0x000000ff` | `0x15` = 21 | Blue 채널만 추출 (0~255) |
| `/ 10.0f` | 2.1 | 높이 스케일 조절 |

**좌표 매핑**: `j` = X좌표, `i` = Z좌표 → **X-Z 평면**에 정점 배치, Y가 높이.

### UV 좌표

```cpp
pVertices[iIndex].vTexcoord = _float2(
    j / (m_iNumVerticesX - 1.f),  // 0.0 ~ 1.0
    i / (m_iNumVerticesZ - 1.f)   // 0.0 ~ 1.0
);
```

전체 지형에 텍스처 1장이 매핑된다. 타일링이 필요하면 셰이더에서 UV를 곱한다.

---

## 3. 노멀 계산 — 인접 삼각형 평균

### 셀 당 2개 삼각형 인덱싱

```
iIndices[0] = iIndex + m_iNumVerticesX     (LB)
iIndices[1] = iIndex + m_iNumVerticesX + 1 (RB)
iIndices[2] = iIndex + 1                   (RT)
iIndices[3] = iIndex                       (LT)

셀 배치:
  [3]───[2]     LT───RT
   │   ╱ │       │  ╱  │
   │ ╱   │       │╱    │
  [0]───[1]     LB───RB

삼각형1: [0]-[1]-[2]  (아래쪽)
삼각형2: [0]-[2]-[3]  (위쪽)
```

### 노멀 누적 방식

```cpp
// 삼각형1의 노멀
vSour = Pos[1] - Pos[0];
vDest = Pos[2] - Pos[1];
vNormal = Normalize(Cross(vSour, vDest));

// 삼각형1에 속한 3개 정점에 노멀 누적 (+=)
pVertices[iIndices[0]].vNormal += vNormal;
pVertices[iIndices[1]].vNormal += vNormal;
pVertices[iIndices[2]].vNormal += vNormal;
```

모든 삼각형 처리 후 **정규화**:

```cpp
for (size_t i = 0; i < m_iNumVertices; i++)
    pVertices[i].vNormal = Normalize(pVertices[i].vNormal);
```

**원리**: 한 정점은 최대 6개 삼각형에 속할 수 있다. 각 삼각형의 면 노멀을 누적하고 정규화하면 **부드러운 음영(Smooth Shading)** 노멀이 된다.

---

## 4. DYNAMIC 인덱스 버퍼

### 생성

```cpp
D3D11_BUFFER_DESC IBDesc{};
IBDesc.Usage          = D3D11_USAGE_DYNAMIC;      // CPU 쓰기 가능
IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // Map 허용
IBDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
IBDesc.ByteWidth      = m_iNumIndices * m_iIndexStride;
```

| 항목 | 값 | 이유 |
|------|-----|------|
| Usage | `DYNAMIC` | 매 프레임 인덱스 갱신 |
| CPUAccessFlags | `WRITE` | CPU에서 IB에 쓰기 |
| 인덱스 포맷 | `R32_UINT` | 129×129=16641 정점 > 65535 |
| 최대 인덱스 수 | `128×128×2×3 = 98304` | 모든 삼각형 |

### 매 프레임 갱신 — Map/Unmap

```cpp
void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
    // ① 프러스텀을 지형 로컬 공간으로 변환
    m_pGameInstance->Transform_Frustum_ToLocalSpace(
        XMMatrixInverse(nullptr, WorldMatrix));

    // ② IB 잠금 (기존 내용 폐기)
    D3D11_MAPPED_SUBRESOURCE SubResource{};
    m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

    _uint* pIndices = static_cast<_uint*>(SubResource.pData);
    _uint  iNumIndices = 0;

    // ③ QuadTree가 보이는 삼각형의 인덱스만 pIndices에 기록
    m_pQuadTree->Culling(m_pGameInstance, m_pVertexPositions,
                         pIndices, &iNumIndices);

    // ④ 실제 그릴 인덱스 수 갱신
    m_iNumIndices = iNumIndices;

    // ⑤ IB 잠금 해제
    m_pContext->Unmap(m_pIB, 0);
}
```

**WRITE_DISCARD의 의미**: GPU가 이전 버퍼를 아직 사용 중이어도, 드라이버가 새 메모리를 할당하여 CPU가 즉시 쓸 수 있게 한다. GPU-CPU 동기화 대기 없이 성능을 유지한다.

---

## 5. QuadTree 구조

### 4분할 재귀

```cpp
HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
    m_iCornerIndices = { iLT, iRT, iRB, iLB };

    // 종료 조건: 인접 정점 (1칸짜리 셀)
    if (1 == iRT - iLT) return S_OK;

    // 중심 및 변 중점 인덱스 계산 (비트 시프트 = 평균)
    m_iCenterIndex = (iLT + iRB) >> 1;
    iLC = (iLT + iLB) >> 1;   // Left Center
    iTC = (iLT + iRT) >> 1;   // Top Center
    iRC = (iRT + iRB) >> 1;   // Right Center
    iBC = (iLB + iRB) >> 1;   // Bottom Center

    // 4개 자식 생성
    m_pChildren[LT] = Create(iLT, iTC, Center, iLC);
    m_pChildren[RT] = Create(iTC, iRT, iRC, Center);
    m_pChildren[RB] = Create(Center, iRC, iRB, iBC);
    m_pChildren[LB] = Create(iLC, Center, iBC, iLB);
}
```

### 인덱스 기반 공간 분할 시각화

```
129×129 지형 (인덱스 0 ~ 16640):

Root:
  LT=16512  ────  RT=16640     (마지막 행)
    │                 │
    │    Center=8320  │
    │                 │
  LB=0     ────  RB=128        (첫 행)

1단계 분할:
  LT 자식: (16512, 16576, 8320, 8256)
  RT 자식: (16576, 16640, 8384, 8320)
  RB 자식: (8320, 8384, 128, 64)
  LB 자식: (8256, 8320, 64, 0)
```

**왜 `>> 1` (비트 시프트)로 중점을 구하는가?**
정점이 격자에 등간격으로 배치되어 있으므로 두 인덱스의 평균이 곧 공간적 중점이다.

### 트리 깊이

129×129 지형 기준: `128 → 64 → 32 → 16 → 8 → 4 → 2 → 1` = **7레벨**
리프 노드는 2×2 정점 = 2개 삼각형을 가진다.

---

## 6. QuadTree 컬링 알고리즘

### Culling 함수 분기 로직

```
Culling(pIndices, pNumIndices) {
    if (자식 없음 || isDraw()==true) {
        // ← 리프이거나, 충분히 멀어서 더 세분화 불필요
        // → 이 노드의 삼각형을 직접 출력
        // → T-Junction 처리 포함
        return;
    }

    // 이 노드의 바운딩 구가 프러스텀 안이면
    if (isIn_Frustum_LocalSpace(Center, Radius)) {
        // 4개 자식에게 재귀
        for (child : children)
            child->Culling(...);
    }
    // 밖이면 → 아무것도 안 함 (전체 가지 컬링)
}
```

### isDraw — 거리 기반 LOD 판정

```cpp
_bool CQuadTree::isDraw(CGameInstance* pGI, const _float3* pVertexPositions)
{
    _float fCamDistance = Length(CamPos - CenterPos);
    _float fWidth = m_iCornerIndices[RT] - m_iCornerIndices[LT];

    return fCamDistance * 0.2f > fWidth;
}
```

| 조건 | 의미 |
|------|------|
| `fCamDistance * 0.2f > fWidth` | **참**: 충분히 멀다 → 이 노드를 직접 그린다 |
| `fCamDistance * 0.2f ≤ fWidth` | **거짓**: 가깝다 → 자식으로 세분화 |

**예시 (128폭 노드)**:
- 카메라 거리 640 이하: `640*0.2=128` → 세분화
- 카메라 거리 641 이상: `641*0.2=128.2` > 128 → 이 노드에서 그림

**0.2 상수**: LOD 전환 감도를 제어한다. 값이 작을수록 더 가까이 가야 세분화된다.

### 프러스텀 바운딩 구 테스트

```cpp
_float fRadius = Length(CenterPos - CornerLT_Pos);

if (isIn_Frustum_LocalSpace(CenterPos, fRadius))
{
    for (child : children)
        child->Culling(...);
}
```

노드의 **중심→꼭짓점 거리**를 반지름으로 하는 구가 프러스텀 안이면 자식을 탐색한다.
밖이면 전체 하위 트리를 건너뛴다 (가지 컬링).

---

## 7. T-Junction 방지 메커니즘

### T-Junction이란?

인접한 두 QuadTree 노드의 LOD가 다를 때 발생하는 **균열(crack)** 문제.

```
고해상도 노드         │  저해상도 노드
  A───B───C           │     D───────E
  │ ╲ │ ╱ │           │     │       │
  │   M   │           │     │       │
  │ ╱ │ ╲ │           │     │       │
  F───G───H           │     I───────J
```

B 정점이 고해상도 쪽에만 존재하면, 저해상도 쪽 변(D-E)에 정렬되지 않아 틈이 생긴다.

### 이웃 노드 설정 — SetUp_Neighbors

```cpp
void CQuadTree::SetUp_Neighbors()
{
    // 종료 조건: 자식의 자식이 없으면 (리프 직전 레벨)
    if (nullptr == m_pChildren[0]->m_pChildren[0]) return;

    // ① 형제 간 이웃 설정 (같은 부모의 자식들)
    m_pChildren[LT]->m_pNeighbors[RIGHT]  = m_pChildren[RT];
    m_pChildren[LT]->m_pNeighbors[BOTTOM] = m_pChildren[LB];
    m_pChildren[RT]->m_pNeighbors[LEFT]   = m_pChildren[LT];
    m_pChildren[RT]->m_pNeighbors[BOTTOM] = m_pChildren[RB];
    m_pChildren[RB]->m_pNeighbors[LEFT]   = m_pChildren[LB];
    m_pChildren[RB]->m_pNeighbors[TOP]    = m_pChildren[RT];
    m_pChildren[LB]->m_pNeighbors[RIGHT]  = m_pChildren[RB];
    m_pChildren[LB]->m_pNeighbors[TOP]    = m_pChildren[LT];

    // ② 사촌 간 이웃 설정 (부모의 이웃의 자식)
    if (m_pNeighbors[RIGHT]) {
        m_pChildren[RT]->m_pNeighbors[RIGHT] = m_pNeighbors[RIGHT]->m_pChildren[LT];
        m_pChildren[RB]->m_pNeighbors[RIGHT] = m_pNeighbors[RIGHT]->m_pChildren[LB];
    }
    // LEFT, TOP, BOTTOM도 동일 패턴...

    // ③ 자식에게 재귀
    for (auto& pChild : m_pChildren)
        pChild->SetUp_Neighbors();
}
```

**핵심**: 형제 이웃은 같은 부모 안에서 직접 설정. 사촌 이웃은 **부모의 이웃 포인터**를 통해 접근.

```
┌──────┬──────┐
│ LT   │  RT  │  ← 부모 A
├──────┼──────┤
│ LB   │  RB  │
└──────┴──────┘
               ┌──────┬──────┐
               │ LT   │  RT  │  ← 부모 B (A의 RIGHT 이웃)
               ├──────┼──────┤
               │ LB   │  RB  │
               └──────┴──────┘

A.RT의 RIGHT 이웃 = B.LT  (사촌)
A.RB의 RIGHT 이웃 = B.LB  (사촌)
```

### 컬링 시 T-Junction 처리

```cpp
// 이웃 4개의 isDraw 상태 확인
for (size_t i = 0; i < 4; i++)
    if (m_pNeighbors[i] != nullptr)
        isDraw[i] = m_pNeighbors[i]->isDraw(...);

// CASE 1: 모든 이웃이 같은 LOD → 일반 2 삼각형
if (isDraw[0] && isDraw[1] && isDraw[2] && isDraw[3]) {
    출력: [LT, RT, RB] + [LT, RB, LB]  // 2삼각형
    return;
}

// CASE 2: LOD가 다른 이웃 존재 → 변 중점 추가하여 세분화
```

**CASE 2 세분화 예시 (LEFT 이웃이 저해상도):**

```
일반 (2삼각형):          T-Junction 보정 (4삼각형):
 LT────RT                LT────RT
  │  ╱  │                 │  ╱  │
  │╱    │                LC╱    │
  LB────RB                │╲    │
                          LB────RB

LEFT 변을 Center 경유로 2분할:
  [LT, Center, LC] + [LC, Center, LB]  ← 4삼각형
  대신: [LT, Center, LB] (일반 1삼각형)
```

코드에서 `isDraw[NEIGHBOR::LEFT] == false`이면:

```cpp
// 왼쪽 변 세분화 (2삼각형으로 분할)
pIndices[(*pNum)++] = LT;
pIndices[(*pNum)++] = Center;
pIndices[(*pNum)++] = iLC;        // Left Center (중점)

pIndices[(*pNum)++] = iLC;
pIndices[(*pNum)++] = Center;
pIndices[(*pNum)++] = LB;
```

`isDraw[NEIGHBOR::LEFT] == true`이면:

```cpp
// 일반 1삼각형
pIndices[(*pNum)++] = LT;
pIndices[(*pNum)++] = Center;
pIndices[(*pNum)++] = LB;
```

---

## 8. 로컬 공간 컬링의 이점

### 왜 월드가 아닌 로컬에서 컬링하는가?

```cpp
// VIBuffer_Terrain::Culling
m_pGameInstance->Transform_Frustum_ToLocalSpace(XMMatrixInverse(nullptr, WorldMatrix));
```

```cpp
// Frustum::Transform_ToLocalSpace
void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
    for (size_t i = 0; i < 8; i++)
        vPoints[i] = XMVector3TransformCoord(m_vWorldPoints[i], WorldMatrixInverse);
    Make_Planes(vPoints, m_vLocalPlanes);
}
```

| 방식 | 변환 대상 | 연산 횟수 |
|------|----------|----------|
| 월드 컬링 | 16641개 정점 × WorldMatrix | 16641회 |
| 로컬 컬링 | 프러스텀 8개 점 × WorldInverse | **8회** |

**프러스텀을 로컬로 가져오면 정점 변환이 불필요**해져 성능이 크게 향상된다.

---

## 9. CTerrain (Client) — 사용 측

```cpp
void CTerrain::Update(_float fTimeDelta)
{
    // 매 프레임 컬링 (IB 동적 갱신)
    m_pVIBufferCom->Culling(m_pTransformCom->Get_WorldMatrix());
}

void CTerrain::Render()
{
    // WVP 행렬 바인딩
    m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix());
    m_pShaderCom->Bind_Matrix("g_ViewMatrix", ...);
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", ...);

    // 텍스처 바인딩 (Diffuse + Mask + Brush)
    m_pTextureCom[DIFFUSE]->Bind_ShaderResource(..., 0);
    m_pTextureCom[MASK]->Bind_ShaderResource(..., 0);
    m_pTextureCom[BRUSH]->Bind_ShaderResource(..., 0);

    // 셰이더 패스 실행 + VB/IB 바인딩 + DrawIndexed
    m_pShaderCom->Begin(0);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();  // DrawIndexed(m_iNumIndices, 0, 0)
}
```

**m_iNumIndices가 매 프레임 달라진다**: Culling에서 갱신된 인덱스 수만큼만 DrawIndexed가 호출된다.

---

## 10. 핵심 정리

| 항목 | 설명 |
|------|------|
| **하이트맵** | BMP Blue 채널 /10.0 → Y 높이, 격자 X-Z 배치 |
| **노멀** | 인접 삼각형 면 노멀 누적 후 정규화 (Smooth Shading) |
| **IB Usage** | `DYNAMIC` + `WRITE_DISCARD` → 매 프레임 GPU 비동기 갱신 |
| **QuadTree 분할** | 인덱스 `>>1` 로 중점 계산, 2×2 셀까지 재귀 (7레벨) |
| **LOD 판정** | `CamDist * 0.2 > Width` → 충분히 멀면 이 노드에서 그림 |
| **프러스텀 컬링** | 바운딩 구 테스트로 전체 가지 제거, 로컬 공간에서 수행 |
| **T-Junction** | 이웃 LOD 차이 시 변 중점 추가하여 균열 방지 |
| **이웃 설정** | 형제(같은 부모) + 사촌(부모 이웃의 자식) 재귀 설정 |

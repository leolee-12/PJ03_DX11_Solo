# O04-D: GPU 인스턴싱 + 파티클 동적 갱신

## 1. 인스턴싱 개념

**인스턴싱(Instancing)**: 동일한 메시를 **1회의 드로우 콜**로 수백~수천 개 그리는 기법.
각 인스턴스는 위치·스케일·색상 등이 다를 수 있다.

```
일반 렌더링:             인스턴싱 렌더링:
  DrawIndexed() ×500       DrawIndexedInstanced() ×1
  = 500 드로우 콜           = 1 드로우 콜 (500 인스턴스)
```

**DX11에서의 구현**: 2개의 버텍스 버퍼를 IA에 동시 바인딩.
- **슬롯 0**: 공유 지오메트리 (정점 위치, UV) — 모든 인스턴스 동일
- **슬롯 1**: 인스턴스 데이터 (월드 행렬, 수명 등) — 인스턴스마다 다름

---

## 2. 클래스 계층

```
CVIBuffer (Engine)
  └── CVIBuffer_Instancing (abstract)
        ├── CVIBuffer_Rect_Instancing   ← 사각형 빌보드 파티클 (눈)
        └── CVIBuffer_Point_Instancing  ← 포인트 파티클 (폭발)
```

### INSTANCE_DESC — 기본 설정

```cpp
struct INSTANCE_DESC {
    _uint   iNumInstance;  // 인스턴스 수
    _float2 vScale;        // 스케일 범위 (min, max)
    _float3 vCenter;       // 생성 중심점
    _float3 vRange;        // 생성 범위 (Center ± Range/2)
};
```

### RECT_INSTANCE_DESC — 확장 설정

```cpp
struct RECT_INSTANCE_DESC : public INSTANCE_DESC {
    _float3 vPivot;     // Spread 기준점
    _float2 vSpeed;     // 속도 범위 (min, max)
    _float2 vLifeTime;  // 수명 범위 (min, max)
    _bool   isLoop;     // 수명 만료 시 리셋 여부
};
```

---

## 3. VTXINSTANCEPARTICLE — 인스턴스 정점 구조

```cpp
typedef struct tagVertexInstanceParticle {
    XMFLOAT4 vRight, vUp, vLook, vTranslation;  // 4×4 행렬의 행 벡터
    XMFLOAT2 vLifeTime;                          // (최대수명, 경과시간)
} VTXINSTANCEPARTICLE;
```

**크기**: `4×16 + 8 = 72 bytes`

**vRight/vUp/vLook/vTranslation**: 이것은 **4×4 월드 행렬**의 4개 행이다.
셰이더에서 이를 조합하면 인스턴스별 월드 변환이 된다.

```
[ vRight.x   vRight.y   vRight.z   0 ]     // 행 0: Right 축
[ vUp.x      vUp.y      vUp.z      0 ]     // 행 1: Up 축
[ vLook.x    vLook.y    vLook.z    0 ]     // 행 2: Look 축
[ vTrans.x   vTrans.y   vTrans.z   1 ]     // 행 3: 위치
```

### InputLayout 선언

```cpp
// 슬롯 0: 정점 데이터 (PER_VERTEX_DATA)
{ "POSITION", 0, R32G32B32_FLOAT,    0,  0, PER_VERTEX_DATA, 0 },
{ "TEXCOORD", 0, R32G32_FLOAT,       0, 12, PER_VERTEX_DATA, 0 },

// 슬롯 1: 인스턴스 데이터 (PER_INSTANCE_DATA)
{ "TEXCOORD", 1, R32G32B32A32_FLOAT, 1,  0, PER_INSTANCE_DATA, 1 },  // vRight
{ "TEXCOORD", 2, R32G32B32A32_FLOAT, 1, 16, PER_INSTANCE_DATA, 1 },  // vUp
{ "TEXCOORD", 3, R32G32B32A32_FLOAT, 1, 32, PER_INSTANCE_DATA, 1 },  // vLook
{ "TEXCOORD", 4, R32G32B32A32_FLOAT, 1, 48, PER_INSTANCE_DATA, 1 },  // vTranslation
{ "TEXCOORD", 5, R32G32_FLOAT,       1, 64, PER_INSTANCE_DATA, 1 },  // vLifeTime
```

| 파라미터 | 의미 |
|---------|------|
| `InputSlot = 1` | 두 번째 VB 슬롯 |
| `InputSlotClass = PER_INSTANCE_DATA` | 인스턴스마다 한 번 읽힘 |
| `InstanceDataStepRate = 1` | 인스턴스 1개마다 다음 데이터로 진행 |

---

## 4. 듀얼 VB 바인딩

### Bind_Buffers (CVIBuffer_Instancing)

```cpp
HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
    ID3D11Buffer* pVertexBuffers[] = { m_pVB, m_pVBInstance };
    _uint iVertexStrides[] = { m_iVertexStride, m_iInstanceVertexStride };
    _uint iOffsets[] = { 0, 0 };

    // 2개 VB를 슬롯 0, 1에 동시 바인딩
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, // = 2
        pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitive);
}
```

```
슬롯 0 (VB):            슬롯 1 (Instance VB):
┌─────────────┐          ┌────────────────────────┐
│ Pos, UV     │ ×4개     │ Right,Up,Look,Trans,LT │ ×N개
│ (공유 쿼드) │          │ (인스턴스별 행렬+수명)  │
└─────────────┘          └────────────────────────┘
```

---

## 5. 드로우 콜 — Rect vs Point

### Rect: DrawIndexedInstanced

```cpp
// CVIBuffer_Instancing::Render (Rect용)
m_pContext->DrawIndexedInstanced(
    m_iIndexCountPerInstance,  // 6 (쿼드당 인덱스 수)
    m_iNumInstance,            // N (인스턴스 수)
    0,                         // StartIndexLocation
    0,                         // BaseVertexLocation
    0);                        // StartInstanceLocation
```

**내부 동작**: GPU가 동일한 6개 인덱스 지오메트리를 N번 반복하되,
각 반복마다 슬롯 1에서 다음 인스턴스 데이터를 읽는다.

### Point: DrawInstanced

```cpp
// CVIBuffer_Point_Instancing::Render
m_pContext->DrawInstanced(
    1,                // 정점 1개
    m_iNumInstance,   // N 인스턴스
    0,                // StartVertexLocation
    0);               // StartInstanceLocation
```

**포인트 파티클은 인덱스 버퍼가 없다** (IB = nullptr).
1개 정점 × N 인스턴스. 지오메트리 셰이더에서 빌보드로 확장하거나,
셰이더에서 포인트 스프라이트로 처리한다.

---

## 6. 인스턴스 초기화 — 랜덤 분포

```cpp
for (size_t i = 0; i < m_iNumInstance; i++)
{
    // 랜덤 스케일 (균일 스케일링)
    _float fScale = Random(vScale.x, vScale.y);
    m_pInstanceVertices[i].vRight = _float4(fScale, 0, 0, 0);
    m_pInstanceVertices[i].vUp    = _float4(0, fScale, 0, 0);
    m_pInstanceVertices[i].vLook  = _float4(0, 0, fScale, 0);

    // 랜덤 위치 (Center ± Range/2)
    m_pInstanceVertices[i].vTranslation = _float4(
        Random(Center.x - Range.x*0.5, Center.x + Range.x*0.5),
        Random(Center.y - Range.y*0.5, Center.y + Range.y*0.5),
        Random(Center.z - Range.z*0.5, Center.z + Range.z*0.5),
        1.f);

    // 랜덤 속도 + 랜덤 수명
    m_pSpeeds[i] = Random(vSpeed.x, vSpeed.y);
    m_pInstanceVertices[i].vLifeTime = _float2(
        Random(vLifeTime.x, vLifeTime.y), 0.0f);
}
```

**랜덤 범위 파라미터화**: 스케일, 위치, 속도, 수명 모두 **min~max 범위**를 DESC로 받아
랜덤하게 초기화한다. 파티클마다 개성이 생긴다.

---

## 7. Prototype vs Clone — 인스턴스 VB 소유권

### 프로토타입 (Create)

```
Initialize_Prototype:
  - VB 생성 (DEFAULT, 공유)
  - IB 생성 (DEFAULT, 공유)
  - m_pInstanceVertices 배열 할당 (CPU, 원본 소유)
  - m_pSpeeds 배열 할당 (CPU, 원본 소유)
  - m_InstanceDesc 저장 (BufferDesc)
  ※ m_pVBInstance는 여기서 생성하지 않음!
```

### 클론 (Clone → Initialize)

```
복사 생성자:
  - VB, IB 공유 (AddRef via CVIBuffer)
  - m_pInstanceVertices 포인터 공유 (원본 데이터 참조)
  - m_pSpeeds 포인터 공유

Initialize:
  - m_pVBInstance GPU 버퍼 새로 생성! (DYNAMIC)
  - 공유된 m_pInstanceVertices를 초기 데이터로 사용
```

**핵심**: 인스턴스 VB는 **클론마다 독립적으로 생성**된다.
각 클론이 독립적으로 파티클 위치를 갱신해야 하기 때문이다.

```
프로토타입:     클론 A:            클론 B:
VB (공유) ←─── VB (AddRef)  ←─── VB (AddRef)
IB (공유) ←─── IB (AddRef)  ←─── IB (AddRef)
Vertices ←──── Vertices(공유) ──→ Vertices(공유)  // 초기값 참조
Speeds ←────── Speeds(공유)  ──→ Speeds(공유)
               VBInstance(독립)   VBInstance(독립)  // GPU 버퍼는 각자
```

### Free — 원본만 CPU 배열 해제

```cpp
void CVIBuffer_Rect_Instancing::Free() {
    __super::Free();  // VBInstance Release
    if (false == m_isCloned) {
        Safe_Delete_Array(m_pInstanceVertices);  // 원본만!
        Safe_Delete_Array(m_pSpeeds);
    }
}
```

---

## 8. 동적 갱신 — Drop / Spread

### Drop (눈 파티클)

```cpp
void CVIBuffer_Rect_Instancing::Drop(_float fTimeDelta)
{
    m_pContext->Map(m_pVBInstance, 0,
        D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXINSTANCEPARTICLE* pVertices = (VTXINSTANCEPARTICLE*)SubResource.pData;

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;  // 아래로 이동
        pVertices[i].vLifeTime.y += fTimeDelta;                      // 경과 시간 누적

        // 루프: 수명 만료 시 초기 위치로 리셋
        if (m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            pVertices[i].vLifeTime.y = 0.f;
            pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}
```

### Spread (폭발 파티클)

```cpp
void CVIBuffer_Rect_Instancing::Spread(_float fTimeDelta)
{
    Map(m_pVBInstance, WRITE_NO_OVERWRITE, ...);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        // 피벗에서 바깥으로 퍼지는 방향
        vMoveDir = Normalize(pVertices[i].vTranslation - m_vPivot);
        pVertices[i].vTranslation += vMoveDir * m_pSpeeds[i] * fTimeDelta;

        pVertices[i].vLifeTime.y += fTimeDelta;
        // 루프 리셋 동일...
    }

    Unmap(m_pVBInstance);
}
```

### WRITE_NO_OVERWRITE vs WRITE_DISCARD

| 플래그 | 동작 | 사용처 |
|--------|------|--------|
| `WRITE_DISCARD` | 이전 데이터 폐기, 새 메모리 할당 | Terrain IB (매 프레임 전체 재작성) |
| `WRITE_NO_OVERWRITE` | GPU가 읽지 않는 영역에만 쓰기 | 인스턴스 VB (위치만 수정, 크기 불변) |

**WRITE_NO_OVERWRITE가 더 가볍다**: 드라이버가 새 메모리를 할당하지 않아도 된다.
단, CPU와 GPU가 **동시에 같은 영역을 접근하지 않아야** 한다는 약속이 필요하다.

---

## 9. 클라이언트 사용 예시

### Snow (눈 — Rect Instancing)

```cpp
// Loader.cpp에서 프로토타입 등록
CVIBuffer_Rect_Instancing::RECT_INSTANCE_DESC SnowDesc;
SnowDesc.iNumInstance = 500;       // 눈송이 500개
SnowDesc.vScale = {0.1f, 0.3f};   // 크기 0.1~0.3
SnowDesc.vCenter = {64.f, 15.f, 64.f};
SnowDesc.vRange = {128.f, 10.f, 128.f};
SnowDesc.vSpeed = {1.f, 3.f};     // 낙하 속도
SnowDesc.vLifeTime = {3.f, 5.f};  // 3~5초 수명
SnowDesc.isLoop = true;           // 반복

// Snow::Update
m_pVIBufferCom->Drop(fTimeDelta);  // 매 프레임 아래로 이동
```

### Particle_Explosion (폭발 — Point Instancing)

```cpp
// Explosion::Update
m_pVIBufferCom->Spread(fTimeDelta);  // 매 프레임 바깥으로 퍼짐
```

둘 다 `RENDERGROUP::NONLIGHT`에 등록 — 라이팅 연산 없이 직접 출력.

---

## 10. 핵심 정리

| 항목 | 설명 |
|------|------|
| **듀얼 VB** | 슬롯 0=공유 지오메트리, 슬롯 1=인스턴스 데이터 (DYNAMIC) |
| **InputLayout** | `PER_INSTANCE_DATA` + `InstanceDataStepRate=1` |
| **Rect** | DrawIndexedInstanced(6인덱스, N인스턴스) — 빌보드 쿼드 |
| **Point** | DrawInstanced(1정점, N인스턴스) — 포인트 파티클 |
| **인스턴스 행렬** | vRight/vUp/vLook/vTranslation = 4×4 월드 행렬의 행 벡터 |
| **Map 전략** | WRITE_NO_OVERWRITE — GPU 간섭 없이 부분 수정 |
| **Clone 전략** | VB/IB 공유, CPU 배열 공유, VBInstance만 독립 생성 |
| **Drop** | Y축 하강 + 수명 만료 시 리셋 (눈) |
| **Spread** | 피벗→외부 방사 + 수명 만료 시 리셋 (폭발) |

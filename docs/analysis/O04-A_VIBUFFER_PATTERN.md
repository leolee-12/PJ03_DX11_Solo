# O04-A: VIBuffer DX11 버퍼 생성/바인딩 패턴

## 1. CVIBuffer 클래스 계층

```
CComponent (Engine)
  └── CVIBuffer (abstract)
        ├── CVIBuffer_Rect      ← 2D 사각형 (UI, 풀스크린 쿼드)
        ├── CVIBuffer_Cube      ← 스카이박스
        ├── CVIBuffer_Terrain   ← 하이트맵 지형
        ├── CVIBuffer_Instancing (abstract)
        │     ├── CVIBuffer_Rect_Instancing   ← 빌보드 파티클
        │     └── CVIBuffer_Point_Instancing  ← 포인트 파티클
        └── (CModel/CMesh가 내부적으로 VB/IB 관리)
```

---

## 2. CVIBuffer 베이스 클래스

### 멤버 변수

```cpp
class CVIBuffer abstract : public CComponent {
protected:
    ID3D11Buffer*          m_pVB;              // 버텍스 버퍼
    ID3D11Buffer*          m_pIB;              // 인덱스 버퍼

    _uint                  m_iNumVertexBuffers; // VB 슬롯 수 (보통 1, 인스턴싱 시 2)
    _uint                  m_iNumVertices;      // 정점 수
    _uint                  m_iVertexStride;     // 정점 크기 (bytes)

    _uint                  m_iNumIndices;       // 인덱스 수
    _uint                  m_iIndexStride;      // 인덱스 크기 (2=16bit, 4=32bit)
    DXGI_FORMAT            m_eIndexFormat;      // R16_UINT or R32_UINT
    D3D_PRIMITIVE_TOPOLOGY m_ePrimitive;        // TRIANGLELIST 등

    _float3*               m_pVertexPositions;  // CPU 측 위치 배열 (피킹/충돌용)
};
```

### 복사 생성자 — GPU 버퍼 공유

```cpp
CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
    : CComponent{ Prototype }
    , m_pVB { Prototype.m_pVB }                   // VB 공유
    , m_pIB { Prototype.m_pIB }                   // IB 공유
    , m_iNumVertexBuffers { Prototype.m_iNumVertexBuffers }
    , m_iNumVertices { Prototype.m_iNumVertices }
    , m_iVertexStride { Prototype.m_iVertexStride }
    , m_iNumIndices { Prototype.m_iNumIndices }
    , m_iIndexStride { Prototype.m_iIndexStride }
    , m_eIndexFormat { Prototype.m_eIndexFormat }
    , m_ePrimitive { Prototype.m_ePrimitive }
    , m_pVertexPositions { Prototype.m_pVertexPositions }  // CPU 배열도 공유
{
    Safe_AddRef(m_pVB);
    Safe_AddRef(m_pIB);
}
```

**VB와 IB는 Clone 간에 공유된다.** GPU 버퍼는 불변 데이터이므로 안전하다.
Shader와 동일한 패턴 — 참조 카운팅으로 소유권 공유.

### Free — 원본만 CPU 메모리 해제

```cpp
void CVIBuffer::Free()
{
    __super::Free();

    if (false == m_isCloned)
        Safe_Delete_Array(m_pVertexPositions);  // 원본만 삭제!

    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
}
```

`m_isCloned` 플래그(CComponent에서 상속)로 **원본과 복제를 구분**한다.
CPU 측 위치 배열은 원본만 소유하고, 복제본은 포인터만 공유한다.

---

## 3. DX11 버퍼 생성 패턴

### 버텍스 버퍼 생성 (VIBuffer_Rect 예시)

```cpp
HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
    // ① 메타데이터 설정
    m_iNumVertexBuffers = 1;
    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXPOSTEX);  // 20 bytes (float3 + float2)
    m_iNumIndices = 6;
    m_iIndexStride = 2;  // 16비트 인덱스
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // ② 버퍼 DESC 설정
    D3D11_BUFFER_DESC VBDesc{};
    VBDesc.ByteWidth        = m_iNumVertices * m_iVertexStride;  // 80 bytes
    VBDesc.Usage             = D3D11_USAGE_DEFAULT;  // GPU 전용
    VBDesc.BindFlags         = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags    = 0;  // CPU 접근 불가
    VBDesc.StructureByteStride = m_iVertexStride;

    // ③ CPU 측 정점 데이터 생성
    VTXPOSTEX* pVertices = new VTXPOSTEX[4];

    pVertices[0] = { _float3(-0.5f,  0.5f, 0.f), _float2(0.f, 0.f) };  // 좌상
    pVertices[1] = { _float3( 0.5f,  0.5f, 0.f), _float2(1.f, 0.f) };  // 우상
    pVertices[2] = { _float3( 0.5f, -0.5f, 0.f), _float2(1.f, 1.f) };  // 우하
    pVertices[3] = { _float3(-0.5f, -0.5f, 0.f), _float2(0.f, 1.f) };  // 좌하

    // ④ 초기 데이터 지정
    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    // ⑤ GPU 버퍼 생성
    m_pDevice->CreateBuffer(&VBDesc, &VertexInitialData, &m_pVB);

    // ⑥ CPU 측 임시 데이터 해제
    Safe_Delete_Array(pVertices);
}
```

### D3D11_BUFFER_DESC 필드 설명

| 필드 | 값 | 의미 |
|------|-----|------|
| `ByteWidth` | 정점수 × 정점크기 | 총 버퍼 크기 (bytes) |
| `Usage` | `D3D11_USAGE_DEFAULT` | GPU만 읽기/쓰기 |
| `BindFlags` | `BIND_VERTEX_BUFFER` | 정점 버퍼 용도 |
| `CPUAccessFlags` | 0 | CPU 접근 불가 |
| `StructureByteStride` | 정점 크기 | 구조화된 버퍼의 요소 크기 |

### Usage 옵션 비교

| Usage | GPU 읽기 | GPU 쓰기 | CPU 읽기 | CPU 쓰기 | 용도 |
|-------|---------|---------|---------|---------|------|
| `DEFAULT` | O | O | X | X | 일반 버퍼 |
| `IMMUTABLE` | O | X | X | X | 변경 없는 버퍼 |
| `DYNAMIC` | O | X | X | O (Map) | 매 프레임 갱신 |
| `STAGING` | X | X | O | O | CPU 읽기용 복사 |

이 프레임워크에서:
- **Rect, Cube, Terrain**: DEFAULT (생성 후 변경 없음)
- **Instancing VB**: DYNAMIC (매 프레임 인스턴스 데이터 갱신)
- **Picking 텍스처**: STAGING (GPU → CPU 복사)

### 인덱스 버퍼 생성

```cpp
D3D11_BUFFER_DESC IBDesc{};
IBDesc.ByteWidth        = m_iNumIndices * m_iIndexStride;  // 6 × 2 = 12 bytes
IBDesc.Usage             = D3D11_USAGE_DEFAULT;
IBDesc.BindFlags         = D3D11_BIND_INDEX_BUFFER;
IBDesc.StructureByteStride = m_iIndexStride;

_ushort pIndices[] = { 0, 1, 2,    // 삼각형 1
                       0, 2, 3 };  // 삼각형 2

D3D11_SUBRESOURCE_DATA IndexInitialData{};
IndexInitialData.pSysMem = pIndices;

m_pDevice->CreateBuffer(&IBDesc, &IndexInitialData, &m_pIB);
```

### 16비트 vs 32비트 인덱스

```cpp
m_iIndexStride = 2;  // 16비트 인덱스 (최대 65535 정점)
m_eIndexFormat = m_iIndexStride == 2
    ? DXGI_FORMAT_R16_UINT
    : DXGI_FORMAT_R32_UINT;
```

- **16비트 (R16_UINT)**: 정점 65535개 이하 (Rect, Cube 등)
- **32비트 (R32_UINT)**: 정점 65536개 이상 (Terrain 129×129 = 16641개)

---

## 4. 버퍼 바인딩 + 렌더링

### Bind_Buffers — IA 단계 설정

```cpp
HRESULT CVIBuffer::Bind_Buffers()
{
    ID3D11Buffer* pVertexBuffers[] = { m_pVB };
    _uint iVertexStrides[] = { m_iVertexStride };
    _uint iOffsets[] = { 0 };

    // ① 버텍스 버퍼 바인딩 (슬롯 0부터)
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers,
        pVertexBuffers, iVertexStrides, iOffsets);

    // ② 인덱스 버퍼 바인딩
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

    // ③ 프리미티브 토폴로지 설정
    m_pContext->IASetPrimitiveTopology(m_ePrimitive);
}
```

**DX9와의 차이:**
```cpp
// DX9 (참고)
m_pDevice->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
m_pDevice->SetIndices(m_pIB);
m_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

// DX11
m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
m_pContext->IASetIndexBuffer(m_pIB, format, 0);
m_pContext->IASetPrimitiveTopology(topology);
```

DX11에서는 FVF 대신 **InputLayout** (Shader에서 설정)으로 정점 포맷을 정의한다.

### Render — 드로우 콜

```cpp
HRESULT CVIBuffer::Render()
{
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
    return S_OK;
}
```

**DrawIndexed 파라미터:**
- `m_iNumIndices`: 그릴 인덱스 수
- `StartIndexLocation`: 인덱스 버퍼 내 시작 위치 (0)
- `BaseVertexLocation`: 정점 버퍼 내 기준 오프셋 (0)

---

## 5. Rect 정점 레이아웃

```
(-0.5, 0.5, 0)──────(0.5, 0.5, 0)
  UV(0,0)  [0]          [1]  UV(1,0)
       │  ╲                │
       │    ╲  삼각형1     │
       │      ╲  (0,1,2)  │
       │        ╲          │
       │  삼각형2  ╲       │
       │  (0,2,3)    ╲    │
(-0.5,-0.5, 0)──────(0.5,-0.5, 0)
  UV(0,1)  [3]          [2]  UV(1,1)
```

**Z=0**: 2D 평면에서 사용 (UI, 디퍼드 쿼드)
**-0.5 ~ 0.5 범위**: Transform의 Scale로 실제 크기를 조절

---

## 6. Cube 정점 레이아웃

```cpp
// 8개 꼭짓점, UV = 위치 (큐브맵 샘플링용)
pVertices[0] = { _float3(-0.5f,  0.5f, -0.5f), texcoord=position };
pVertices[1] = { _float3( 0.5f,  0.5f, -0.5f), texcoord=position };
// ...총 8개

// 6면 × 2삼각형 × 3인덱스 = 36 인덱스
// +X면: 1, 5, 6 / 1, 6, 2
// -X면: 4, 0, 3 / 4, 3, 7
// +Y면: 4, 5, 1 / 4, 1, 0
// -Y면: 3, 2, 6 / 3, 6, 7
// +Z면: 5, 4, 7 / 5, 7, 6
// -Z면: 0, 1, 2 / 0, 2, 3
```

**UV가 Position과 같은 이유**: 스카이박스에서 **큐브맵 텍스처**를 샘플링할 때
정점 위치 벡터를 방향으로 사용한다:

```hlsl
// HLSL (Cube Shader)
TextureCube g_Texture;
float4 PS_Main(float3 vTexcoord : TEXCOORD) : SV_Target
{
    return g_Texture.Sample(sampler, vTexcoord);  // 3D 방향으로 큐브맵 샘플링
}
```

---

## 7. 오브젝트 렌더 흐름 전체 정리

```
[Late_Update]
  GameObject → Add_RenderGroup(NONBLEND, this)

[Draw → Render_NonBlend]
  각 오브젝트 Render() 호출:

    ① Shader::Bind_Matrix("g_WorldMatrix", ...)
    ② Shader::Bind_Matrix("g_ViewMatrix", ...)
    ③ Shader::Bind_Matrix("g_ProjMatrix", ...)
    ④ Texture::Bind_ShaderResource(...)
    ⑤ Shader::Begin(passIndex)           // InputLayout + Apply
    ⑥ VIBuffer::Bind_Buffers()           // IA에 VB+IB 바인딩
    ⑦ VIBuffer::Render()                 // DrawIndexed
```

---

## 8. 핵심 정리

| 항목 | 설명 |
|------|------|
| **VB/IB 공유** | Clone 간 GPU 버퍼를 참조 카운팅으로 공유 |
| **CPU 위치 배열** | 원본만 소유 (m_isCloned으로 구분), 피킹/충돌용 |
| **Usage** | DEFAULT(일반), DYNAMIC(인스턴싱), STAGING(CPU 읽기) |
| **인덱스 포맷** | 16비트(≤65535 정점), 32비트(>65535 정점) |
| **Bind 순서** | IASetVertexBuffers → IASetIndexBuffer → IASetPrimitiveTopology |
| **Render** | DrawIndexed(인덱스수, 0, 0) |
| **Cube UV=Position** | 큐브맵 샘플링을 위해 위치를 방향 벡터로 사용 |

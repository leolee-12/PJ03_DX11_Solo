# O05-C: Picking — G-Buffer 월드 좌표 읽기

## 1. 전통 피킹 vs G-Buffer 피킹

### 전통 레이캐스트 피킹
```
마우스 클릭 → 스크린→NDC→월드 역변환 → 레이 생성 → 모든 삼각형과 교차 검사
```
**단점**: 삼각형 수에 비례하는 연산량. 복잡한 메시에서 무겁다.

### G-Buffer 피킹 (이 엔진의 방식)
```
마우스 클릭 → 픽셀 좌표로 배열 인덱싱 → 즉시 월드 좌표 반환
```
**장점**: O(1) 조회. 디퍼드 렌더링의 월드 좌표 렌더 타겟을 재활용한다.

---

## 2. Target_World 렌더 타겟

### 생성 (Renderer 초기화)

```cpp
// R32G32B32A32_FLOAT — 픽셀당 월드 좌표 저장
m_pGameInstance->Add_RenderTarget(
    TEXT("Target_World"),
    ViewportDesc.Width, ViewportDesc.Height,
    DXGI_FORMAT_R32G32B32A32_FLOAT,  // XYZ = 월드 위치, W = 유효 플래그
    _float4(0.0f, 0.f, 0.f, 0.f));  // 초기값 (W=0 = 빈 픽셀)
```

### MRT 소속

```
MRT_GameObjects:
  [0] Target_Diffuse   — 디퓨즈 색상
  [1] Target_Normal    — 월드 노멀
  [2] Target_Depth     — 뷰 깊이
  [3] Target_World     — 월드 좌표 ← 피킹에 사용
```

**셰이더 출력**: NonBlend 패스의 픽셀 셰이더가 각 픽셀의 월드 좌표를
`Target_World`에 기록한다. W 채널은 유효 플래그 (0이면 빈 공간).

---

## 3. CPicking 클래스 구조

```cpp
class CPicking final : public CBase
{
    HWND              m_hWnd;
    ID3D11Texture2D*  m_pTexture2D;      // STAGING 텍스처 (CPU 읽기용)
    _float4*          m_pPixelPositions;  // CPU 배열 (전체 픽셀)
    _uint             m_iNumPixels;      // Width × Height
    _uint             m_iNumPixelX;      // Width
};
```

---

## 4. 초기화 — STAGING 텍스처 생성

```cpp
HRESULT CPicking::Initialize(HWND hWnd, _uint iWidth, _uint iHeight)
{
    D3D11_TEXTURE2D_DESC TextureDesc{};
    TextureDesc.Width  = iWidth;
    TextureDesc.Height = iHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // Target_World와 동일

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count   = 1;

    TextureDesc.Usage          = D3D11_USAGE_STAGING;     // CPU 접근 전용
    TextureDesc.BindFlags      = 0;                        // 파이프라인 바인딩 없음
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

    m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D);

    m_pPixelPositions = new _float4[iWidth * iHeight];
}
```

### STAGING 텍스처란?

| Usage | GPU 읽기 | GPU 쓰기 | CPU 읽기 | CPU 쓰기 |
|-------|---------|---------|---------|---------|
| DEFAULT | O | O | X | X |
| DYNAMIC | O | X | X | O |
| **STAGING** | X | X | **O** | **O** |

**STAGING 텍스처는 GPU 파이프라인에 바인딩할 수 없다.**
오직 `CopyResource()`로 GPU 텍스처 내용을 복사받은 후, `Map()`으로 CPU에서 읽는 용도.

---

## 5. Update — GPU → CPU 복사

```cpp
void CPicking::Update()
{
    // ① Target_World RT → STAGING 텍스처로 GPU 복사
    m_pGameInstance->Copy_RT_Resource(TEXT("Target_World"), m_pTexture2D);

    // ② STAGING 텍스처를 CPU 메모리로 Map
    D3D11_MAPPED_SUBRESOURCE SubResource{};
    m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

    // ③ 전체 픽셀 데이터를 CPU 배열로 복사
    memcpy(m_pPixelPositions, SubResource.pData, sizeof(_float4) * m_iNumPixels);

    // ④ Map 해제
    m_pContext->Unmap(m_pTexture2D, 0);
}
```

### Copy_RT_Resource 체인

```
CPicking::Update()
  → GameInstance::Copy_RT_Resource("Target_World", pStagingTex)
    → Target_Manager::Copy_RT_Resource(...)
      → RenderTarget::Copy_Resource(pOut)
        → m_pContext->CopyResource(pOut, m_pTexture2D);
                                    ↑        ↑
                               STAGING    GPU 렌더타겟
```

**CopyResource**: GPU 내부에서 텍스처 간 복사. STAGING 텍스처로 복사하면
이후 CPU에서 Map으로 읽을 수 있다.

---

## 6. Picking — 마우스 좌표로 월드 좌표 조회

```cpp
_bool CPicking::Picking(_float3* pOut)
{
    // ① 마우스 스크린 좌표 획득
    ::POINT ptMouse = {};
    GetCursorPos(&ptMouse);
    ScreenToClient(m_hWnd, &ptMouse);

    // ② 2D 좌표 → 1D 인덱스 (행 우선 배열)
    _uint iIndex = ptMouse.y * m_iNumPixelX + ptMouse.x;

    // ③ W 채널이 0이면 빈 공간 (오브젝트 없음)
    if (0.f == m_pPixelPositions[iIndex].w)
        return false;

    // ④ XYZ = 월드 좌표 복사
    memcpy(pOut, &m_pPixelPositions[iIndex], sizeof(_float3));
    return true;
}
```

### 흐름 시각화

```
[스크린 공간]            [Target_World RT]         [CPU 배열]
마우스 (320, 240)  →    픽셀 [240][320]     →    m_pPixelPositions[240*800+320]
                         = (12.5, 3.2, 45.1, 1.0)
                                                    pOut = (12.5, 3.2, 45.1)
                                                    return true
```

### W 채널의 역할

| W 값 | 의미 |
|------|------|
| `0.0` | 빈 공간 (하늘, 배경) — 셰이더가 초기값 그대로 |
| `!= 0.0` | 오브젝트가 있는 픽셀 — 유효한 월드 좌표 |

셰이더에서 월드 좌표를 출력할 때 W를 1로 설정하므로,
오브젝트가 그려지지 않은 픽셀은 클리어 값 `(0,0,0,0)`이 유지된다.

---

## 7. 성능 특성

### 장점
- **O(1) 조회**: 배열 인덱싱만으로 월드 좌표 획득
- **추가 렌더링 불필요**: 디퍼드 파이프라인에서 이미 생성된 데이터 재활용
- **메시 복잡도 무관**: 삼각형 수에 관계없이 동일한 속도

### 비용
- **GPU→CPU 복사**: CopyResource + Map/memcpy가 매 프레임 발생
- **메모리**: 800×600 × 16bytes = **7.3MB** (STAGING + CPU 배열)
- **동기화**: CopyResource는 GPU 파이프라인을 멈출 수 있음 (stall)

### 최적화 가능 방안
- 마우스 클릭 시에만 Update 호출 (매 프레임 아님)
- 마우스 주변 작은 영역만 복사 (CopySubresourceRegion)
- ReadBack Queue 사용으로 비동기 복사

---

## 8. 핵심 정리

| 항목 | 설명 |
|------|------|
| **방식** | G-Buffer의 Target_World RT에서 월드 좌표 직접 읽기 |
| **Target_World** | R32G32B32A32_FLOAT, MRT_GameObjects[3] |
| **STAGING 텍스처** | CPU 읽기 전용 복사본, BindFlags=0 |
| **복사 체인** | RenderTarget → CopyResource → STAGING → Map → memcpy |
| **조회** | 마우스 (x,y) → 배열[y*W+x] → _float3 월드 좌표 |
| **유효 판정** | W 채널 == 0 → 빈 공간, != 0 → 유효 |
| **성능** | O(1) 조회, 단 GPU→CPU 전송 비용 존재 |

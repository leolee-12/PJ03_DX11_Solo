# O03-A: DX11 디바이스 초기화

## 1. DX9 vs DX11 디바이스 구조 비교

### DX9 (현재 프레임워크)

```
IDirect3DDevice9  ← 리소스 생성 + 렌더링 + 상태 설정 (모든 것이 하나)
```

### DX11 (참고 프레임워크)

```
ID3D11Device         ← 리소스 생성 전담 (스레드 안전)
ID3D11DeviceContext   ← 렌더링 명령 발행 + 상태 설정 (스레드 비안전)
IDXGISwapChain        ← 프론트/백 버퍼 교체 (Present)
```

**분리의 의미:**
- `Device`는 **멀티스레드 안전** → 로딩 스레드에서 텍스처/버퍼 생성 가능
- `DeviceContext`는 **싱글스레드** → 메인 스레드에서만 렌더링 명령 발행
- 이 분리 덕분에 CLoader의 워커 스레드에서 안전하게 리소스 생성 가능

---

## 2. CGraphic_Device 클래스 구조

```cpp
class CGraphic_Device final : public CBase {
    ID3D11Device*            m_pDevice;           // 리소스 생성기
    ID3D11DeviceContext*     m_pDeviceContext;     // 렌더 커맨드 발행기
    IDXGISwapChain*          m_pSwapChain;        // 더블 버퍼링
    ID3D11RenderTargetView*  m_pBackBufferRTV;    // 백버퍼 렌더 타겟 뷰
    ID3D11DepthStencilView*  m_pDepthStencilView; // 깊이/스텐실 뷰
};
```

### DX11의 View 개념

DX11에서는 `ID3D11Texture2D`를 직접 사용하지 않는다. 대신 **View** 객체를 통해
텍스처의 용도를 명시한다:

| View 타입 | 용도 | 예시 |
|-----------|------|------|
| `ID3D11RenderTargetView` | 렌더링 대상 | 백버퍼, G-Buffer |
| `ID3D11DepthStencilView` | 깊이/스텐실 테스트 | 깊이 버퍼 |
| `ID3D11ShaderResourceView` | 셰이더에서 읽기 | 텍스처 샘플링 |

같은 텍스처에 여러 View를 생성할 수도 있다 (예: 깊이 버퍼를 셰이더에서 읽기).

---

## 3. 초기화 순서 (Initialize)

### 전체 흐름

```
① D3D11CreateDevice     → Device + Context 생성
② Ready_SwapChain       → DXGI 체인 탐색 + SwapChain 생성
③ Ready_BackBufferRTV   → 백버퍼 텍스처 → RenderTargetView 생성
④ Ready_DepthStencilView → 깊이/스텐실 텍스처 + View 생성
⑤ OMSetRenderTargets     → RTV + DSV를 Output Merger에 바인딩
⑥ RSSetViewports         → 뷰포트 설정
⑦ Device/Context 반환    → ppDevice, ppContext로 외부에 전달
```

### ① D3D11CreateDevice — 디바이스 생성

```cpp
_uint iFlag = 0;
#ifdef _DEBUG
    iFlag = D3D11_CREATE_DEVICE_DEBUG;  // 디버그 레이어 활성화
#endif

D3D_FEATURE_LEVEL FeatureLV;

D3D11CreateDevice(
    nullptr,                    // 기본 어댑터 (GPU)
    D3D_DRIVER_TYPE_HARDWARE,   // 하드웨어 가속
    0,                          // 소프트웨어 래스터라이저 (미사용)
    iFlag,                      // 플래그 (DEBUG 모드)
    nullptr,                    // Feature Level 배열 (기본값 사용)
    0,                          // Feature Level 배열 크기
    D3D11_SDK_VERSION,          // SDK 버전
    &m_pDevice,                 // [OUT] 디바이스
    &FeatureLV,                 // [OUT] 실제 Feature Level
    &m_pDeviceContext            // [OUT] 디바이스 컨텍스트
);
```

**D3D11_CREATE_DEVICE_DEBUG**: 디버그 빌드에서만 활성화. GPU 호출의 유효성을 검사하고
Output Window에 경고/오류를 출력한다. Release 빌드에서는 성능을 위해 비활성화.

**Feature Level**: 이 코드에서는 nullptr을 전달하므로 시스템이 지원하는
최고 Feature Level(보통 11_0 또는 11_1)이 자동 선택된다.

### ② Ready_SwapChain — DXGI 체인 탐색 + 스왑체인 생성

DX11에서는 스왑체인 생성을 위해 **DXGI 팩토리**가 필요하다.
Device → DXGIDevice → Adapter → Factory 순서로 올라가며 팩토리를 얻는다:

```cpp
IDXGIDevice*   pDevice  = nullptr;
IDXGIAdapter*  pAdapter = nullptr;
IDXGIFactory*  pFactory = nullptr;

// Device → DXGI Device (QueryInterface)
m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

// DXGI Device → Adapter (GPU)
pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

// Adapter → Factory (DXGI 최상위)
pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
```

**왜 이렇게 복잡한가?**
DX11은 **D3D11CreateDeviceAndSwapChain**을 제공하지만, 이 코드에서는
Device를 먼저 생성한 후 DXGI 계층을 역추적하여 Factory를 얻는다.
이 패턴은 DXGI 팩토리를 직접 생성하는 것보다 **디바이스와 동일한 어댑터**를
보장하는 안전한 방식이다.

```
DXGI 계층 구조:
IDXGIFactory  ← 최상위 (스왑체인 생성 담당)
  └── IDXGIAdapter  ← GPU 하나 (비디오 카드)
        └── IDXGIDevice  ← D3D 디바이스의 DXGI 인터페이스
              └── ID3D11Device  ← D3D11 디바이스
```

### SwapChain DESC 설정

```cpp
DXGI_SWAP_CHAIN_DESC SwapChain;
ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

// 백버퍼 텍스처 크기 = 윈도우 크기
SwapChain.BufferDesc.Width  = iWinCX;
SwapChain.BufferDesc.Height = iWinCY;

// 픽셀 포맷: R8G8B8A8 (채널당 8비트, 총 32비트)
SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

// 주사율 60Hz
SwapChain.BufferDesc.RefreshRate.Numerator   = 60;
SwapChain.BufferDesc.RefreshRate.Denominator = 1;

// 렌더 타겟 용도
SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
SwapChain.BufferCount = 1;  // 더블 버퍼링 (프론트1 + 백1)

// 멀티샘플링 없음 (후처리로 AA 구현)
SwapChain.SampleDesc.Quality = 0;
SwapChain.SampleDesc.Count   = 1;

SwapChain.OutputWindow = hWnd;
SwapChain.Windowed     = static_cast<BOOL>(isWindowed);
SwapChain.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;  // 이전 프레임 폐기

pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain);
```

**DXGI_SWAP_EFFECT_DISCARD**: Present 후 백버퍼 내용을 보존하지 않는다.
매 프레임 전체를 새로 그리는 게임에 적합하다.

### ③ Ready_BackBufferRenderTargetView

```cpp
ID3D11Texture2D* pBackBufferTexture = nullptr;

// 스왑체인이 소유한 백버퍼 텍스처를 가져온다
m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
    (void**)&pBackBufferTexture);

// 텍스처 → RenderTargetView 생성
m_pDevice->CreateRenderTargetView(
    pBackBufferTexture, nullptr, &m_pBackBufferRTV);

// 텍스처 참조 해제 (RTV가 내부적으로 참조 유지)
Safe_Release(pBackBufferTexture);
```

**중요**: `GetBuffer`는 텍스처의 참조 카운트를 증가시킨다.
사용 후 반드시 `Release`해야 메모리 누수가 없다.

### ④ Ready_DepthStencilView

```cpp
D3D11_TEXTURE2D_DESC TextureDesc;
ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

TextureDesc.Width     = iWinCX;
TextureDesc.Height    = iWinCY;
TextureDesc.MipLevels = 1;
TextureDesc.ArraySize = 1;

// D24: 깊이 24비트, S8: 스텐실 8비트 = 총 32비트
TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

TextureDesc.SampleDesc.Quality = 0;
TextureDesc.SampleDesc.Count   = 1;

TextureDesc.Usage     = D3D11_USAGE_DEFAULT;  // GPU 전용
TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
TextureDesc.CPUAccessFlags = 0;

m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture);
m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView);

Safe_Release(pDepthStencilTexture);
```

**DXGI_FORMAT_D24_UNORM_S8_UINT 분석:**
- `D24`: 깊이값 24비트 (0.0~1.0 정규화, 약 1600만 단계)
- `S8`: 스텐실값 8비트 (0~255, 마스킹/아웃라인 등에 사용)
- 총 32비트로 백버퍼와 동일한 크기

### ⑤ Output Merger 바인딩 + 뷰포트

```cpp
// 렌더 타겟 + 깊이/스텐실을 Output Merger에 연결
ID3D11RenderTargetView* pRTVs[] = { m_pBackBufferRTV };
m_pDeviceContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

// 뷰포트 설정 (렌더링 영역)
D3D11_VIEWPORT ViewPortDesc;
ViewPortDesc.TopLeftX = 0;
ViewPortDesc.TopLeftY = 0;
ViewPortDesc.Width    = (_float)iWinSizeX;
ViewPortDesc.Height   = (_float)iWinSizeY;
ViewPortDesc.MinDepth = 0.f;
ViewPortDesc.MaxDepth = 1.f;

m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);
```

### ⑥ Device/Context 외부 반환

```cpp
*ppDevice  = m_pDevice;
*ppContext = m_pDeviceContext;

// 외부에서도 참조하므로 AddRef
Safe_AddRef(m_pDevice);
Safe_AddRef(m_pDeviceContext);
```

CGraphic_Device가 내부에서 보유하는 것과 별도로 외부(GameInstance, MainApp 등)에서도
Device/Context를 참조하므로 참조 카운트를 +1 한다.

---

## 4. 프레임 렌더링 사이클

```cpp
// MainApp::Render()
m_pGameInstance->Begin_Draw(&vClearColor);  // ① 초기화
m_pGameInstance->Draw();                     // ② 렌더링
m_pGameInstance->End_Draw();                 // ③ 화면 출력
```

### ① Begin_Draw — 버퍼 초기화

```cpp
HRESULT CGraphic_Device::Clear_BackBuffer_View(const _float4* pClearColor)
{
    // 백버퍼를 지정 색상으로 클리어
    m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV,
        reinterpret_cast<const _float*>(pClearColor));
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
    // 깊이=1.0(최대), 스텐실=0으로 초기화
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
```

### ③ End_Draw — Present

```cpp
HRESULT CGraphic_Device::Present()
{
    // 백버퍼를 프론트버퍼와 교체하여 화면에 표시
    return m_pSwapChain->Present(0, 0);
    // 첫 번째 인자 0 = VSync 없음 (최대 FPS)
    // 1로 바꾸면 VSync 활성화
}
```

---

## 5. 디버그 모드 — Live Object 추적

```cpp
void CGraphic_Device::Free()
{
    Safe_Release(m_pSwapChain);
    Safe_Release(m_pDepthStencilView);
    Safe_Release(m_pBackBufferRTV);
    Safe_Release(m_pDeviceContext);

#if defined(DEBUG) || defined(_DEBUG)
    ID3D11Debug* d3dDebug;
    m_pDevice->QueryInterface(__uuidof(ID3D11Debug),
        reinterpret_cast<void**>(&d3dDebug));

    // 아직 해제되지 않은 D3D 리소스 목록 출력
    d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

    d3dDebug->Release();
#endif

    Safe_Release(m_pDevice);
}
```

**ReportLiveDeviceObjects**: 프로그램 종료 시 해제되지 않은 D3D 리소스를
Output Window에 상세히 출력한다. 메모리 누수 디버깅에 핵심적인 도구.

**실행 순서의 중요성:**
1. SwapChain, RTV, DSV, Context를 먼저 Release
2. 아직 Device가 살아있는 상태에서 ReportLiveDeviceObjects 호출
3. 나머지 미해제 리소스(다른 곳에서 해제 실패한 것)가 있으면 여기서 발견
4. 마지막으로 Device를 Release

---

## 6. DX11 파이프라인 개요

```
Input Assembler (IA)  ← 버텍스/인덱스 버퍼 바인딩
        ↓
Vertex Shader (VS)    ← 월드/뷰/프로젝션 변환
        ↓
Hull → Tessellator → Domain  (미사용)
        ↓
Geometry Shader (GS)  (미사용)
        ↓
Rasterizer (RS)       ← 뷰포트, 컬링, 클리핑
        ↓
Pixel Shader (PS)     ← 텍스처 샘플링, 조명 계산
        ↓
Output Merger (OM)    ← 렌더 타겟 + 깊이/스텐실 테스트
```

CGraphic_Device가 설정하는 것:
- **OM**: 렌더 타겟(m_pBackBufferRTV) + 깊이/스텐실(m_pDepthStencilView)
- **RS**: 뷰포트(RSSetViewports)

나머지 파이프라인 단계는 각 오브젝트의 Render 함수에서 설정한다.

---

## 7. 핵심 정리

| 항목 | 설명 |
|------|------|
| **Device 분리** | Device(리소스 생성, MT-safe) + Context(렌더링, ST-only) |
| **DXGI 계층** | Factory → Adapter → Device → SwapChain |
| **View 패턴** | Texture2D를 직접 사용하지 않고 RTV/DSV/SRV로 용도 명시 |
| **깊이 포맷** | D24_UNORM_S8_UINT (깊이 24bit + 스텐실 8bit) |
| **디버그** | D3D11Debug::ReportLiveDeviceObjects로 누수 추적 |
| **Present** | SwapChain::Present(0,0) = VSync 없이 즉시 교체 |

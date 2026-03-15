# O03-C: RenderTarget + Target_Manager + 디퍼드 렌더링

## 1. CRenderTarget — 렌더 타겟 래퍼

### 핵심 구조

하나의 RenderTarget은 **하나의 Texture2D에서 두 가지 View**를 생성한다:

```
ID3D11Texture2D (텍스처 리소스)
  ├── ID3D11RenderTargetView (RTV)    ← 렌더링 대상으로 사용
  └── ID3D11ShaderResourceView (SRV)  ← 셰이더에서 읽기용
```

### Initialize — 듀얼 바인딩 텍스처 생성

```cpp
HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight,
    DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    m_vClearColor = vClearColor;

    D3D11_TEXTURE2D_DESC TextureDesc{};
    TextureDesc.Width  = iWidth;
    TextureDesc.Height = iHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = ePixelFormat;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;

    // 핵심: RTV + SRV 동시 사용 가능
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET
                          | D3D11_BIND_SHADER_RESOURCE;

    m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D);
    m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV);
    m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV);
}
```

**같은 텍스처에 RTV + SRV를 동시에 생성**하는 것이 디퍼드 렌더링의 핵심이다.
G-Buffer 패스에서 RTV로 렌더링한 결과를, 라이팅 패스에서 SRV로 읽는다.

### Copy_Resource — CPU 읽기 지원

```cpp
HRESULT CRenderTarget::Copy_Resource(ID3D11Texture2D* pOut)
{
    m_pContext->CopyResource(pOut, m_pTexture2D);
    return S_OK;
}
```

GPU 텍스처의 내용을 CPU 접근 가능한 Staging 텍스처로 복사한다.
**Picking** 시스템에서 World 좌표 렌더 타겟의 픽셀을 읽을 때 사용된다.

---

## 2. CTarget_Manager — 렌더 타겟 그룹 관리

### 자료 구조

```cpp
class CTarget_Manager {
    // 개별 렌더 타겟 저장 (이름 → 타겟)
    map<wstring, CRenderTarget*>       m_RenderTargets;

    // MRT 그룹 (그룹 이름 → 타겟 리스트)
    map<wstring, list<CRenderTarget*>> m_MRTs;

    // 백버퍼 백업용
    ID3D11RenderTargetView* m_pBackBufferRTV;
    ID3D11DepthStencilView* m_pDSV;
};
```

### Begin_MRT — MRT 바인딩

```cpp
HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag,
    _bool isClearTarget, _bool isClearDepth, ID3D11DepthStencilView* pDSV)
{
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    // ① 기존 셰이더 리소스 바인딩을 모두 해제 (충돌 방지)
    ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]
        = { nullptr };
    m_pContext->PSSetShaderResources(0,
        D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

    // ② 현재 백버퍼 RTV + DSV를 백업
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pDSV);

    // ③ MRT의 RTV들을 배열로 수집
    _uint iNumRenderTargets = 0;
    ID3D11RenderTargetView* pRenderTargets[8] = {};

    for (auto& pRenderTarget : *pMRTList)
    {
        if (isClearTarget)
            pRenderTarget->Clear();  // 각 타겟을 클리어 색상으로 초기화
        pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }

    // ④ 커스텀 DSV가 있으면 클리어
    if (pDSV && isClearDepth)
        m_pContext->ClearDepthStencilView(pDSV,
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    // ⑤ 새 MRT 바인딩 (커스텀 DSV 또는 기존 DSV)
    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets,
        pDSV ? pDSV : m_pDSV);
}
```

**SRV 해제가 먼저인 이유**: DX11은 같은 텍스처를 RTV와 SRV로 동시에 바인딩할 수 없다.
이전 패스에서 SRV로 사용하던 텍스처를 이번 패스에서 RTV로 사용하려면
먼저 SRV 바인딩을 해제해야 한다.

### End_MRT — 백버퍼 복원

```cpp
HRESULT CTarget_Manager::End_MRT()
{
    // 백업해둔 백버퍼 RTV + DSV로 복원
    ID3D11RenderTargetView* pRTVs[8] = { m_pBackBufferRTV };
    m_pContext->OMSetRenderTargets(8, pRTVs, m_pDSV);

    Safe_Release(m_pBackBufferRTV);  // OMGetRenderTargets가 AddRef했으므로
    Safe_Release(m_pDSV);
}
```

---

## 3. 렌더 타겟 구성

### 9개의 렌더 타겟

| 이름 | 해상도 | 포맷 | 용도 |
|------|--------|------|------|
| `Target_Diffuse` | 뷰포트 | R8G8B8A8 | 디퓨즈 색상 |
| `Target_Normal` | 뷰포트 | R16G16B16A16 | 월드 노멀 |
| `Target_Depth` | 뷰포트 | R32G32B32A32_FLOAT | 뷰 공간 깊이 |
| `Target_World` | 뷰포트 | R32G32B32A32_FLOAT | 월드 좌표 (피킹용) |
| `Target_Shade` | 뷰포트 | R16G16B16A16 | 라이팅 결과 |
| `Target_Specular` | 뷰포트 | R16G16B16A16 | 스펙큘러 반사 |
| `Target_Shadow` | 8192×4608 | R32G32B32A32_FLOAT | 그림자 깊이 |
| `Target_Final` | 뷰포트 | R8G8B8A8 | 합성 결과 |
| `Target_Blur_X` | 뷰포트 | R8G8B8A8 | 블러 중간 결과 |

**Target_Shadow가 8192×4608인 이유**: 그림자 품질을 위해 고해상도 깊이맵을 사용.
`g_iMaxWidth`와 `g_iMaxHeight`는 Engine_Defines.h에 정의된 상수.

### 5개의 MRT 그룹

```
MRT_GameObjects: [Target_Diffuse, Target_Normal, Target_Depth, Target_World]
MRT_LightAcc:   [Target_Shade, Target_Specular]
MRT_Shadow:      [Target_Shadow]
MRT_Final:       [Target_Final]
MRT_Blur_X:      [Target_Blur_X]
```

---

## 4. 디퍼드 렌더링 파이프라인

### 전체 렌더링 순서 (Renderer::Draw)

```
① Render_Priority  ─→ MRT_Final에 스카이 등 먼저 그림
② Render_Shadow    ─→ MRT_Shadow에 그림자 깊이 렌더 (8192×4608)
③ Render_NonBlend  ─→ MRT_GameObjects에 G-Buffer 생성
④ Render_Lights    ─→ MRT_LightAcc에 라이팅 축적
⑤ Render_Combined  ─→ MRT_Final에 Diffuse×Shade+Specular+Shadow 합성
⑥ Render_Blur      ─→ MRT_Blur_X → 백버퍼 (2패스 블러)
⑦ Render_NonLights ─→ 백버퍼에 직접 (조명 불필요 오브젝트)
⑧ Render_Blend     ─→ 백버퍼에 직접 (반투명 오브젝트)
⑨ Render_UI        ─→ 백버퍼에 직접 (UI)
```

### 단계별 상세 분석

#### ① Render_Priority — 배경 오브젝트

```cpp
void CRenderer::Render_Priority()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_Final"));  // Target_Final에 렌더
    for (auto& pObj : m_RenderObjects[PRIORITY])
        pObj->Render();
    m_pGameInstance->End_MRT();
}
```

스카이박스처럼 **가장 먼저 그려져야 하는 오브젝트**. G-Buffer가 아닌
Final 타겟에 직접 렌더한다 (라이팅 불필요).

#### ② Render_Shadow — 그림자 패스

```cpp
void CRenderer::Render_Shadow()
{
    // Shadow MRT + 커스텀 DSV (8192×4608), 클리어 포함
    m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), true, true, m_pShadowDSV);

    // 뷰포트를 고해상도로 변경
    Change_Viewport(g_iMaxWidth, g_iMaxHeight);

    for (auto& pObj : m_RenderObjects[SHADOW])
        pObj->Render_Shadow();  // 그림자용 별도 렌더 함수

    m_pGameInstance->End_MRT();

    // 뷰포트를 원래 해상도로 복원
    Change_Viewport(originalWidth, originalHeight);
}
```

**커스텀 DSV 사용**: 그림자용 깊이 버퍼는 8192×4608 해상도로 별도 생성.
일반 백버퍼의 DSV(1280×720)와는 독립적이다.

#### ③ Render_NonBlend — G-Buffer 생성

```cpp
void CRenderer::Render_NonBlend()
{
    // 4개 타겟에 동시 렌더 (MRT)
    m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"));

    for (auto& pObj : m_RenderObjects[NONBLEND])
        pObj->Render();  // PS에서 4개 타겟에 동시 출력

    m_pGameInstance->End_MRT();
}
```

불투명 오브젝트들이 **하나의 드로우 콜로 4개의 렌더 타겟에 동시 출력**한다.
셰이더의 PS 출력 구조체가 4개의 SV_Target을 가진다:

```hlsl
struct PS_OUTPUT {
    float4 vDiffuse  : SV_Target0;  // → Target_Diffuse
    float4 vNormal   : SV_Target1;  // → Target_Normal
    float4 vDepth    : SV_Target2;  // → Target_Depth
    float4 vWorld    : SV_Target3;  // → Target_World
};
```

#### ④ Render_Lights — 라이팅 축적

```cpp
void CRenderer::Render_Lights()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"));

    // G-Buffer를 셰이더 입력으로 바인딩
    m_pGameInstance->Bind_RT_ShaderResource(
        TEXT("Target_Normal"), m_pShader, "g_NormalTexture");
    m_pGameInstance->Bind_RT_ShaderResource(
        TEXT("Target_Depth"), m_pShader, "g_DepthTexture");

    // 카메라/행렬 정보 바인딩
    m_pShader->Bind_RawValue("g_vCamPosition",
        m_pGameInstance->Get_CamPosition(), sizeof(_float4));
    m_pShader->Bind_Matrix("g_ViewMatrixInverse",
        m_pGameInstance->Get_Transform_Float4x4_Inverse_Ptr(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrixInverse",
        m_pGameInstance->Get_Transform_Float4x4_Inverse_Ptr(D3DTS::PROJECTION));

    // 각 광원마다 풀스크린 쿼드를 그려서 라이팅 축적
    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

    m_pGameInstance->End_MRT();
}
```

**라이팅이 풀스크린 쿼드인 이유**: 디퍼드 렌더링에서는 각 광원이 화면 전체를
대상으로 연산한다. G-Buffer에서 노멀/깊이를 읽고 월드 좌표를 복원하여
픽셀별로 조명을 계산한다.

**뷰/프로젝션 역행렬이 필요한 이유**: 화면 공간(UV) + 깊이 값에서 월드 좌표를
복원하기 위해 역행렬로 역변환한다.

#### ⑤ Render_Combined — 최종 합성

```cpp
void CRenderer::Render_Combined()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_Final"), false);  // 클리어 안 함!

    // 모든 필요한 타겟을 셰이더 입력으로
    Bind_RT(Target_Diffuse,  "g_DiffuseTexture");
    Bind_RT(Target_Shade,    "g_ShadeTexture");
    Bind_RT(Target_Specular, "g_SpecularTexture");
    Bind_RT(Target_Shadow,   "g_ShadowTexture");
    Bind_RT(Target_Depth,    "g_DepthTexture");

    // 라이트 뷰/프로젝션 (그림자 매핑용)
    Bind_Matrix("g_LightViewMatrix", Shadow의 View);
    Bind_Matrix("g_LightProjMatrix", Shadow의 Proj);

    m_pShader->Begin(3);  // Pass 3 = Combined 합성
    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();
}
```

**isClearTarget=false**: Priority 패스에서 이미 스카이를 그렸으므로
Target_Final을 클리어하지 않고 위에 덧그린다.

합성 공식 (셰이더 내부):
```
Final = Diffuse × Shade + Specular + Shadow 계수
```

#### ⑥ Render_Blur — 2패스 분리 블러

```cpp
void CRenderer::Render_Blur()
{
    // Pass 1: X축 블러 (Target_Final → Target_Blur_X)
    Begin_MRT("MRT_Blur_X");
    Bind_RT(Target_Final, "g_FinalTexture");
    m_pShader->Begin(4);  // Pass 4 = Blur X
    Render();
    End_MRT();

    // Pass 2: Y축 블러 (Target_Blur_X → 백버퍼 직접)
    Bind_RT(Target_Blur_X, "g_BlurXTexture");
    m_pShader->Begin(5);  // Pass 5 = Blur Y
    Render();
    // End_MRT 없음 → 백버퍼에 직접 출력!
}
```

**분리 블러 (Separable Blur)**: 2D 가우시안 블러를 X축 → Y축으로 분리하여
O(n²) → O(2n)으로 최적화하는 표준 기법.

---

## 5. 렌더 그룹 (RENDERGROUP)

```cpp
enum class RENDERGROUP {
    PRIORITY,   // 스카이박스 (Final 타겟)
    SHADOW,     // 그림자 캐스터 (Shadow 타겟)
    NONBLEND,   // 불투명 (G-Buffer → MRT_GameObjects)
    NONLIGHT,   // 조명 불필요 (백버퍼 직접)
    BLEND,      // 반투명 (백버퍼 직접, 알파 블렌딩)
    UI,         // UI (백버퍼 직접, 최후순위)
    END
};
```

### Renderer의 렌더 그룹 자료구조

```cpp
list<CGameObject*> m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];
```

매 프레임 Late_Update에서 `Add_RenderGroup()`으로 등록하고,
Draw에서 순서대로 처리 후 모두 Clear한다 (일회용 큐).

---

## 6. Renderer 내부 자원

### 풀스크린 쿼드 셋업

```cpp
// Renderer::Initialize()
m_pShader = CShader::Create(m_pDevice, m_pContext,
    TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"),
    VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);

m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);

// 풀스크린 쿼드의 월드 행렬 (뷰포트 크기)
XMStoreFloat4x4(&m_WorldMatrix,
    XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));

// 뷰 = Identity, 프로젝션 = 직교 (UI와 동일)
XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
XMStoreFloat4x4(&m_ProjMatrix,
    XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));
```

디퍼드 패스의 라이팅, 합성, 블러는 모두 **화면 전체를 덮는 사각형**에
셰이더를 적용하는 방식이다. 이 사각형이 `m_pVIBuffer` (CVIBuffer_Rect).

### Shadow DSV

```cpp
HRESULT CRenderer::Ready_Shadow_DSV()
{
    TextureDesc.Width  = g_iMaxWidth;   // 8192
    TextureDesc.Height = g_iMaxHeight;  // 4608
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture);
    m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV);
}
```

그림자 패스에서는 고해상도 DSV를 사용하므로 별도로 생성한다.

---

## 7. 디퍼드 셰이더 패스 맵

| 패스 번호 | 용도 | 입력 | 출력 |
|-----------|------|------|------|
| 0 | 디버그 렌더 타겟 표시 | g_Texture | 백버퍼 |
| 1~2 | 광원별 라이팅 (방향/점) | Normal + Depth | Shade + Specular |
| 3 | 최종 합성 | Diffuse + Shade + Specular + Shadow + Depth | Final |
| 4 | 블러 X축 | Final | Blur_X |
| 5 | 블러 Y축 | Blur_X | 백버퍼 |

---

## 8. 핵심 정리

| 항목 | 설명 |
|------|------|
| **RenderTarget** | Texture2D에 RTV + SRV 동시 생성 (렌더→읽기) |
| **MRT** | 최대 8개 렌더 타겟 동시 바인딩 |
| **Begin/End MRT** | 백버퍼 백업 → MRT 바인딩 → 작업 → 백버퍼 복원 |
| **SRV 해제 필수** | 같은 텍스처의 RTV/SRV 동시 바인딩 불가 |
| **9단계 파이프라인** | Priority → Shadow → G-Buffer → Light → Combined → Blur → NonLight → Blend → UI |
| **풀스크린 쿼드** | 디퍼드 패스는 화면 크기 사각형 + 직교 투영 |
| **Shadow DSV** | 8192×4608 별도 깊이 버퍼 |

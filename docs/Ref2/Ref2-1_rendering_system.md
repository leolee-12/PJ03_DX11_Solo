# 참고프로젝트2 — 렌더링 시스템 심화 분석

> DirectX 11 디퍼드 + 포워드 하이브리드 렌더 파이프라인

---

## 1. 핵심 책임과 경계

### Renderer DLL의 책임
- **렌더 파이프라인 오케스트레이션**: 26개 렌더 그룹의 순차 처리
- **MRT 기반 G-Buffer 관리**: 렌더 타겟 생성/바인딩/클리어
- **라이트 패스**: 배경/플레이어/이펙트 3종 라이트 카테고리
- **후처리**: 글로우(다운샘플+블러), 디스토션, 블랙아웃/화이트아웃
- **디버그 렌더 타겟 시각화**

### Engine DLL의 렌더링 책임
- **DX11 디바이스/스왑체인 초기화** (`CGraphic_Device`)
- **셰이더 래핑** (`CShader` — Effects11 기반)
- **버텍스/인덱스 버퍼** (`CVIBuffer` 계열)
- **텍스처 SRV 관리** (`CTexture`)
- **View/Proj 행렬 파이프라인** (`CPipeLine`)

### 경계 원칙
```
Engine: "DX 리소스 생성·관리, 개별 오브젝트 렌더 수행"
Renderer: "전체 화면을 어떤 순서로, 어디에 그릴지 결정"
```
- `CGameObject::Render()` — Engine 측, 개별 오브젝트가 자기 자신을 그림
- `CRenderer::Draw()` — Renderer 측, 오브젝트들의 Render를 호출 순서 조율

---

## 2. 클래스 간 소유/참조 관계

```
CRenderInstance (Singleton, 파사드)
├── [소유] CRenderer         ← 핵심 렌더 파이프라인
├── [소유] CLobby_Renderer   ← 로비 전용 렌더러
├── [소유] CTarget_Manager   ← 렌더 타겟 저장소
├── [소유] CLight_Manager    ← 라이트 저장소
└── [소유] CPicking          ← 피킹 유틸

CRenderer
├── [참조] CRenderInstance   ← MRT Begin/End, 라이트 렌더 등 위임
├── [참조] CGameInstance     ← View/Proj 행렬, 카메라 위치 조회
├── [소유] CShader (3개)     ← Shader_Deferred, Shader_Deferred_Glow, Shader_Deferred_UI_Glow
├── [소유] CVIBuffer_Rect    ← 풀스크린 쿼드 (후처리/디퍼드 합성용)
├── [소유] CTransform        ← 디스토션 빌보드 트랜스폼
├── [소유] CTexture (3개)    ← 디스토션 텍스처, 오라 텍스처, 마무리 텍스처
├── [소유] ID3D11DepthStencilView ← 섀도우 전용 DSV
└── [임시 보관] list<CGameObject*> m_RenderObjects[26]  ← 프레임 단위 수집/소비

CTarget_Manager
├── [소유] map<wstring, CRenderTarget*>        ← 개별 렌더 타겟
└── [소유] map<wstring, list<CRenderTarget*>>  ← MRT 그룹

CRenderTarget
├── [소유] ID3D11Texture2D         ← GPU 텍스처
├── [소유] ID3D11RenderTargetView  ← RTV
└── [소유] ID3D11ShaderResourceView ← SRV

CLight_Manager
├── [소유] list<CLight*>          ← 배경 라이트
├── [소유] map<string, CLight*>   ← 플레이어 라이트 (키: 플레이어 이름)
└── [소유] map<string, CLight*>   ← 이펙트 라이트 (키: 이펙트 이름)
```

### 생성 순서
```
CRenderInstance::Initialize_Engine()
  ├── CTarget_Manager::Create(pDevice, pContext)
  ├── CRenderer::Create(pDevice, pContext)
  │     └── Initialize()
  │           ├── Initialize_RenderTarget()  ← Target_Manager에 ~50개 RT 등록
  │           ├── CShader::Create() × 3
  │           ├── CVIBuffer_Rect::Create()
  │           ├── CTexture::Create() × 3
  │           └── Shadow DSV 생성
  ├── CLight_Manager::Create()
  └── CPicking::Create()
```

---

## 3. 한 프레임 렌더 흐름

### 3.1 오브젝트 등록 (Update 단계)

```cpp
// CGameObject::Late_Update()에서 호출
CRenderInstance::Add_RenderObject(RG_PLAYER, this, &renderObjDesc);
```
- 각 오브젝트가 자신의 렌더 그룹과 부가 정보(이름, 글로우 색상)를 등록
- `CRenderer::m_RenderObjects[26]` 리스트에 AddRef 하여 보관

### 3.2 Draw() — 전체 렌더 순서 (3092줄의 핵심)

```
CRenderer::Draw(fTimeDelta)
│
│  [1단계: 에디터/피킹용]
├── Render_Node()              ← 에디터 노드
├── Render_NonBlend_Test()     ← 에디터 피킹 (MRT_EffectToolPick)
├── Render_NonBlend_Layer()    ← 에디터 피킹 (MRT_GameObjects)
│
│  [2단계: 사전 렌더]
├── Render_StageDepth()        ← 스테이지 깊이 맵 (MRT_StageDepth)
├── Render_Priority()          ← 배경/스카이박스 (백버퍼 직접)
├── Render_Glow_Priority()     ← 우선 글로우 이펙트
├── Render_Blend_Priority()    ← 우선 블렌드 + 스타 글로우
│
│  [3단계: 섀도우 + 맵 디퍼드]
├── Render_ShadowObj()         ← 섀도우맵 (MRT_ShadowObjects + Shadow DSV)
├── Render_Map()               ← 맵 디퍼드 (MRT_BloomDiffuse → MRT_GameObjects)
├── Render_NonBlend()          ← 불투명 오브젝트 G-Buffer (MRT_GameObjects)
│
│  [4단계: 맵 라이팅 + 디퍼드 합성]
├── Render_Lights()            ← 배경 라이트 → Shade/Specular (MRT_LightAcc)
├── Render_Deferred()          ← Diffuse × Shade + Specular + Shadow → 백버퍼 (Pass 4)
│
│  [5단계: 포워드 + 플레이어]
├── Render_NonLight()          ← 라이트 미적용 오브젝트 (백버퍼 직접)
├── Render_Metallic()          ← 메탈릭 파츠 (MRT_Matallic)
├── Draw_Test_PostProcess()    ← 백버퍼 복사 후처리
├── Draw_MapBlackOut()         ← 맵 어둡게 (선택적)
│
│  [6단계: 플레이어 (캐릭터별 개별 디퍼드)]
├── Render_Player()            ← 각 캐릭터마다:
│   ├── MRT_Player 에 G-Buffer 렌더
│   ├── Render_PlayerLight()   ← 플레이어 전용 라이트 + 이펙트 라이트
│   │     └── MRT_LightAcc 에 Shade/Specular
│   ├── Render_PlayerAuraMaskBlur()  ← 오라 마스크 다운샘플+블러
│   └── Render_PlayerDeferred()      ← Diffuse×Shade+Specular+Shadow+Metallic (Pass 3)
│
│  [7단계: 이펙트 + 글로우]
├── Render_AllGlow_Effect_BackSide() ← 뒷면 이펙트 글로우 (MRT_AllGlowDiffuse_*)
├── Render_NonLight_Effect()         ← 이펙트 (아웃라인 포함)
├── Render_AllGlow_Effect_Pri()      ← 우선 이펙트 글로우
├── Render_Blend()                   ← 알파 블렌딩 오브젝트
├── Render_Glow()                    ← 메인 글로우 (다운샘플+블러+합성)
│
│  [8단계: 후처리]
├── Render_Distortion()        ← 디스토션 왜곡 (3-pass)
│
│  [9단계: UI]
├── Render_MultyGlow_UI()      ← 멀티 글로우 UI
├── Render_UI()                ← 일반 UI
├── Render_Glow_UI()           ← UI 글로우
├── Render_AllGlow_Effect()    ← 최종 글로우 합성
│
│  [10단계: 컷신 + 전환 효과]
├── Render_CutScene_*()        ← 컷신 이펙트/오브젝트
├── Draw_AllBlackOut()         ← 전체 블랙아웃
├── Draw_WhiteBlack_Mode()     ← 흑백 전환 + 와이프
├── Draw_AllWhiteOut()         ← 전체 화이트아웃
│
│  [디버그]
└── Render_Debug()             ← 디버그 RT 미니 뷰 + 충돌체 시각화
```

---

## 4. MRT 구성과 G-Buffer 레이아웃

### 맵/일반 오브젝트 G-Buffer (`MRT_GameObjects`)

| 렌더타겟 | 포맷 | 내용 |
|----------|------|------|
| `Target_Diffuse` | B8G8R8A8_UNORM | 디퓨즈 색상 |
| `Target_Normal` | R16G16B16A16_UNORM | 월드 노멀 |
| `Target_Depth` | R32G32B32A32_FLOAT | 뷰 깊이 (z/w) |

### 플레이어 G-Buffer (`MRT_Player`) — **4채널**

| 렌더타겟 | 내용 |
|----------|------|
| `Target_Player_Diffuse` | 플레이어 디퓨즈 |
| `Target_Player_Normal` | 플레이어 노멀 |
| `Target_Player_Depth` | 플레이어 깊이 |
| `Target_Player_AuraMask` | **오라 마스크** (블러 후 합성용) |

### 라이트 축적 (`MRT_LightAcc`)

| 렌더타겟 | 내용 |
|----------|------|
| `Target_Shade` | Diffuse 셰이딩 결과 |
| `Target_Specular` | 스펙큘러 결과 |

### 후처리 렌더타겟

| MRT | 렌더타겟 | 용도 |
|-----|----------|------|
| `MRT_ShadowObjects` | `Target_LightDepth` | 섀도우 깊이 맵 |
| `MRT_GlowDiffuse` | `Target_GlowDiffuse` + `Target_GlowAlpha` | 개별 글로우 |
| `MRT_Down` / `MRT_DownSecond` | 1/2, 1/4 해상도 | 다운샘플링 |
| `MRT_Blur_X` / `MRT_Blur_Y` | 1/4 해상도 | 가우시안 블러 XY |
| `MRT_AllGlowDiffuse_0~9` | 풀 해상도 × 10 슬롯 | 이펙트별 글로우 배열 |
| `MRT_Distortion` | 디스토션 마스크 | UV 왜곡 맵 |
| `MRT_ResultDistortion_BackBuffer` | 왜곡된 백버퍼 복사본 | 최종 합성 |
| `MRT_BlackOut/WhiteOut/All*` | 전환 효과 | 블랙/화이트 페이드 |
| `MRT_PlayerDefferd` | 플레이어 디퍼드 결과 | 누적 합성 |
| `MRT_Player_Blur_X/Y` | 플레이어 블러 | 오라 블러 |
| `MRT_Matallic` | 메탈릭 마스크 | 메탈릭 셰이딩 |
| `MRT_StageDepth` | 스테이지 깊이 | 깊이 비교 |
| `MRT_BloomDiffuse` | 맵 블룸 (3채널) | 디퓨즈+알파+깊이 |

---

## 5. 핵심 후처리 파이프라인 상세

### 5.1 글로우 (Glow / Bloom)

**4-pass 다운샘플+가우시안 블러+합성**:
```
[원본 1920×1080]
    │
    ▼  Pass 3 (다운샘플 4x)
[MRT_Down: 960×540]
    │
    ▼  Pass 3 (다운샘플 6x)
[MRT_DownSecond: 480×270]
    │
    ▼  Pass 0 (가우시안 블러 X)
[MRT_Blur_X: 480×270]
    │
    ▼  Pass 1 (가우시안 블러 Y)
[MRT_Blur_Y: 480×270]
    │
    ▼  Pass 2 (원본 + 블러 합성, GlowColor/Factor 적용)
[백버퍼에 직접 출력]
```

- 3종 셰이더: `Shader_Deferred_Glow.hlsl` (일반), `Shader_Deferred_UI_Glow.hlsl` (UI)
- 글로우 색상/강도는 `GLOW_DESC`로 오브젝트별 개별 지정 가능
- **이펙트 글로우 배열**: `MRT_AllGlowDiffuse_0~9`로 최대 10개 이펙트를 개별 RT에 렌더 후 순차 블러

### 5.2 디스토션 (Distortion)

**3-pass 화면 왜곡**:
```
Pass 0/4: 디스토션 마스크 생성 (MRT_Distortion)
  - 각 DISTORTION_DESC의 위치에 빌보드 쿼드 렌더
  - Transform.LookAt(카메라) → 항상 카메라를 향함
  - fLifeTime 기반 자동 소멸 (isLoop 플래그로 루프 가능)

Pass 1: 백버퍼 + 마스크 = 왜곡된 백버퍼 (MRT_ResultDistortion_BackBuffer)
  - 마스크 UV 오프셋으로 백버퍼 샘플링 위치 왜곡

Pass 2: 최종 결과를 백버퍼에 출력
```

### 5.3 블랙아웃/화이트아웃

- 시간 기반 알파 보간: `m_fAccBlackTime / m_fBlackTime`
- `Switch_BlackOut()` → `m_isStartBlackOut` 토글
- `Draw_AllBlackOut()`: 풀스크린 쿼드에 검정+알파 렌더
- 화이트아웃: 스프라이트 와이프 연출 (`m_vWhiteDir` 방향, 콜백 `m_pDoneCheck`)

---

## 6. 플레이어 전용 디퍼드 파이프라인

일반 맵 오브젝트와 **별도 G-Buffer**를 사용하는 것이 가장 큰 특징:

```
캐릭터 A → MRT_Player (Diffuse/Normal/Depth/AuraMask)
         → MRT_LightAcc (캐릭터 A 전용 라이트 → Shade/Specular)
         → MRT_PlayerDefferd (Deferred 합성, 첫 캐릭터는 Clear, 이후 누적)
         → 오라 마스크 블러 (다운샘플 → Blur X → Blur Y → 합성)

캐릭터 B → 같은 과정 반복 (MRT_PlayerDefferd에 DoNotClear로 누적)

최종 → Render_PlayerBlur()로 전체 플레이어 결과 블러 합성
```

**설계 의도**: 격투 게임 특성상 캐릭터별 개별 라이팅(방향, 오라 색상)이 필요하기 때문에 캐릭터마다 독립된 라이트 패스를 수행한다.

---

## 7. 사용된 디자인 패턴

### 7.1 싱글톤 + 파사드
```cpp
// CRenderInstance — 싱글톤이면서 Renderer 내부의 모든 매니저 접근 창구
DECLARE_SINGLETON(CRenderInstance)

// Client/Engine은 항상 CRenderInstance를 통해 접근
CRenderInstance::Get_Instance()->Add_RenderObject(RG_PLAYER, this, &desc);
CRenderInstance::Get_Instance()->Begin_MRT(L"MRT_Player");
CRenderInstance::Get_Instance()->Render_Lights(...);
```
- `CRenderer`, `CTarget_Manager`, `CLight_Manager`, `CPicking` 모두 `CRenderInstance`를 통해서만 접근
- 파사드가 모든 내부 호출을 위임 → 외부에서 내부 구조 은닉

### 7.2 수집-소비 패턴 (Gather-Consume)
```cpp
// 수집: Late_Update 단계 (각 오브젝트가 자신을 등록)
renderer->Add_RenderObject(RG_PLAYER, this, &desc);

// 소비: Draw() 단계 (리스트 순회 후 클리어)
for (auto& obj : m_RenderObjects[RG_PLAYER]) {
    obj->Render(fTimeDelta);
    Safe_Release(obj);
}
m_RenderObjects[RG_PLAYER].clear();
```
- 매 프레임 `AddRef` → `Render` → `Release` → `clear`
- 별도 정렬 없이 등록 순서대로 처리 (격투 게임이라 오브젝트 수가 적음)

### 7.3 MRT 스택 패턴
```cpp
// Target_Manager가 이전 RTV/DSV를 저장하고 복원
Begin_MRT("MRT_Player") {
    m_pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOldDSV);  // 저장
    m_pContext->OMSetRenderTargets(iNumRTV, RenderTargets, ...); // 설정
}
End_MRT() {
    m_pContext->OMSetRenderTargets(1, &m_pOldRTV, m_pOldDSV);   // 복원
    Safe_Release(m_pOldRTV/DSV);
}
```
- 1단계 깊이만 지원 (중첩 Begin_MRT는 불가)
- `Begin_MRT_DoNotClear`로 누적 렌더 가능 (플레이어 합성에 사용)

### 7.4 카테고리 라이트
```cpp
// Light_Manager — 3종 라이트를 별도 컨테이너로 관리
list<CLight*>          m_Lights;        // 배경 (씬 전역)
map<string, CLight*>   m_PlayerLights;  // 캐릭터별 (키=캐릭터 이름)
map<string, CLight*>   m_EffectLights;  // 이펙트별 (키=이펙트 이름)
```
- 캐릭터별 개별 라이트 → 격투 게임의 "캐릭터마다 다른 조명" 연출

### 7.5 레퍼런스 카운팅 메모리 관리
- `CBase::AddRef()/Release()` 기반
- `Safe_AddRef`, `Safe_Release` 헬퍼
- 렌더 큐에 등록 시 AddRef, 소비 후 Release → **소유권 공유**

---

## 8. DirectX API 호출 지점과 래핑 방식

### 8.1 디바이스 초기화 (`CGraphic_Device`)
```
호출 지점                          래핑
──────────────────────────────────────────────
D3D11CreateDeviceAndSwapChain()   Ready_SwapChain()
CreateRenderTargetView()          Ready_BackBufferRenderTargetView()
CreateDepthStencilView()          Ready_DepthStencilRenderTargetView()
Present()                         CGraphic_Device::Present()
ClearRenderTargetView()           Clear_BackBuffer_View()
ClearDepthStencilView()           Clear_DepthStencil_View()
```

### 8.2 렌더 타겟 (`CRenderTarget`)
```
호출 지점                          래핑
──────────────────────────────────────────────
CreateTexture2D()                 Initialize() — RT 텍스처 생성
CreateRenderTargetView()          Initialize() — RTV 생성
CreateShaderResourceView()        Initialize() — SRV 생성
ClearRenderTargetView()           Clear()
CopyResource()                    Copy_RenderTarget()
```

### 8.3 MRT 전환 (`CTarget_Manager`)
```
호출 지점                          래핑
──────────────────────────────────────────────
OMGetRenderTargets()              Begin_MRT() — 이전 상태 저장
OMSetRenderTargets()              Begin_MRT() — 새 MRT 바인딩
                                  End_MRT()   — 이전 상태 복원
PSSetShaderResources(0, MAX, null) Begin_MRT() — SRV 언바인딩 (RW 충돌 방지)
```

### 8.4 셰이더 (`CShader` — Effects11 래핑)
```
호출 지점                          래핑
──────────────────────────────────────────────
D3DX11CompileEffectFromFile()     Initialize_Prototype()
GetVariableByName()->AsMatrix()   Bind_Matrix()
GetVariableByName()->SetRawValue() Bind_RawValue()
GetVariableByName()->AsShaderResource()->SetResource() Bind_ShaderResourceView()
GetPassByIndex()->Apply()         Begin(iPassIndex)
CreateInputLayout()               Initialize_Prototype()
IASetInputLayout()                Begin(iPassIndex)
```
- 모든 셰이더 리소스 바인딩이 `const char*` 상수 이름 기반
- Pass 인덱스로 기법 전환 (예: Pass 0 = Blur_X, Pass 1 = Blur_Y, Pass 2 = Glow합성, Pass 3 = 다운샘플, Pass 4 = Deferred합성)

### 8.5 버퍼 (`CVIBuffer`)
```
호출 지점                          래핑
──────────────────────────────────────────────
CreateBuffer()                    Create_Buffer()
IASetVertexBuffers()              Bind_Buffers()
IASetIndexBuffer()                Bind_Buffers()
IASetPrimitiveTopology()          Bind_Buffers()
DrawIndexed()                     Render()
```

### 8.6 섀도우 DSV (Renderer에서 직접)
```cpp
// CRenderer::Initialize() — 섀도우 전용 DSV 직접 생성
TextureDesc.Width = 1920 * 2;  // 섀도우맵 2배 해상도
TextureDesc.Height = 1080 * 2;
TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture);
m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV);
```

---

## 9. 참고할 만한 설계 판단

### 9.1 플레이어/맵 G-Buffer 분리
- **판단**: 플레이어와 맵 오브젝트를 별도 G-Buffer에 렌더
- **이유**: 격투 게임에서 캐릭터별 독립 라이팅, 오라 효과, 메탈릭 셰이딩 필요
- **참고점**: 캐릭터 강조 연출이 필요한 게임에 유효. 다만 RT 수가 급증하므로 메모리 비용 주의

### 9.2 글로우 이펙트 배열 (10슬롯)
- **판단**: `MRT_AllGlowDiffuse_0~9`로 이펙트별 개별 RT 할당
- **이유**: 서로 다른 글로우 색상/강도의 이펙트를 독립적으로 처리
- **참고점**: 이펙트 수가 제한적인 격투 게임에서 유효. RT 풀 크기 고정(10개)이므로 관리 단순

### 9.3 파사드 싱글톤 (CRenderInstance)
- **판단**: 모든 렌더 서브시스템을 단일 파사드로 래핑
- **이유**: Engine↔Renderer DLL 경계에서 최소한의 인터페이스만 노출
- **참고점**: DLL 분리 시 필수적. 다만 파사드가 비대해지는 단점 (200+ 줄 헤더)

### 9.4 Begin/End MRT 패턴
- **판단**: MRT 전환을 Begin/End 쌍으로 래핑, 이전 RTV/DSV 자동 복원
- **이유**: 복잡한 멀티패스에서 상태 관리 실수 방지
- **참고점**: 단순하고 실용적. 다만 1단계 깊이만 지원하므로 중첩 MRT가 필요하면 확장 필요

### 9.5 수집-소비 렌더 큐
- **판단**: 프레임마다 오브젝트가 자신을 렌더 그룹에 등록, Renderer가 소비 후 클리어
- **이유**: 동적 오브젝트 추가/삭제에 유연, 오브젝트가 자신의 렌더 그룹을 결정
- **참고점**: 정렬이 필요 없는 게임(격투 게임 등 오브젝트 수 소량)에 적합. 대규모 씬이면 정렬/컬링 추가 필요

### 9.6 디스토션의 빌보드 + 라이프타임
- **판단**: 디스토션을 월드 공간 빌보드 쿼드로 렌더, 시간 기반 자동 소멸
- **이유**: 타격 이펙트 등 일시적 화면 왜곡을 간단하게 구현
- **참고점**: `DISTORTION_DESC` 구조체 하나로 위치/크기/수명/루프 제어 — 재사용성 높음

### 9.7 Effects11 기반 셰이더 래핑
- **판단**: `ID3DX11Effect` 사용, Pass 인덱스로 기법 전환
- **이유**: 멀티패스 렌더링에서 Pass 관리 편의, 상수 이름 기반 바인딩
- **주의**: Effects11은 레거시 프레임워크. 현대 프로젝트에서는 직접 셰이더 컴파일이 권장됨

### 9.8 SRV 언바인딩 안전 처리
```cpp
// Begin_MRT 시작 시 모든 PS SRV를 null로 설정
ID3D11ShaderResourceView* pSRV[128] = { nullptr };
m_pContext->PSSetShaderResources(0, 128, pSRV);
```
- **이유**: 렌더 타겟을 SRV로 읽고 있는 상태에서 같은 텍스처에 RTV로 쓰면 DX 경고/에러 발생
- **참고점**: 모든 MRT 전환 전에 SRV 클리어는 필수적인 안전 패턴

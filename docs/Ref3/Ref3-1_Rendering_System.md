# 참고프로젝트3 — 렌더링 시스템 심화 분석

> **분석 대상**: CRenderer, CTarget, CTarget_Manager, CLight, CLight_Manager, CShader, CView_Texture
> **모드**: [분석]

---

## 1. 핵심 책임과 시스템 경계

### 렌더링 시스템이 담당하는 것
- **렌더 큐 관리**: 9단계 렌더 그룹별 오브젝트 분류·순회
- **Deferred Rendering 파이프라인**: G-Buffer 생성 → 라이트 패스 → 합성
- **후처리 체인**: Shadow Map → Glow → Edge Detection → Blur → Final Rendering
- **MRT(Multiple Render Targets)**: 렌더 타겟 생성·바인딩·해제
- **라이트 연산**: Directional/Point Light를 셰이더에 전달하여 스크린 스페이스 라이팅

### 시스템 경계 — 이것은 렌더링이 하지 않는다
- **오브젝트 로직**: Update는 씬/오브젝트 책임. Renderer는 `Render_GameObject()` 시점에만 개입
- **메시/텍스처 로드**: CStaticMesh, CDynamicMesh, CTexture가 담당
- **카메라**: CCamera 계열이 View/Proj 행렬을 디바이스에 세팅 (Renderer는 읽기만)
- **충돌**: 별도 시스템. 렌더링과 무관

---

## 2. 클래스 간 소유/참조 관계

```
CRenderer (컴포넌트, 사실상 싱글톤 — Clone()이 AddRef+this 반환)
├── 소유: m_pGraphicDev (LPDIRECT3DDEVICE9, AddRef)
├── 소유: m_pTargetMgr → CTarget_Manager (싱글톤, AddRef)
├── 소유: m_mapShader → map<tchar*, CShader*> (13개 셰이더, 직접 Create)
├── 소유: m_pTempBuff → CView_Texture (화면 크기 쿼드)
├── 소유: m_pDownSizeBuff → CView_Texture (1/4 크기 쿼드)
└── 참조: m_RenderList[RENDER_END] → list<CGameObject*> (매 프레임 AddRef/Release)

CTarget_Manager (싱글톤)
├── 소유: m_mapTargets → map<tchar*, CTarget*> (18개 렌더 타겟)
└── 소유: m_mapMRT → map<tchar*, list<CTarget*>> (MRT 그룹 2개)

CTarget
├── 소유: m_pTargetTexture (D3D 텍스처)
├── 소유: m_pRenderTarget (서피스)
├── 소유: m_pShadowDepthStencil (그림자용 깊이 버퍼, 조건부)
├── 참조: m_pOldSurface (바인딩 시 백업용)
├── 참조: m_pBackBuff (원래 백버퍼)
├── 참조: m_pDepthStencilBuffer (원래 깊이 버퍼)
└── 소유: m_pBufferCom → CView_Texture (디버그 표시용)

CLight_Manager (싱글톤)
└── 소유: m_LightList → list<CLight*>

CLight
├── 소유: m_pGraphicDev (AddRef)
├── 소유: m_pBufferCom → CView_Texture (풀스크린 쿼드)
└── 값: m_LightInfo (D3DLIGHT9 구조체)

CShader (컴포넌트, Clone 가능)
├── 소유: m_pGraphicDev (AddRef)
├── 소유: m_pEffect (LPD3DXEFFECT)
└── 소유: m_pErrBuffer (컴파일 에러 버퍼)

CView_Texture (CVIBuffer 상속)
└── 소유: VB/IB (부모 CVIBuffer에서 관리)
```

### 핵심 소유 원칙
- **누가 Create했으면 그가 Release**: Renderer가 Create한 셰이더/버퍼는 Renderer::Free()에서 해제
- **싱글톤 참조 시 AddRef**: Renderer가 Target_Manager를 참조할 때 AddRef, Free()에서 Release
- **렌더 리스트는 매 프레임 리셋**: Add_RenderList에서 AddRef, Clear_RenderList에서 Release

---

## 3. 한 프레임 렌더링 호출 흐름

```
CRenderer::Render_GameObject()
│
├─ 1. Render_Priority()                     스카이박스 등 최우선 (셰이더 없이 직접 렌더)
│
├─ 2. Render_Deferred()                     ◀ Deferred 핵심
│   ├─ [Shadow Pass] Target_Shadow에 렌더
│   │   ├─ Begin_SingleTarget("Target_Shadow")    4096×4096 R32F
│   │   ├─ Shader_ShadowMap: LightView·LightProj 세팅
│   │   ├─ DynamicMesh/StaticMesh → passIdx=100 (그림자 전용 패스)
│   │   └─ End_SingleTarget
│   │
│   ├─ [G-Buffer Pass] MRT_CNDS 4개 동시 출력
│   │   ├─ Begin_MRT("MRT_CNDS")
│   │   │   → Target_Color(A8R8G8B8), Target_Normal(A16B16G16R16F),
│   │   │     Target_Depth(A32B32G32R32F), Target_SpecularIntensity(A8B8G8R8)
│   │   ├─ Shader_DynamicMesh: 그림자 텍스처 바인딩 → DynamicMesh 렌더
│   │   ├─ Shader_StaticMesh: 그림자 텍스처 바인딩 → StaticMesh 렌더
│   │   └─ End_MRT("MRT_CNDS")
│   │
│   └─ [Depth2 Pass] Target_Depth2에 StaticMesh 깊이만 별도 기록
│
├─ 3. Render_Glow()                          글로우 소스 추출 + 블러
│   ├─ Target_Color의 알파 → Target_GlowSources 추출
│   ├─ DownSampling → Target_DownFilter (1/4)
│   ├─ Blur → Target_Blur (1/4)
│   └─ Color RGB + Blur 합성 → Target_Glow
│
├─ 4. Render_Edge("Target_Edge_Normal", "Target_Normal")    노멀 기반 외곽선
├─ 5. Render_Edge("Target_Edge_Depth", "Target_Depth")      깊이 기반 외곽선
│
├─ 6. Render_Light()                         ◀ 라이트 패스
│   ├─ Begin_MRT("MRT_Light")
│   │   → Target_Shade(A16B16G16R16F), Target_Specular(A16B16G16R16F)
│   ├─ Shader_Light: Normal+Depth+SpecIntensity 텍스처 바인딩
│   ├─ Light_Manager::Render_Light(pEffect)
│   │   └─ 각 CLight: passIdx(0=Directional, 1=Point) → 풀스크린 쿼드 드로우
│   └─ End_MRT("MRT_Light")
│
├─ 7. Render_Scene()                         ◀ 최종 합성 + DOF
│   ├─ Target_Scene에 Shader_Blend로 합성
│   │   Color × Shade + Specular + Glow + Edge
│   ├─ DownSampling → Target_DownScene (1/4)
│   ├─ Blur → Target_BlurScene (1/4)
│   └─ Shader_FinalRendering → 백버퍼 출력
│       BackColor, FadeColor, Depth 기반 DOF(블러 씬 vs 원본 씬 블렌딩)
│
├─ 8. Render_Default()                       MRT에 안 그려진 NoneAlpha 오브젝트
├─ 9. Render_Alpha()                         Z 정렬 후 알파 블렌딩
├─ 10. Render_Effect()                       Shader_Effect로 일괄 렌더 (Z 정렬)
├─ 11. Render_UI()                           직교 투영 전환 → Shader_UI → 복원
│
└─ 12. Clear_RenderList()                    전체 렌더 리스트 Release + Clear
```

---

## 4. 사용된 디자인 패턴

### 4-1. 싱글톤 (CTarget_Manager, CLight_Manager)
- `DECLARE_SINGLETON` / `IMPLEMENT_SINGLETON` 매크로
- Renderer가 생성자에서 `GetInstance()` + `AddRef()`로 참조 획득

### 4-2. 유사 싱글톤 (CRenderer)
- CComponent를 상속하지만 **Clone()이 `AddRef(); return this;`** → 복제 대신 공유
- 모든 GameObject가 같은 Renderer 인스턴스를 참조
- 컴포넌트 인터페이스를 유지하면서 실질적 싱글톤 동작

### 4-3. 팩토리 (CTarget, CLight, CShader)
- `static Create()` → 생성 + Ready → 실패 시 Safe_Release
- 생성자 private → Create만 허용

### 4-4. 전략 패턴 (렌더 패스별 셰이더 교체)
- Renderer가 셰이더를 맵(`m_mapShader`)으로 관리
- 각 렌더 패스에서 적절한 셰이더를 찾아 `pEffect->Begin()` → 오브젝트 순회 → `pEffect->End()`
- 오브젝트는 `Render_GameObject(pEffect, passIdx)`로 셰이더를 주입받아 렌더

### 4-5. 커맨드 + 큐 (렌더 리스트)
- 오브젝트가 Update에서 `Renderer::Add_RenderList(RENDER_ALPHA, this)` 호출 → 큐에 등록
- Renderer가 프레임 끝에 큐를 순회하며 일괄 처리 → Clear

### 4-6. Mediator (CTarget_Manager)
- Renderer ↔ Target 사이의 중재자
- Renderer는 타겟 이름(문자열)만 알고, Target의 내부 구현(서피스 교체 등)은 모름
- `Begin_MRT` / `End_MRT`로 복수 타겟을 한 번에 제어

---

## 5. DirectX API 호출 지점과 래핑 방식

### 5-1. 디바이스 직접 호출 (Renderer)
```cpp
// UI 렌더에서 직접 호출 — Z-Buffer 끄기/켜기, 투영 전환
m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
D3DXMatrixOrthoLH(&matCurProj, ...);
m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matCurProj);
```
→ **래핑 없이 직접 호출**. Renderer가 디바이스를 직접 소유하므로 가능.

### 5-2. CTarget의 래핑 (렌더 타겟 관리)
| DX9 API | CTarget 메서드 | 역할 |
|---------|---------------|------|
| `D3DXCreateTexture(D3DUSAGE_RENDERTARGET)` | `Ready_Target()` | 렌더 타겟 텍스처 생성 |
| `GetSurfaceLevel(0)` | `Ready_Target()` | 서피스 획득 |
| `GetRenderTarget(idx)` / `SetRenderTarget(idx)` | `SetUp_OnGraphicDev()` | 기존 RT 백업 + 새 RT 바인딩 |
| `SetDepthStencilSurface()` | `SetUp_OnGraphicDev()` | 그림자맵이면 전용 깊이 버퍼, 아니면 기본 깊이 버퍼 |
| `Clear(D3DCLEAR_TARGET)` | `Clear_Target()` | RT를 초기 색상으로 클리어 |
| `SetRenderTarget(idx, oldSurface)` | `Release_OnGraphicDev()` | 원래 RT 복원 |
| `SetTexture(stage, tex)` | `SetUp_OnShader()` → `pEffect->SetTexture()` | 셰이더에 텍스처 바인딩 |

→ **CTarget이 DX9 서피스 교체의 복잡성을 캡슐화**. 특히 Old Surface 백업/복원 패턴이 핵심.

### 5-3. CShader의 래핑 (이펙트 프레임워크)
| DX9 API | CShader 메서드 |
|---------|---------------|
| `D3DXCreateEffectFromFile()` | `Ready_Shader()` |
| `LPD3DXEFFECT` 반환 | `Get_EffectHandle()` |

→ **얇은 래퍼**. 셰이더 로드만 캡슐화하고, 파라미터 세팅(`SetMatrix/SetVector/SetTexture`)은 Renderer가 `pEffect`를 직접 조작.

### 5-4. CView_Texture의 래핑 (풀스크린 쿼드)
| DX9 API | CView_Texture |
|---------|---------------|
| `SetStreamSource` / `SetFVF` / `SetIndices` | `Render_Buffer()` |
| `DrawIndexedPrimitive(TRIANGLELIST)` | `Render_Buffer()` |

→ Pre-Transformed 좌표(`D3DFVF_VTXVIEWTEX`)로 화면 쿼드를 그리는 유틸리티. Deferred에서 필수.

### 5-5. CLight의 DX 호출
```cpp
m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);    // 현재 View 행렬 읽기
m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj); // Proj 행렬 읽기
D3DXMatrixInverse(&matViewProjInv, ...);               // ViewProj 역행렬 → 월드 좌표 복원용
```
→ 라이트가 G-Buffer의 깊이를 월드 좌표로 복원하기 위해 ViewProj 역행렬 계산.

---

## 6. 렌더 타겟 총정리

| 타겟 이름 | 크기 | 포맷 | 용도 |
|-----------|------|------|------|
| Target_Shadow | 4096² | R32F | 그림자 맵 (라이트 시점 깊이) |
| Target_Color | 화면 | A8R8G8B8 | 알베도 (G-Buffer) |
| Target_Normal | 화면 | A16B16G16R16F | 월드 노멀 (G-Buffer) |
| Target_Depth | 화면 | A32B32G32R32F | 뷰 깊이 (G-Buffer) |
| Target_SpecularIntensity | 화면 | A8B8G8R8 | 스페큘러 강도 (G-Buffer) |
| Target_Depth2 | 화면 | R32F | StaticMesh 전용 깊이 |
| Target_Edge_Normal | 화면 | A8R8G8B8 | 노멀 기반 외곽선 |
| Target_Edge_Depth | 화면 | A8R8G8B8 | 깊이 기반 외곽선 |
| Target_GlowSources | 화면 | A8R8G8B8 | 글로우 소스 마스크 |
| Target_DownFilter | 1/4 | A8R8G8B8 | 글로우 다운샘플링 |
| Target_Blur | 1/4 | A8R8G8B8 | 글로우 블러 |
| Target_Glow | 화면 | A8R8G8B8 | 글로우 합성 결과 |
| Target_Shade | 화면 | A16B16G16R16F | 디퓨즈 라이팅 |
| Target_Specular | 화면 | A16B16G16R16F | 스페큘러 라이팅 |
| Target_Scene | 화면 | A16B16G16R16F | 최종 합성 장면 |
| Target_DownScene | 1/4 | A16B16G16R16F | DOF용 다운샘플 |
| Target_BlurScene | 1/4 | A16B16G16R16F | DOF용 블러 |

**MRT 그룹**:
- `MRT_CNDS`: Color + Normal + Depth + SpecularIntensity (G-Buffer 4채널 동시 출력)
- `MRT_Light`: Shade + Specular (라이팅 2채널 동시 출력)

---

## 7. 설계 판단 분석 — 참고할 점과 한계

### 참고할 점

**1) Renderer를 컴포넌트로 위장한 유사 싱글톤**
- `Clone()`이 `AddRef(); return this;` → 모든 GameObject가 같은 인스턴스 공유
- 컴포넌트 시스템의 일관성을 유지하면서 전역 렌더러 역할 수행
- **장점**: 오브젝트가 `m_pRendererCom->Add_RenderList()` 형태로 자연스럽게 호출

**2) Target의 Old Surface 백업/복원 패턴**
```
SetUp_OnGraphicDev: GetRenderTarget(idx) → 백업, SetRenderTarget(idx, new) → 교체
Release_OnGraphicDev: SetRenderTarget(idx, old) → 복원
```
- DX9에서 RT 교체 시 반드시 필요한 패턴을 CTarget에 캡슐화
- 그림자맵일 경우 깊이 스텐실까지 교체/복원 — 조건 분기가 Target 내부에 숨겨짐

**3) MRT 관리를 태그 기반 그룹으로 추상화**
- `Add_MRT("MRT_CNDS", "Target_Color")` → 문자열로 타겟을 그룹에 등록
- `Begin_MRT` / `End_MRT` 호출만으로 4개 RT 동시 바인딩/해제
- 새 MRT 그룹 추가 시 코드 변경 최소화

**4) 후처리를 작은 함수 단위로 분리**
- `Render_DownSampling()`, `Render_Blur()`, `Render_Edge()` 등 재사용 가능한 빌딩 블록
- `Render_Blur`는 반복 횟수(`iBlurCount`)와 다운샘플 비율(`iDownSampleValue`)을 파라미터화
- Glow 파이프라인 = DownSampling + Blur + 합성 조합

**5) BadComputer 프리프로세서 분기**
- `#ifndef BadComputer`로 후처리(Shadow, Glow, Edge, DOF)를 통째로 스킵
- 저사양 대응을 컴파일 타임에 처리 — 런타임 분기보다 깔끔

**6) 셰이더를 Renderer가 중앙 관리**
- 개별 오브젝트가 셰이더를 들고 있지 않음 (Deferred 용도)
- Renderer가 패스별로 적절한 셰이더를 선택 → 오브젝트에 Effect 핸들 주입
- 오브젝트는 `Render_GameObject(pEffect, passIdx)` 시그니처만 구현하면 됨

### 한계/개선 가능 포인트

**1) 문자열 키 기반 검색 — `lstrcmp` 선형 탐색**
- `Find_Target()`이 `find_if + lstrcmp` 사용 → O(N) 탐색
- 해시맵이지만 `const _tchar*` 포인터를 키로 사용 (해시 함수 커스텀 필요)
- 개선: 문자열 해싱 또는 enum ID 기반으로 전환

**2) Renderer의 비대화**
- Renderer.cpp가 1150줄 — 셰이더 생성, RT 생성, 13개 렌더 함수 모두 포함
- 후처리 체인을 별도 클래스(PostProcessor)로 분리하면 유지보수 용이

**3) pEffect AddRef/Release 불일치**
- `Render_Edge`, `Render_Light`, `Render_Scene`에서 `pEffect->AddRef()` 후 `pEffect->Release()`
- `Render_Glow`, `Render_Blur`, `Render_DownSampling`에서는 AddRef 없이 사용
- 일관되지 않은 소유권 관리 → 참조 카운트 버그 가능성

**4) Clear_Target의 RT 전환 오버헤드**
- `Clear_Target()`이 RT를 교체 → 클리어 → 복원 (3번 서피스 전환)
- `Begin_MRT()`에서 4개 타겟 각각에 Clear_Target() 호출 → 총 12번 서피스 전환
- 개선: Clear와 SetUp을 합쳐서 서피스 전환 횟수를 줄일 수 있음

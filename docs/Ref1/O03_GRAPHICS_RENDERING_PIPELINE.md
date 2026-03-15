# O03. 그래픽스 & 렌더링 파이프라인

## 1. DX11 디바이스 구조 (CGraphic_Device)

### DX9 → DX11 디바이스 분리
```
DX9:  IDirect3DDevice9 (단일 객체로 모든 것 수행)
DX11: ID3D11Device        ← 리소스 생성 (메모리 할당, 멀티스레드 안전)
      ID3D11DeviceContext  ← 렌더링 명령 실행 (바인딩, Draw 호출)
      IDXGISwapChain       ← 프론트/백 버퍼 교환
```

### 초기화 과정
```
1. SwapChain 생성 (백버퍼 텍스처 포함)
2. 백버퍼 → RenderTargetView (RTV) 생성
3. 깊이/스텐실 Texture2D → DepthStencilView (DSV) 생성
```

### DX11 View 시스템 (핵심 개념)
```
ID3D11Texture2D (원본 텍스처)
├── ID3D11RenderTargetView (RTV) ← 렌더링 출력 대상으로 사용
├── ID3D11ShaderResourceView (SRV) ← 셰이더에서 읽기용으로 사용
└── ID3D11DepthStencilView (DSV) ← 깊이 테스트용으로 사용
```

> 하나의 텍스처에 용도별 View를 붙여 사용 → DX9의 `GetSurfaceLevel` / `SetRenderTarget`보다 유연.

---

## 2. 디퍼드 렌더링 개요

### 포워드 vs 디퍼드
```
포워드 (DX9, 현재):  각 오브젝트마다 조명 계산 → N개 오브젝트 × M개 조명
디퍼드 (DX11, 참고):  1차 기하정보만 저장 → 2차 조명 한번에 계산 → N + M
```

### 렌더타겟 목록 (총 9개)
| 렌더타겟 | 포맷 | 용도 |
|---------|------|------|
| Target_Diffuse | R8G8B8A8 | 물체의 색상(알베도) |
| Target_Normal | R16G16B16A16 | 월드 노멀 |
| Target_Depth | R32G32B32A32_FLOAT | 뷰 공간 깊이 |
| Target_World | R32G32B32A32_FLOAT | 월드 좌표 |
| Target_Shade | R16G16B16A16 | 조명 결과 (디퓨즈) |
| Target_Specular | R16G16B16A16 | 스펙큘러 결과 |
| Target_Shadow | R32G32B32A32_FLOAT | 그림자 맵 (고해상도 8192x4608) |
| Target_Final | R8G8B8A8 | 최종 합성 결과 |
| Target_Blur_X | R8G8B8A8 | 블러 중간 결과 |

### MRT (Multiple Render Target) 그룹
```
MRT_GameObjects → [Diffuse, Normal, Depth, World]  // G-Buffer
MRT_LightAcc   → [Shade, Specular]                 // 조명 누적
MRT_Shadow      → [Shadow]                          // 그림자 맵
MRT_Final       → [Final]                           // 최종 합성
MRT_Blur_X      → [Blur_X]                          // 후처리 블러
```

---

## 3. 렌더링 패스 순서 (CRenderer::Draw)

```
Pass 1: Render_Priority()   ← 스카이박스/배경 → MRT_Final에 직접 렌더
Pass 2: Render_Shadow()     ← 그림자 캐스터 → MRT_Shadow (고해상도 뷰포트)
Pass 3: Render_NonBlend()   ← 불투명 오브젝트 → MRT_GameObjects (G-Buffer)
Pass 4: Render_Lights()     ← 조명 처리 → MRT_LightAcc (풀스크린 쿼드)
Pass 5: Render_Combined()   ← Diffuse×Shade+Specular+Shadow → MRT_Final
Pass 6: Render_Blur()       ← 블러 X → MRT_Blur_X → 백버퍼에 블러 Y
Pass 7: Render_NonLights()  ← 조명 미적용 이펙트 → 백버퍼 직접
Pass 8: Render_Blend()      ← 반투명 → 백버퍼 직접
Pass 9: Render_UI()         ← UI → 백버퍼 직접
```

### 디퍼드 렌더링 흐름도
```
[G-Buffer 패스]
오브젝트 → Diffuse + Normal + Depth + World 렌더타겟에 기록

[조명 패스]
각 조명 × 풀스크린 쿼드 → Normal/Depth 읽기 → Shade + Specular 계산

[합성 패스]
Diffuse × Shade + Specular - Shadow 적용 → Final 렌더타겟

[후처리]
Final → 블러 X → 블러 Y → 백버퍼

[포워드 패스]
NonLight, Blend, UI → 백버퍼에 직접 렌더 (디퍼드 대상 아님)
```

---

## 4. 그림자 시스템 (CShadow)

### 구조
```cpp
SHADOW_DESC {
    _float3 vEye, vAt;           // 광원 시점
    _float fFovy, fNear, fFar;   // 광원 투영
    _float fAspect;
};
// View + Projection 행렬 저장 (D3DTS::VIEW, D3DTS::PROJECTION)
```

### 그림자 맵 렌더링
```
1. MRT_Shadow 바인딩 + 전용 DSV (8192×4608 고해상도)
2. 뷰포트를 g_iMaxWidth × g_iMaxHeight로 변경
3. SHADOW 그룹 오브젝트의 Render_Shadow() 호출
4. 뷰포트 원복
```

### 그림자 적용 (Combined 패스)
```
광원 View/Proj 행렬로 픽셀의 광원 공간 깊이 계산
→ Target_Shadow 깊이와 비교 → 그림자 여부 결정
```

---

## 5. Shader 시스템 (CShader)

### Effects11 기반
```cpp
m_pEffect: ID3DX11Effect*         // .fx 파일 컴파일 결과
m_InputLayouts: vector<ID3D11InputLayout*>  // 패스별 입력 레이아웃
m_iNumPasses: _uint                // 사용 가능한 패스 수
```

### 바인딩 API
```cpp
Bind_RawValue(name, pData, size)    ← 임의 데이터 (float, int 등)
Bind_Matrix(name, pMatrix)          ← 단일 행렬
Bind_Matrices(name, pMatrix, count) ← 행렬 배열 (본 행렬 등)
Bind_ShaderResource(name, pSRV)     ← 텍스처 1장
Bind_ShaderResources(name, ppSRVs, count) ← 텍스처 배열
Begin(iPassIndex)                   ← 패스 적용 (InputLayout + Apply)
```

### 디퍼드 셰이더 패스 구성 (Shader_Deferred.hlsl)
```
Pass 0: 디버그 렌더타겟 시각화
Pass 1: 디렉셔널 라이트 계산
Pass 2: 포인트 라이트 계산
Pass 3: Combined (Diffuse × Shade + Specular + Shadow)
Pass 4: 블러 X축
Pass 5: 블러 Y축
```

---

## 6. CPipeLine - 카메라 행렬 관리

```cpp
m_TransformationMatrix[VIEW]       ← 뷰 행렬 (카메라)
m_TransformationMatrix[PROJECTION] ← 프로젝션 행렬
m_TransformationMatrix_Inverse[VIEW/PROJ] ← 역행렬 (조명 패스에서 사용)
m_vCamPosition                     ← 카메라 월드 위치
```

### Update()
```
뷰 역행렬의 4행(Position)에서 카메라 위치 추출
→ 스펙큘러 계산 시 시선 벡터에 사용
```

> **현재 프로젝트와 차이**: 현재는 `D3DXMatrixLookAtLH`로 뷰 행렬 직접 설정. 참고에서는 카메라 오브젝트가 `Set_Transform(VIEW, ...)` 호출 → PipeLine이 보관.

---

## 7. CFrustum - 절두체 컬링

### 원리
```
1. NDC 공간 8개 꼭짓점 (±1, ±1, 0~1) 초기화
2. Update(): ProjInv × ViewInv → 월드 공간 8점 계산
3. 6개 평면 (Near, Far, Left, Right, Top, Bottom) 생성
4. isIn_WorldSpace(pos, radius): 월드 공간 판정
5. Transform_ToLocalSpace(): 로컬 공간 변환 후 판정
```

### 사용 시점
```
Update_Engine():
  ③ Priority_Update → ④ PipeLine::Update() → ⑤ Frustum::Update()
  → ⑥ Update()에서 절두체 컬링 적용 가능
```

---

## 8. CRenderTarget & CTarget_Manager

### CRenderTarget 구조
```cpp
ID3D11Texture2D*           m_pTexture2D;  // 실제 텍스처
ID3D11RenderTargetView*    m_pRTV;        // 렌더 출력 View
ID3D11ShaderResourceView*  m_pSRV;        // 셰이더 입력 View
_float4                    m_vClearColor; // 클리어 색상
```

### CTarget_Manager 역할
```
Add_RenderTarget()  → 렌더타겟 생성 및 보관
Add_MRT()           → MRT 그룹에 렌더타겟 등록
Begin_MRT()         → 해당 MRT의 모든 RTV를 출력 대상으로 바인딩
End_MRT()           → 원래 백버퍼 RTV로 복원
Bind_ShaderResource() → 렌더타겟의 SRV를 셰이더에 바인딩
```

---

## 9. 현재(DX9) → 참고(DX11) 렌더링 비교 요약

| 항목 | 현재 | 참고 |
|------|------|------|
| 렌더링 방식 | 포워드 | **디퍼드** + 포워드 혼합 |
| 렌더타겟 수 | 1 (백버퍼) | **9개** (G-Buffer + 조명 + 후처리) |
| 그림자 | 없음 | **Shadow Map** (8192×4608) |
| 후처리 | 없음 | **가우시안 블러** (2패스 분리) |
| 셰이더 | 고정 파이프라인 | **Effects11** (.fx, 멀티패스) |
| 조명 | 디바이스 라이트 (고정) | **셰이더 기반** (디퓨즈+스펙큘러) |
| 절두체 컬링 | 없음 | **CFrustum** (월드/로컬 판정) |
| 뷰포트 전환 | 없음 | **동적 뷰포트** (그림자 맵용) |

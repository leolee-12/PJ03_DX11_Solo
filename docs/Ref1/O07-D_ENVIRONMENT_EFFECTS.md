# O07-D: 환경/이펙트 오브젝트 (Terrain, Sky, Snow, Explosion, Sprite)

## 1. 전체 구성

```
Layer_BackGround:
  CTerrain   ×1  — 지형 (NONBLEND)
  CSky       ×1  — 스카이박스 (PRIORITY)
  CForkLift  ×10 — 정적 메시 (NONBLEND)

Layer_Effect:
  CSnow              ×1  — 눈 입자 (NONLIGHT)
  CParticle_Explosion ×1  — 폭발 입자 (NONLIGHT)
  CSprite            ×30 — 스프라이트 (BLEND)
```

**렌더 순서**: PRIORITY → NONBLEND(+G-Buffer) → Deferred Lighting → NONLIGHT → BLEND → UI

---

## 2. CTerrain — 지형

### 컴포넌트 구성

```cpp
class CTerrain final : public CGameObject {
    CShader*           m_pShaderCom;
    CTexture*          m_pTextureCom[TEXTURE_END];  // DIFFUSE, MASK, BRUSH (3종)
    CNavigation*       m_pNavigationCom;
    CVIBuffer_Terrain* m_pVIBufferCom;
};
```

### 3종 텍스처 시스템

| 텍스처 | 변수명 | 셰이더 바인딩 | 용도 |
|--------|--------|---------------|------|
| Diffuse | `TEXTURE_DIFFUSE` | `g_DiffuseTexture` | 지형 기본 색상 (배열) |
| Mask | `TEXTURE_MASK` | `g_MaskTexture` | 스플래팅 마스크 |
| Brush | `TEXTURE_BRUSH` | `g_BrushTexture` | 브러시 오버레이 |

**Diffuse**: `Bind_ShaderResources` (복수) — 텍스처 **배열**을 통째로 바인딩.
셰이더에서 Mask를 기반으로 여러 Diffuse 텍스처를 블렌딩한다 (Texture Splatting).

### Update 흐름

```cpp
void CTerrain::Priority_Update(_float fTimeDelta) {
    m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    // Nav Mesh 월드 행렬 갱신 → 디버그 렌더용
}

void CTerrain::Update(_float fTimeDelta) {
    m_pVIBufferCom->Culling(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
    // QuadTree 프러스텀 컬링 → DYNAMIC IB 갱신
}
```

**핵심**: Culling은 `Update`에서, Navigation 갱신은 `Priority_Update`에서 수행.
Terrain의 렌더 그룹은 **NONBLEND** → G-Buffer MRT에 기록된다.

### Render

```cpp
// Shader: VtxNorTex (Position + Normal + TexCoord)
m_pShaderCom->Begin(0);       // 패스 0: 디퓨즈 + 노멀 + 깊이 + 월드 좌표
m_pVIBufferCom->Bind_Buffers();
m_pVIBufferCom->Render();     // DYNAMIC IB에 의해 컬링된 삼각형만 렌더
```

---

## 3. CSky — 스카이박스

### 컴포넌트 구성

```cpp
class CSky final : public CGameObject {
    CShader*        m_pShaderCom;     // VtxCube 셰이더
    CTexture*       m_pTextureCom;    // 큐브맵 텍스처
    CVIBuffer_Cube* m_pVIBufferCom;   // 큐브 메시 (8정점)
};
```

### 카메라 추적

```cpp
void CSky::Late_Update(_float fTimeDelta) {
    // 카메라 위치를 스카이박스 위치로 설정
    m_pTransformCom->Set_State(STATE::POSITION,
        XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
}
```

**원리**: 스카이박스를 카메라 위치에 고정하면 카메라가 어디로 이동해도
큐브가 항상 같은 거리에 보인다 → 무한히 먼 배경 효과.

### PRIORITY 렌더 그룹

```
렌더 순서:
  PRIORITY → 스카이박스 + 카메라 (MRT 바인딩 없이 직접 백버퍼에)
  NONBLEND → G-Buffer MRT에 기록 (Terrain, Monster, ForkLift 등)
  ...
```

**PRIORITY가 먼저 렌더되는 이유**: 깊이 버퍼에 먼저 그려도
이후 NONBLEND 오브젝트가 Z-test를 통과하면 덮어쓴다.
일반적으로 스카이박스 셰이더는 **Z = 1.0 (최대 깊이)**로 출력하여
다른 모든 오브젝트보다 뒤에 위치하게 한다.

### 텍스처 바인딩

```cpp
m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2);
// 인덱스 2 = 큐브맵 텍스처 슬롯 (TextureCube SRV)
```

---

## 4. CForkLift — 정적 메시 오브젝트

### 헤더 구조

```cpp
class CForkLift final : public CGameObject {
    CShader* m_pShaderCom;    // NonAnim 셰이더
    CModel*  m_pModelCom;     // NonAnim 모델 (FBX)
};
```

**구현 파일(ForkLift.cpp)이 레포에 누락**되어 있지만, 헤더와 Level_GamePlay의
생성 코드로부터 구조를 파악할 수 있다:

- CModel `TYPE::NONANIM` — 정적 메시 (본/애니메이션 없음)
- 10개 인스턴스를 for 루프로 생성 (각각 독립적인 CGameObject)
- 렌더 그룹: NONBLEND (G-Buffer에 기록)
- Monster처럼 랜덤 위치로 배치하거나, 고정 위치일 수 있음

**GPU 인스턴싱과의 차이**: ForkLift는 각각이 독립 GameObject이므로
Draw call이 10회 발생한다. Snow/Explosion은 인스턴싱으로 1회 Draw call.

---

## 5. CSnow — 눈 입자 (Rect Instancing)

### 컴포넌트 구성

```cpp
class CSnow final : public CGameObject {
    CShader*                   m_pShaderCom;    // VtxPosTexInstanceParticle
    CTexture*                  m_pTextureCom;   // 눈 텍스처
    CVIBuffer_Rect_Instancing* m_pVIBufferCom;  // 수백 개의 빌보드 Rect
};
```

### 초기화

```cpp
m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(64.0f, 0.f, 64.0f, 1.f));
// Terrain 중앙 (128×128의 절반 = 64) 위에 배치
```

### Update — Drop 애니메이션

```cpp
void CSnow::Update(_float fTimeDelta) {
    m_pVIBufferCom->Drop(fTimeDelta);
    // VIBuffer_Rect_Instancing::Drop()에서:
    //   - 각 인스턴스의 Y축 하강 (Speed × TimeDelta)
    //   - Lifetime 감소 → 0 이하이면 초기 위치로 리셋
    //   - WRITE_NO_OVERWRITE로 인스턴스 VB 갱신
}
```

### Render — NONLIGHT 그룹

```cpp
m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this);
```

**NONLIGHT를 사용하는 이유**:
- 눈 입자는 자체 발광/반투명 효과
- G-Buffer MRT(NONBLEND)를 거치면 디퍼드 조명이 적용됨
- 조명 없이 원본 색상 그대로 출력하려면 **디퍼드 파이프라인 이후**에 렌더해야 함

```
렌더 파이프라인:
  NONBLEND  → G-Buffer 기록 → Deferred Lighting → Combined
  NONLIGHT  → Combined 결과 위에 직접 렌더 (조명 계산 스킵)
  BLEND     → 알파 블렌딩으로 합성
```

### 셰이더

```cpp
// Shader: VtxPosTexInstanceParticle
// 입력: Position + TexCoord (슬롯0) + 4×float4 월드행렬행 + Lifetime (슬롯1)
m_pShaderCom->Begin(0);
m_pVIBufferCom->Bind_Buffers();  // IASetVertexBuffers(2개 VB)
m_pVIBufferCom->Render();        // DrawIndexedInstanced
```

---

## 6. CParticle_Explosion — 폭발 입자 (Point Instancing)

### 컴포넌트 구성

```cpp
class CParticle_Explosion final : public CGameObject {
    CShader*                    m_pShaderCom;    // VtxPosInstanceParticle
    CTexture*                   m_pTextureCom;   // Snow 텍스처 재사용
    CVIBuffer_Point_Instancing* m_pVIBufferCom;  // 포인트 인스턴싱
};
```

### Update — Spread 애니메이션

```cpp
void CParticle_Explosion::Update(_float fTimeDelta) {
    m_pVIBufferCom->Spread(fTimeDelta);
    // VIBuffer_Point_Instancing::Spread()에서:
    //   - 각 인스턴스가 랜덤 방향으로 퍼져나감
    //   - Lifetime 감소 → 0 이하이면 원점으로 리셋
    //   - WRITE_NO_OVERWRITE로 인스턴스 VB 갱신
}
```

### Render — 카메라 위치 전달

```cpp
HRESULT CParticle_Explosion::Render() {
    // ... WVP 행렬 바인딩 ...

    // Point Instancing 전용: 카메라 위치 전달
    m_pShaderCom->Bind_RawValue("g_vCamPosition",
        m_pGameInstance->Get_CamPosition(), sizeof(_float4));

    // ...
}
```

**카메라 위치가 필요한 이유**: Point 토폴로지는 정점 1개 → 지오메트리 셰이더에서
카메라를 향하는 빌보드 쿼드로 확장한다. 이때 카메라 방향 벡터가 필요하다.

### Snow vs Explosion 비교

| 항목 | CSnow | CParticle_Explosion |
|------|-------|---------------------|
| **버퍼** | Rect Instancing | Point Instancing |
| **토폴로지** | TriangleList (인덱스 6) | PointList |
| **빌보드** | VS에서 처리 가능 | **GS에서 Point → Quad 확장** |
| **Draw** | DrawIndexedInstanced | DrawInstanced |
| **애니메이션** | Drop (Y축 하강) | Spread (방사 확산) |
| **카메라 위치** | 불필요 | **Bind_RawValue로 전달** |
| **텍스처** | Snow 전용 | Snow 텍스처 **재사용** |
| **셰이더** | VtxPosTexInstanceParticle | VtxPosInstanceParticle |
| **렌더 그룹** | NONLIGHT | NONLIGHT |

---

## 7. CSprite — 스프라이트 애니메이션 (알파 블렌딩)

### 컴포넌트 구성

```cpp
class CSprite final : public CGameObject {
    CShader*       m_pShaderCom;    // VtxPosTex
    CTexture*      m_pTextureCom;   // Explosion 텍스처 (90장 시퀀스)
    CVIBuffer_Rect* m_pVIBufferCom; // 단일 사각형
    _float         m_fFrame = 0.f;  // 현재 프레임 인덱스
};
```

### 초기화 — 랜덤 위치

```cpp
m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
    m_pGameInstance->Random(0.f, 10.f),   // X: 0~10 랜덤
    2.0f,                                  // Y: 고정 높이
    m_pGameInstance->Random(0.f, 10.f),   // Z: 0~10 랜덤
    1.f));
```

30개 인스턴스가 각각 다른 랜덤 위치에 생성된다.

### Update — 프레임 애니메이션

```cpp
void CSprite::Update(_float fTimeDelta) {
    m_fFrame += 90.f * fTimeDelta;  // 초당 90프레임 진행
    if (m_fFrame >= 90.0f)
        m_fFrame = 0.f;             // 루프
}
```

**90장 텍스처 시퀀스**: 폭발 애니메이션을 90장의 개별 텍스처로 저장.
`m_fFrame`이 float이므로 소수점 이하는 버려지고 정수 인덱스로 사용된다.

### Render — BLEND + 깊이 텍스처

```cpp
HRESULT CSprite::Render() {
    // WVP 바인딩...

    // ① 현재 프레임의 텍스처 바인딩
    m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",
        static_cast<_uint>(m_fFrame));  // float → uint 변환 = 프레임 인덱스

    // ② G-Buffer의 깊이 타겟 바인딩
    m_pGameInstance->Bind_RT_ShaderResource(
        TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture");

    // ③ 셰이더 패스 1 (소프트 파티클)
    m_pShaderCom->Begin(1);
    // ...
}
```

### 소프트 파티클 (Soft Particle) 기법

```
패스 0 (일반):  알파 블렌딩만 적용
패스 1 (소프트): 깊이 비교로 경계 부드럽게 처리

원리:
  - Target_Depth에서 해당 픽셀의 장면 깊이 (SceneDepth) 읽기
  - 현재 파티클의 깊이 (ParticleDepth) 계산
  - 차이가 작으면 → 알파를 줄여서 페이드아웃
  - 결과: 지형과 겹치는 부분의 날카로운 경계가 사라짐
```

**BLEND 렌더 그룹**: NONLIGHT 이후에 렌더되며, 알파 블렌딩이 활성화된
상태에서 백버퍼에 직접 합성된다.

### 컴포넌트 출처 차이

```cpp
// VIBuffer_Rect: STATIC 레벨 (항상 상주)
Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"), ...);

// Explosion 텍스처: GAMEPLAY 레벨 (씬 전환 시 해제)
Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Explosion"), ...);
```

**STATIC vs GAMEPLAY**: 범용 지오메트리(Rect)는 STATIC 레벨에 상주하여
씬 전환 시에도 유지된다. 게임 전용 리소스(텍스처)는 GAMEPLAY에 배치.

---

## 8. 렌더 그룹별 처리 비교

| 렌더 그룹 | MRT | 조명 | 알파 | 대상 |
|-----------|-----|------|------|------|
| **PRIORITY** | X | X | X | Sky (카메라 Z=1.0) |
| **NONBLEND** | **MRT_GameObjects** | **디퍼드** | X | Terrain, Monster, ForkLift, Body, Weapon |
| **SHADOW** | Shadow Depth Map | X | X | Body |
| **NONLIGHT** | X | **스킵** | X | Snow, Explosion (자체 색상) |
| **BLEND** | X | X | **O** | Sprite (소프트 파티클) |

### 렌더 순서 (Renderer.cpp)

```
① Render_Priority()     — Sky 등
② Render_NonBlend()     — MRT 기록 (G-Buffer)
③ Render_Shadow()       — 그림자 깊이맵
④ Render_LightAcc()     — 디퍼드 조명 계산
⑤ Render_Combined()     — Shade + Specular 합성
⑥ Render_Blur()         — 후처리 블러
⑦ Render_NonLights()    — 조명 없는 이펙트
⑧ Render_Blend()        — 알파 블렌딩 이펙트
⑨ Render_UI()           — UI 오버레이
```

---

## 9. 환경/이펙트 패턴 요약

### 공통 패턴

모든 클라이언트 오브젝트가 따르는 동일한 구조:

```cpp
class CXxx final : public CGameObject {
    // 1. 컴포넌트 멤버
    CShader*   m_pShaderCom;
    CTexture*  m_pTextureCom;
    CVIBuffer* m_pVIBufferCom;

    // 2. Ready_Components() — Clone으로 컴포넌트 획득
    // 3. Update() — 로직 갱신
    // 4. Late_Update() — 렌더 그룹 등록
    // 5. Render() — WVP + 텍스처 바인딩 + Begin + Bind_Buffers + Render
    // 6. Free() — Safe_Release 체인
};
```

### 오브젝트별 고유 포인트

| 오브젝트 | 고유 특징 |
|---------|----------|
| **Terrain** | 3종 텍스처(Splatting), QuadTree Culling, Navigation |
| **Sky** | 카메라 위치 추적, 큐브맵, PRIORITY 렌더 |
| **ForkLift** | NonAnim 모델, 가장 단순한 정적 메시 |
| **Snow** | Rect Instancing + Drop, NONLIGHT |
| **Explosion** | Point Instancing + Spread + GS 빌보드, 카메라 위치 필요 |
| **Sprite** | 프레임 애니메이션(90장), 소프트 파티클(깊이 비교), BLEND |

---

## 10. 핵심 정리

| 항목 | 설명 |
|------|------|
| **스카이박스** | 카메라 위치 = 스카이 위치, PRIORITY로 먼저 렌더 (Z=1.0) |
| **Terrain** | Splatting (Diffuse 배열 + Mask), QuadTree Culling, Navigation |
| **인스턴싱 이펙트** | Drop(눈)/Spread(폭발), NONLIGHT (조명 무시) |
| **Point→빌보드** | Explosion은 GS에서 Point→Quad 변환, 카메라 방향 필요 |
| **스프라이트** | 텍스처 시퀀스(90장) + 소프트 파티클(Target_Depth 비교) |
| **렌더 그룹** | 용도별 분리: NONBLEND(G-Buffer) / NONLIGHT(직접) / BLEND(알파) |
| **컴포넌트 레벨** | STATIC(범용, 상주) vs GAMEPLAY(씬 전용, 해제) |

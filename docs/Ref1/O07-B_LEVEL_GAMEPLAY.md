# O07-B: Level_GamePlay 레이어 구성 + 조명/그림자 설정

## 1. 초기화 순서

```cpp
HRESULT CLevel_GamePlay::Initialize()
{
    Ready_Lights();                              // ① 조명 + 그림자
    Ready_Layer_Camera(TEXT("Layer_Camera"));     // ② 카메라
    Ready_Layer_Player(TEXT("Layer_Player"));     // ③ 플레이어
    Ready_Layer_Monster(TEXT("Layer_Monster"));   // ④ 몬스터
    Ready_Layer_BackGround(TEXT("Layer_BackGround")); // ⑤ 환경
    Ready_Layer_Effect(TEXT("Layer_Effect"));     // ⑥ 이펙트
}
```

**순서 중요**: 카메라가 먼저 생성되어야 Priority_Update에서 View/Proj 행렬이 설정된다.

---

## 2. 조명 설정 — Ready_Lights

### 디렉셔널 라이트 (1개)

```cpp
LIGHT_DESC LightDesc{};
LightDesc.eType     = LIGHT::DIRECTIONAL;
LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);   // 대각선 아래
LightDesc.vDiffuse   = _float4(0.6f, 0.6f, 0.6f, 1.f); // 회색 (60%)
LightDesc.vAmbient   = _float4(0.3f, 0.3f, 0.3f, 1.f); // 환경광 (30%)
LightDesc.vSpecular  = _float4(1.f, 1.f, 1.f, 1.f);    // 하이라이트 (100%)
```

### 포인트 라이트 (2개)

```cpp
// 빨간 포인트 라이트
LightDesc.eType     = LIGHT::POINT;
LightDesc.vPosition = _float4(20.0f, 5.0f, 10.f, 1.f);
LightDesc.fRange    = 10.0f;
LightDesc.vDiffuse  = _float4(1.f, 0.f, 0.f, 1.f);  // 빨강

// 초록 포인트 라이트
LightDesc.vPosition = _float4(30.0f, 5.0f, 10.f, 1.f);
LightDesc.fRange    = 10.0f;
LightDesc.vDiffuse  = _float4(0.f, 1.f, 0.f, 1.f);  // 초록
```

### 그림자 라이트

```cpp
SHADOW_DESC ShadowDesc{};
ShadowDesc.vEye    = _float3(-10.f, 20.f, -10.f);  // 라이트 위치
ShadowDesc.vAt     = _float3(0.f, 0.f, 0.f);       // 라이트가 바라보는 점
ShadowDesc.fFovy   = XMConvertToRadians(60.0f);
ShadowDesc.fAspect = (float)WinSizeX / WinSizeY;
ShadowDesc.fNear   = 0.1f;
ShadowDesc.fFar    = 500.0f;
```

**그림자 라이트는 Perspective**: 포인트/스팟 라이트 느낌의 그림자.
Orthographic으로 변경하면 디렉셔널 라이트 그림자가 된다.

---

## 3. 레이어 구성

### Layer_Camera

```cpp
CCamera_Free::CAMERA_FREE_DESC Desc{};
Desc.vEye = _float3(0.f, 10.f, -6.f);
Desc.vAt  = _float3(0.f, 0.f, 0.f);
Desc.fFovy = XMConvertToRadians(60.0f);
Desc.fNear = 0.1f;
Desc.fFar  = 500.f;
Desc.fSensor = 0.1f;           // 마우스 감도
Desc.fSpeedPerSec = 10.f;
Desc.fRotationPerSec = XMConvertToRadians(120.0f);
```

### Layer_Player — 1개

```cpp
Add_GameObject(GAMEPLAY, TEXT("Prototype_GameObject_Player"),
    GAMEPLAY, pLayerTag);
```

### Layer_Monster — 10개

```cpp
for (size_t i = 0; i < 10; i++)
    Add_GameObject(GAMEPLAY, TEXT("Prototype_GameObject_Monster"),
        GAMEPLAY, pLayerTag);
```

### Layer_BackGround

```cpp
Add_GameObject(..., TEXT("Prototype_GameObject_Terrain"), ...);    // 지형 1개
Add_GameObject(..., TEXT("Prototype_GameObject_Sky"), ...);        // 스카이박스 1개
for (size_t i = 0; i < 10; i++)
    Add_GameObject(..., TEXT("Prototype_GameObject_ForkLift"), ...); // 지게차 10개
```

### Layer_Effect

```cpp
Add_GameObject(..., TEXT("Prototype_GameObject_Snow"), ...);                // 눈 1개
Add_GameObject(..., TEXT("Prototype_GameObject_Particle_Explosion"), ...);  // 폭발 1개
for (size_t i = 0; i < 30; i++)
    Add_GameObject(..., TEXT("Prototype_GameObject_Sprite"), ...);          // 스프라이트 30개
```

---

## 4. 전체 씬 오브젝트 요약

| 레이어 | 오브젝트 | 수량 | 렌더 그룹 |
|--------|---------|------|----------|
| Camera | Camera_Free | 1 | PRIORITY |
| Player | Player (Body+Weapon) | 1 | NONBLEND + SHADOW |
| Monster | Monster | 10 | NONBLEND + SHADOW |
| BackGround | Terrain | 1 | NONBLEND |
| BackGround | Sky | 1 | PRIORITY |
| BackGround | ForkLift | 10 | NONBLEND |
| Effect | Snow (Rect Instance) | 1 | NONLIGHT |
| Effect | Explosion (Point Instance) | 1 | NONLIGHT |
| Effect | Sprite | 30 | BLEND |

**총 오브젝트**: 56개 (파츠 제외)

---

## 5. Add_GameObject 호출 구조

```cpp
m_pGameInstance->Add_GameObject(
    ENUM_CLASS(LEVEL::GAMEPLAY),   // 프로토타입 레벨 슬롯
    TEXT("Prototype_..."),          // 프로토타입 태그
    ENUM_CLASS(LEVEL::GAMEPLAY),   // 오브젝트 배치 레벨 슬롯
    pLayerTag,                     // 레이어 이름
    &Desc);                        // 초기화 파라미터 (선택)
```

**내부 동작**: Prototype_Manager에서 Clone → Object_Manager의 레이어에 추가.

---

## 6. 디퍼드 렌더링 파이프라인과의 관계

```
Ready_Lights()에서 설정한 조명들:
  ↓
디렉셔널 라이트 → Renderer::Render_LightAcc_Directional (패스1, 전체화면)
포인트 라이트 ×2 → Renderer::Render_LightAcc_Point (패스2, 전체화면)
  ↓
Combined 패스에서 Shade + Specular 합성
  ↓
그림자 라이트 → Renderer::Render_Shadow (라이트 공간 깊이맵)
  → Combined 패스에서 Target_ShadowDepth 비교
```

---

## 7. 핵심 정리

| 항목 | 설명 |
|------|------|
| **조명** | 디렉셔널 1 + 포인트 2 (빨강/초록) + 그림자 1 |
| **레이어** | Camera, Player, Monster, BackGround, Effect (5개) |
| **오브젝트 수** | 56개 (Player 1, Monster 10, ForkLift 10, Sprite 30 등) |
| **초기화 순서** | 조명 → 카메라 → 플레이어 → 몬스터 → 환경 → 이펙트 |
| **Add_GameObject** | 프로토타입 Clone → 레이어에 추가 |
| **그림자** | Perspective 투영 (Eye=-10,20,-10 → At=0,0,0) |

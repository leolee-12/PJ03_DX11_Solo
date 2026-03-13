# 참고프로젝트2 — UI 시스템 심화 분석

> 분석 범위: `CUIObject`, `CUI_Manager`, `CBattleInterface_Manager`, `CUI_Define`, 대표 UI 구체 클래스(`CUI_HpGauge`, `CUI_Timer`, `CUI_Combo`, `CUI_ComboNumber`, `CUI_SkillGauge`)
> 분석 대상: 헤더 + cpp 구현부

---

## 1. 시스템 핵심 책임과 경계

### 책임 분리

| 계층 | 클래스 | 위치 | 책임 |
|------|--------|------|------|
| **UI 베이스** | `CUIObject` | Client | 직교 투영 설정, 좌/우 미러링, 이동 애니메이션, 해상도 보정 |
| **UI 매니저** | `CUI_Manager` | Client (싱글톤) | 플레이어 슬롯 관리, UI 리스트(Top/Mid/Bot), 옵션 UI 토글 |
| **전투 데이터** | `CBattleInterface_Manager` | Client (싱글톤) | HP/콤보/기 게이지/스파킹 등 전투 상태 데이터 중앙 관리 |
| **UI 요소들** | `CUI_HpGauge`, `CUI_Timer` 등 | Client | 각 UI 요소의 시각적 표현 + 셰이더 바인딩 |
| **렌더러** | `CRenderer` | Renderer DLL | `RG_UI` 렌더 그룹으로 UI 일괄 렌더 |

### 경계

```
┌────────────────────────────────────────────────────────┐
│  Client                                                 │
│                                                         │
│  Level_GamePlay::Ready_UI()                             │
│    → Add_GameObject_ToLayer("Prototype_GameObject_UI_*")│
│    → CUIObject::Clone() → Initialize(UI_DESC)          │
│                                                         │
│  CUI_Manager (싱글톤)                                   │
│    ├── m_pPawnArray[4] ← Character 참조                 │
│    ├── m_ListTopUI / m_ListMidUI / m_ListBotUI          │
│    └── 옵션/시작/종료 UI 생성                            │
│                                                         │
│  CBattleInterface_Manager (싱글톤)                       │
│    └── Ki/HP/Combo/Sparking 전투 상태 데이터             │
│                                                         │
│  CUI_* (수십 개 구체 클래스)                             │
│    └── Late_Update에서 RG_UI 렌더 그룹 등록              │
├────────────────────────────────────────────────────────┤
│  Renderer DLL                                           │
│  CRenderer::Render_UI()                                 │
│    → 등록된 UI 오브젝트 순차 Render() 호출               │
│    → 직교 투영 + UI 전용 셰이더 패스                     │
└────────────────────────────────────────────────────────┘
```

핵심 경계:
- UI 오브젝트는 **CGameObject를 상속**하므로 일반 오브젝트와 동일한 Update/Render 루프를 탐
- **데이터 읽기 방향**: UI → `CUI_Manager` → `CCharacter::PawnDesc` (단방향 풀 방식)
- UI는 **전투 상태를 변경하지 않음** — 읽기 전용 뷰 역할
- 예외: `CBattleInterface_Manager`는 전투 로직 매니저로 UI와 전투 양쪽에서 접근

---

## 2. 클래스 간 소유/참조 관계

```
CUI_Manager (싱글톤, CBase 상속)
  ├── [참조] CGameInstance* m_pGameInstance
  ├── [참조] CCharacter* m_pPawnArray[4]  (LPLAYER1, LPLAYER2, RPLAYER1, RPLAYER2)
  ├── [참조] list<CUIObject*> m_ListTopUI   (Y < 160)
  ├── [참조] list<CUIObject*> m_ListMidUI   (160 ≤ Y < 450)
  └── [참조] list<CUIObject*> m_ListBotUI   (Y ≥ 450)

CBattleInterface_Manager (싱글톤, CBase 상속)
  ├── [참조] CCharacter* m_p1TeamCharacter[3]
  ├── [참조] CCharacter* m_p2TeamCharacter[3]
  └── [소유] 전투 상태: iHitCount, iKiGuage, iKiNumber, bSparking 등

CUIObject (추상, CGameObject 상속)
  ├── [참조] CUI_Manager* m_pUI_Manager    (싱글톤 참조, AddRef/Release)
  ├── [참조] CCharacter* m_pMainPawn       (UI_Manager에서 획득)
  ├── [참조] CCharacter* m_pSubPawn        (UI_Manager에서 획득)
  ├── [소유] CShader* m_pShaderCom         (Clone, UI 전용 셰이더)
  ├── [소유] CTexture* m_pTextureCom       (Clone)
  ├── [소유] CVIBuffer_Rect* m_pVIBufferCom (Clone, 사각형 쿼드)
  └── [상속] CTransform* m_pTransformCom   (CGameObject에서)

CUI_HpGauge : CUIObject
  └── [소유] CTexture* m_pMaskTexture      (HP 애니메이션 마스크)

CUI_SkillGauge : CUI_Skill : CUIObject
  └── [소유] CTexture* m_pEffectTexture    (이펙트 텍스처)

CUI_Combo (추상 중간 클래스) : CUIObject
  └── CUI_ComboNumber : CUI_Combo          (숫자 자릿수별 인스턴스)
  └── CUI_ComboFont : CUI_Combo            ("COMBO" 텍스트)
  └── CUI_ComboEffect : CUI_Combo          (이펙트)
```

### 생성 체인

```
Level_GamePlay::Ready_UI()
  │
  ├─ CUI_Manager::InitUIObject()
  │   └─ Layer_Character 순회 → m_pPawnArray[slot] = Character
  │
  └─ for (LEFT, RIGHT):
      ├─ Add_GameObject_ToLayer("Prototype_GameObject_UI_HpPanel", "Layer_UI_HpGauge", &desc)
      │   → CUI_HpPanel::Clone(desc)
      │     → CUIObject::Initialize(desc)
      │       ├─ eLRPos 결정 (LEFT/RIGHT)
      │       ├─ RIGHT면 m_fPosX = 1280 - X, m_fSizeX *= -1 (미러링)
      │       ├─ m_pMainPawn / m_pSubPawn 설정
      │       └─ 해상도 오프셋 계산
      │     → Ready_Components()
      │       ├─ Add_Component("Shader_UI_VtxRect") → m_pShaderCom
      │       ├─ Add_Component("VIBuffer_Rect") → m_pVIBufferCom
      │       └─ Add_Component("Texture_UI_HpGauge") → m_pTextureCom
      │     → Set_UI_Setting(sizeX, sizeY, posX, posY, depth)
      │
      ├─ Add_GameObject_ToLayer("UI_HpGauge", ...)
      ├─ Add_GameObject_ToLayer("UI_SkillGauge", ...)
      ├─ Add_GameObject_ToLayer("UI_Timer", ...)
      └─ Add_GameObject_ToLayer("UI_ComboNumber", ...) × 3 (자릿수별)
```

총 **32개 UI 프로토타입**이 게임플레이 레벨에서 생성됨.

---

## 3. 주요 함수의 호출 흐름 (한 프레임 기준)

### 3.1 Update 단계

```
CObject_Manager::Update(fTimeDelta)
  └─ 모든 레이어 순회 → CUIObject::Update(fTimeDelta)
      ├─ InitPlayer()  ← 매 프레임 CUI_Manager에서 캐릭터 참조 갱신
      └─ (서브클래스별 로직)
          ├─ CUI_HpGauge: Animation() → MoveAnimUI() (등장 애니메이션)
          ├─ CUI_ComboNumber: ScaleAnimation() + EndAlphaEffect()
          └─ CUI_SkillGauge: EffectRadioValue() (반짝임 효과)
```

### 3.2 Late_Update 단계 (렌더 등록)

```
CObject_Manager::Late_Update(fTimeDelta)
  └─ CUIObject::Late_Update(fTimeDelta)
      ├─ m_bCharaStun = m_pMainPawn->Get_PawnDesc().bStun  ← 캐릭터 상태 풀
      └─ (서브클래스)
          └─ m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this)
              ↑ 모든 UI 구체 클래스가 RG_UI 렌더 그룹에 자신을 등록
```

### 3.3 Render 단계

```
CRenderer::Draw(fTimeDelta)
  └─ Render_UI(fTimeDelta)
      └─ for (pRenderObject : m_RenderObjects[RG_UI])
          └─ pRenderObject->Render(fTimeDelta)
              ├─ Bind_ShaderResources()           ← 셰이더 파라미터 바인딩
              │   ├─ CUIObject::Bind_ShaderResources()
              │   │   ├─ m_pTransformCom->Bind_ShaderResource("g_WorldMatrix")
              │   │   ├─ m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)
              │   │   └─ m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Ortho)
              │   └─ (서브클래스별)
              │       ├─ m_pTextureCom->Bind_ShaderResource("g_Texture", index)
              │       ├─ m_pShaderCom->Bind_RawValue("g_Radio", &ratio)
              │       └─ 기타 커스텀 파라미터
              ├─ m_pShaderCom->Begin(passIndex)   ← 셰이더 패스 선택
              ├─ m_pVIBufferCom->Bind_Buffers()   ← VB/IB 바인딩
              └─ m_pVIBufferCom->Render()          ← DrawIndexed
```

### 3.4 CUI_Manager 프레임 갱신

```
Level_GamePlay::Update(fTimeDelta)
  └─ CUI_Manager::GamePlayUpdate(fTimeDelta)
      ├─ 시작 타이머 체크 → UsingCreateStartUI() (0.5초 후)
      ├─ F2 → UsingCreateEndUI() (KO 연출)
      └─ TAB → CreateOption() / DestroyOption() (옵션 UI 토글)
```

---

## 4. 사용된 디자인 패턴

### 4.1 프로토타입 패턴 (Clone)

모든 UI 오브젝트는 `CGameObject::Clone()` 패턴을 따른다:

```cpp
// 프로토타입 등록 (Loader에서 1회)
m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
    TEXT("Prototype_GameObject_UI_HpGauge"), CUI_HpGauge::Create(pDevice, pContext));

// 복제 생성 (Level에서)
m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY,
    TEXT("Prototype_GameObject_UI_HpGauge"), TEXT("Layer_UI_HpGauge"), &desc);
// 내부: pPrototype->Clone(&desc) → new CUI_HpGauge(*this) → Initialize(&desc)
```

### 4.2 싱글톤 매니저

```
CUI_Manager       — UI 상태/플레이어 슬롯 중앙 관리
CBattleInterface_Manager — 전투 데이터(Ki/HP/Combo) 중앙 관리
```

UI 오브젝트는 생성자에서 `CUI_Manager::Get_Instance()`로 싱글톤을 참조하고 `Safe_AddRef`로 수명 관리.

### 4.3 좌/우 미러링 패턴

격투 게임 특유의 좌/우 대칭 UI를 **단일 클래스 + enum**으로 처리:

```cpp
// Initialize에서 LR에 따라 자동 미러링
switch (m_eLRPos) {
case LEFT:
    m_fPosX = 319;
    m_pMainPawn = m_pPawnArray[LPLAYER1];
    break;
case RIGHT:
    m_fPosX = (1280 - 319);  // X좌표 반전
    m_fSizeX *= -1;           // 텍스처 X 플립
    m_pMainPawn = m_pPawnArray[RPLAYER1];
    break;
}
```

**하나의 프로토타입으로 좌/우 인스턴스 2개를 생성**하며, `UI_DESC::eLRPos`로 구분.

### 4.4 화면 영역 자동 분류

UI 오브젝트가 `Set_UI_Setting()` 호출 시 Y좌표 기반으로 **Top/Mid/Bot 리스트에 자동 분류**:

```cpp
void CUIObject::UI_PosArea(_float fAreaPosY) {
    if (fAreaPosY >= 450)      Add_UIList(BOT);
    else if (fAreaPosY >= 160) Add_UIList(MID);
    else                       Add_UIList(TOP);
}
```

용도: 컷씬 등에서 특정 영역 UI만 숨기기 (`CutSceneUI(false)` → Top/Bot 숨김).

### 4.5 중간 추상 클래스 패턴

콤보/스킬 같은 복합 UI는 중간 추상 클래스를 둔다:

```
CUIObject (추상)
  └─ CUI_Combo (추상 — 콤보 상태, 색상 공통 로직)
      ├─ CUI_ComboNumber (각 자릿수 렌더)
      ├─ CUI_ComboFont ("COMBO" 텍스트)
      └─ CUI_ComboEffect (히트 이펙트)
```

공통 로직(콤보 카운트 갱신, 알파 페이드, 색상 결정)을 중간 클래스에 배치.

### 4.6 데이터 드리븐 셰이더 패스

각 UI 요소가 **셰이더 패스 인덱스**로 다른 렌더 효과를 선택:

```cpp
// CUI_HpGauge
(m_fHpRadio >= 1.f) ? m_iShaderID = 11 : m_iShaderID = 1;  // 풀HP=특수 이펙트
m_pShaderCom->Begin(m_iShaderID);

// CUI_ComboNumber
m_pShaderCom->Begin(4);   // 알파 컷 + 색상 변환 패스

// CUI_SkillGauge
m_pShaderCom->Begin(21);  // 게이지 반짝임 패스

// CUI_Timer
m_pShaderCom->Begin(0);   // 기본 텍스처 패스
```

하나의 `Shader_UI_VtxRect.hlsl`에 **수십 개 패스**를 정의하여, 각 UI가 패스 번호만 바꿔 다양한 효과를 구현.

---

## 5. DirectX API 호출 지점과 래핑 방식

### 5.1 직교 투영 행렬 설정

```cpp
// CUIObject::Set_UI_Setting()
XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());  // 뷰 = 항등행렬
XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(
    (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));  // 직교 투영
```

- 뷰 행렬은 항등행렬 → 카메라 무관
- 직교 투영: 화면 크기(1280×720)와 동일한 좌표계
- 깊이 범위 [0, 1] → `fDepth` 값으로 UI 간 Z-order 제어

### 5.2 좌표 변환 (스크린 → 직교)

```cpp
// Set_UI_Setting 내부
m_pTransformCom->Set_State(CTransform::STATE_POSITION,
    XMVectorSet(
        fPosX - g_iWinSizeX * 0.5f,   // 스크린 X → 직교 X (중앙=0)
        -fPosY + g_iWinSizeY * 0.5f,  // 스크린 Y → 직교 Y (위=+, 아래=-)
        fDepth,                         // Z-order
        1.f));
```

스크린 좌표(좌상단 원점) → 직교 좌표(중앙 원점)으로 변환.

### 5.3 셰이더 바인딩 체인

```
CUIObject::Bind_ShaderResources()
  ├─ CTransform::Bind_ShaderResource(shader, "g_WorldMatrix")
  │   → ID3DX11EffectMatrixVariable::SetMatrix()
  ├─ CShader::Bind_Matrix("g_ViewMatrix", &Identity)
  │   → ID3DX11EffectMatrixVariable::SetMatrix()
  └─ CShader::Bind_Matrix("g_ProjMatrix", &Ortho)
      → ID3DX11EffectMatrixVariable::SetMatrix()

(서브클래스)
  ├─ CTexture::Bind_ShaderResource(shader, "g_Texture", index)
  │   → ID3DX11EffectShaderResourceVariable::SetResource(SRV)
  └─ CShader::Bind_RawValue("g_Radio", &value, sizeof(float))
      → ID3DX11EffectVariable::SetRawValue()
```

### 5.4 렌더 호출

```
CShader::Begin(passIndex)
  → ID3DX11EffectPass::Apply()     ← 셰이더 상태 적용

CVIBuffer_Rect::Bind_Buffers()
  → IASetVertexBuffers()           ← 사각형 VB
  → IASetIndexBuffer()             ← 사각형 IB

CVIBuffer_Rect::Render()
  → DrawIndexed(6, 0, 0)           ← 2삼각형 = 1사각형
```

모든 UI 요소는 **동일한 사각형 쿼드(CVIBuffer_Rect)**를 공유하며, CTransform의 스케일/위치만 변경.

### 5.5 렌더 그룹 등록

```cpp
// CUI_HpGauge::Late_Update()
m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
```

`RG_UI`는 렌더 파이프라인에서 **디퍼드 합성 이후, 디스토션 이전**에 실행됨.
→ UI가 3D 씬 위에 오버레이되지만, 디스토션 후처리의 영향은 받지 않음.

---

## 6. 해상도 보정 시스템

### 기준 해상도와 오프셋

```cpp
// CUIObject::Initialize
m_vPrevWinSize = { 1280.f, 720.f };                    // 기준 해상도
m_vOffSetWinSize.x = g_iWinSizeX / m_vPrevWinSize.x;   // X 스케일 비율
m_vOffSetWinSize.y = g_iWinSizeY / m_vPrevWinSize.y;   // Y 스케일 비율

// Set_UI_Setting에서 적용
fSizeX *= m_vOffSetWinSize.x;
fSizeY *= m_vOffSetWinSize.y;
fPosX  *= m_vOffSetWinSize.x;
fPosY  *= m_vOffSetWinSize.y;
```

1280×720 기준으로 UI를 배치하고, 실제 해상도에 비례하여 크기/위치를 스케일링.

---

## 7. UI 애니메이션 시스템

### MoveAnimUI — 이동 보간

```cpp
_bool CUIObject::MoveAnimUI(_vector vTargetPos, _float fSpeed, _float fDepth, _float fTimeDelta, _float fEndDistance)
{
    vTargetPos = 스크린→직교 변환;
    _vector vMoveDir = Normalize(vTargetPos - currentPos);
    newPos = currentPos + vMoveDir * fSpeed * fTimeDelta;
    // 목표 거리 이내 도달 시 스냅
    if (Distance(target, current) <= fEndDistance) {
        Set_State(target); return TRUE;
    }
    return FALSE;
}
```

**등속 이동**(선형 보간) — 목표 지점에 `fEndDistance` 이내로 접근하면 스냅.

### Animation — 등장/퇴장

```cpp
_bool CUIObject::Animation(_vector vStartPos, _vector vTargetPos, _float fSpeed, ...)
{
    if (m_pUI_Manager->m_bChange[m_eLRPos]) {  // 캐릭터 교체 시 트리거
        if (!m_bStart) {
            Set_State(startPos);  // 시작 위치로 리셋
            m_bStart = TRUE;
        }
        bFinishAnim = MoveAnimUI(targetPos, speed, ...);
    }
}
```

캐릭터 태그 교체(`m_bChange`) 시 UI가 화면 밖에서 안으로 슬라이드 인하는 연출.

### 스케일 애니메이션 (콤보)

```cpp
// CUI_ComboNumber::ScaleAnimation()
if (m_iPrevCombo < m_iComboCount)
    m_bScaleAnim = TRUE;

if (m_bScaleAnim) {
    Set_UI_Setting(m_fSizeX * 1.25f, m_fSizeY * 1.25f, ...);  // 1.25배 확대
    m_bScaleAnim = FALSE;  // 1프레임만
} else {
    Set_UI_Setting(m_fSizeX, m_fSizeY, ...);  // 원래 크기
}
```

콤보 카운트 증가 시 **1프레임 동안 1.25배 확대**하는 팝 효과.

---

## 8. 멀티 텍스처 인덱싱

### 숫자 렌더링 (CUI_ComboNumber)

하나의 `CTexture` 컴포넌트에 **0~9 숫자 + 색상별 = 30장의 텍스처**를 로드:

```cpp
// 텍스처 인덱스 = 숫자값 + 색상오프셋 × 10
m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",
    Return_TextureIndex() + SetColor() * 10);

_uint Return_TextureIndex() {
    switch (m_iNumUI) {
    case FIRST:  return (m_iComboCount / 100);        // 백의 자리
    case SECOND: return ((m_iComboCount / 10) % 10);  // 십의 자리
    case THIRD:  return (m_iComboCount % 10);          // 일의 자리
    }
}

_uint SetColor() {
    if (combo >= 50)      return PURPLE;  // 2
    else if (combo >= 10) return BLUE;    // 1
    else                  return RED;     // 0
}
```

**인스턴스 3개**(백/십/일)가 각각 자릿수에 맞는 텍스처 인덱스를 계산.

### HP 게이지 마스크 텍스처

```cpp
// CUI_HpGauge::Bind_ShaderResources()
m_pTextureCom->Bind_ShaderResource(shader, "g_Texture", 0);     // 메인 HP 바
m_pMaskTexture->Bind_ShaderResource(shader, "g_MaskTexture", 0); // 마스크 (흐름 효과)

// 셰이더 파라미터
Bind_RawValue("g_Radio", &m_fHpRadio);       // HP 비율 0~1
Bind_RawValue("g_fRedRadio", &m_fRedHpRadio); // 감소 잔상 비율
Bind_RawValue("g_MaskTimer", &m_fMaskUVTimer); // 마스크 UV 스크롤
Bind_RawValue("g_DestroyTimer", &m_fRedGaugeTimer); // 잔상 페이드
Bind_RawValue("g_bState", &m_bRedAlpha);      // 잔상 활성 여부
Bind_RawValue("g_vColor", &vColor);           // 게이지 색상
```

셰이더에서 `g_Radio`로 클리핑하여 게이지를 표현하고, `g_fRedRadio`로 빨간 잔상을 보여줌.

---

## 9. 프레임워크 참고 설계 판단

### 9.1 채택할 만한 설계

**CGameObject 상속으로 UI 통합**
- UI가 일반 오브젝트와 동일한 Update/Render 루프에 참여
- 별도의 UI 전용 갱신 체계가 불필요하며, 레이어 기반 관리와 자연스럽게 통합
- 프로토타입 패턴으로 UI 복제 생성도 일관적

**좌/우 미러링 패턴**
- 격투 게임에서 필수인 대칭 UI를 `UI_LRPOS` enum 하나로 해결
- `RIGHT`일 때 X좌표 반전 + SizeX 부호 반전(텍스처 플립) — 심플하고 효과적

**단일 셰이더 + 멀티 패스**
- `Shader_UI_VtxRect.hlsl` 하나에 수십 개 패스를 정의
- 각 UI가 패스 번호만 지정하면 다양한 시각 효과 구현 가능
- 셰이더 교체 비용 없이 패스만 전환 → 성능 효율적

**해상도 보정 시스템**
- 기준 해상도(1280×720) 대비 비율로 위치/크기를 스케일링
- 간단하지만 실용적인 멀티 해상도 대응

**HP 잔상 게이지**
- `g_Radio`(현재HP) + `g_fRedRadio`(잔상HP) + 타이머 기반 페이드 — 격투 게임 감성의 핵심 UI
- 셰이더에서 이중 클리핑으로 구현하여 CPU 부하 거의 없음

**영역별 UI 분류 (Top/Mid/Bot)**
- 등록 시 Y좌표 기반 자동 분류 → 컷씬 시 Top/Bot만 숨김
- 매니저가 리스트로 관리하므로 일괄 조작이 간편

### 9.2 개선 여지가 있는 부분

**CUI_Manager의 public 멤버 과다**
- `m_pPawnArray`, `m_bChange`, `m_fTotalDuration`, `m_bActive` 등 대부분이 public
- UI 오브젝트들이 매니저의 멤버를 직접 읽고 쓰는 구조 → 캡슐화 부족
- getter/setter로 접근을 제한하면 의존성이 명확해짐

**CUI_Manager와 CBattleInterface_Manager의 역할 혼재**
- `CUI_Manager`가 전투 시작/종료 UI 생성, 캐릭터 교체까지 담당
- `CBattleInterface_Manager`는 이름은 "인터페이스"이지만 실제로는 전투 데이터 관리
- 역할이 모호하여, UI 쪽에서 전투 데이터에 접근할 때 두 매니저를 모두 참조해야 함

**InitPlayer() 매 프레임 호출**
- `CUIObject::Update()`에서 매 프레임 `InitPlayer()` 호출하여 캐릭터 참조를 갱신
- 캐릭터 교체는 드물게 발생하므로, 이벤트 기반 갱신이 더 효율적

**하드코딩된 좌표값**
- `m_fPosX = 319`, `m_fPosY = 67` 등 픽셀 좌표가 각 클래스에 하드코딩
- 레이아웃 변경 시 코드 수정 필요 → JSON/XML 기반 레이아웃 데이터로 분리하면 유연

**렌더 순서 Z-order 관리 부재**
- 깊이 값(`fDepth`)으로 Z-order를 제어하지만, `RG_UI` 그룹 내 정렬 로직은 없음
- `Render_UI()`가 등록 순서대로 렌더하므로, 등록 순서에 의존적

### 9.3 내 프레임워크(DX9) 적용 시 고려사항

| 참고프로젝트2 (DX11) | 내 프레임워크 (DX9) | 차이/적용 |
|----------------------|---------------------|-----------|
| Effects11 패스 기반 | D3DXEFFECT 패스 기반 | 거의 동일한 구조 적용 가능 |
| `XMMatrixOrthographicLH()` | `D3DXMatrixOrthoLH()` | 함수명만 다름, 개념 동일 |
| `CVIBuffer_Rect` (VB+IB) | `CVIBuffer` 기반 | 동일 패턴으로 UI 사각형 버퍼 생성 |
| `CTexture::Bind_ShaderResource(SRV)` | `SetTexture(stage, IDirect3DTexture9)` | DX9는 고정 텍스처 스테이지 |
| RG_UI 렌더 그룹 | RENDER_UI (enum 추가) | 기존 RENDER_PRIORITY~RENDER_UI에 추가 |
| Effects11 `SetRawValue` | `D3DXEffect::SetFloat/SetVector` | DX9는 타입별 함수 사용 |

**DX9 UI 렌더 설정 참고**:
```cpp
// DX9에서 직교 투영 UI 렌더 시 필요한 렌더 스테이트
m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);      // Z 테스트 끄기
m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);  // Z 쓰기 끄기
// 알파 블렌딩 활성화
m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
```

---

## 부록: UI 등록 전체 시퀀스 (게임플레이)

```
[로딩 중 — Loader 스레드]
CLoader::Loading_For_GamePlay()
  ├─ Add_Prototype("Prototype_GameObject_UI_HpGauge", CUI_HpGauge::Create())
  ├─ Add_Prototype("Prototype_GameObject_UI_Timer", CUI_Timer::Create())
  ├─ ... (총 32개 UI 프로토타입 등록)
  └─ Add_Prototype("Prototype_Component_Texture_UI_*", CTexture::Create())

[레벨 진입]
Level_GamePlay::Ready_Scene()
  ├─ CUI_Manager::InitUIObject()  ← 캐릭터 → 슬롯 매핑
  │
  └─ Ready_UI()
      ├─ for (LEFT, RIGHT):
      │   ├─ Clone("UI_HpPanel")       → Layer_UI_HpGauge
      │   ├─ Clone("UI_HpGauge")       → Layer_UI_HpGauge
      │   ├─ Clone("UI_SubHpGauge")    → Layer_UI_HpGauge
      │   ├─ Clone("UI_Chara_Icon")    → Layer_UI_Chara_Icon
      │   ├─ Clone("UI_SkillGauge")    → Layer_UI_SkillGauge
      │   ├─ Clone("UI_SkillGaugeBar") → Layer_UI_SkillGauge
      │   ├─ Clone("UI_ComboNumber") ×3 → Layer_UI_Combo_Number
      │   └─ Clone("UI_ComboFont")     → Layer_UI_Combo_Font
      │
      ├─ Clone("UI_TimerPanel")     → Layer_UI_Timer
      ├─ Clone("UI_Timer")          → Layer_UI_Timer
      └─ Clone("UI_Opt_Sound_*") ×8 → Layer_UI_Option_Sound

[매 프레임]
Update: UI 로직 (애니메이션, 상태 폴링)
Late_Update: RG_UI 렌더 그룹 등록
Render: 직교 투영 + 셰이더 패스로 사각형 쿼드 렌더
```

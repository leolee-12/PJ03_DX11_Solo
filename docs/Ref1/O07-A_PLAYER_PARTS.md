# O07-A: Player + Body + Weapon 파츠 협동

## 1. 전체 구조

```
CPlayer (CContainerObject)         ← 컨테이너 (입력 처리, 상태 관리)
  ├── CTransform                   ← 플레이어 월드 위치/회전
  ├── CNavigation                  ← 이동 제한
  ├── CCollider (OBB)              ← 플레이어 충돌체
  │
  ├── CBody (CPartObject)          ← 파츠 0: 캐릭터 몸체
  │     ├── CModel (Anim)          ← 스킨 메시 + 애니메이션
  │     ├── CShader (AnimMesh)     ← 스키닝 셰이더
  │     └── CCollider (Sphere)     ← 몸체 충돌체
  │
  └── CWeapon (CPartObject)        ← 파츠 1: 무기
        ├── CModel (NonAnim)       ← 정적 메시
        ├── CShader (StaticMesh)   ← 일반 셰이더
        └── CCollider (OBB)        ← 무기 충돌체
```

### 상속 계층

```
CGameObject
  ├── CContainerObject             ← 파츠 배열 소유
  │     └── CPlayer
  └── CPartObject                  ← 부모 행렬 포인터 소유
        ├── CBody
        └── CWeapon
```

---

## 2. CPlayer — 상태 머신 + 파츠 오케스트라

### 상태 비트 플래그

```cpp
enum STATE {
    IDLE   = 0x00000001,  // 0001
    RUN    = 0x00000002,  // 0010
    ATTACK = 0x00000004,  // 0100
};
_uint m_iState = {};
```

비트 플래그 방식으로 **복합 상태** 표현 가능 (`RUN | ATTACK`).

### Update — 입력 → 상태 전환

```cpp
void CPlayer::Update(_float fTimeDelta)
{
    if (VK_LEFT)  m_pTransformCom->Turn(Y축, -fTimeDelta);
    if (VK_RIGHT) m_pTransformCom->Turn(Y축, +fTimeDelta);
    if (VK_DOWN)  m_pTransformCom->Go_Backward(fTimeDelta);

    if (VK_UP) {
        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
        if (m_iState & IDLE) m_iState ^= IDLE;  // IDLE 해제
        m_iState |= RUN;                          // RUN 설정
    } else {
        m_iState = IDLE;
    }

    // 마우스 왼쪽 클릭 → 피킹 위치로 순간 이동
    if (VK_LBUTTON) {
        if (m_pGameInstance->Picking(&vPickPos))
            m_pTransformCom->Set_State(POSITION, vPickPos);
    }

    // 모든 파츠 Update 전파
    for (auto& pPartObj : m_PartObjects)
        pPartObj->Update(fTimeDelta);

    // 충돌체 갱신
    m_pColliderCom->Update(WorldMatrix);
}
```

**핵심**: Player는 **위치/상태만 관리**하고, 렌더링은 파츠에게 위임한다.
`Render()`가 비어 있음에 주목.

### 파츠 초기화

```cpp
HRESULT CPlayer::Ready_PartObjects()
{
    // Body: 부모 행렬 + 상태 포인터 전달
    CBody::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.pParentState = &m_iState;
    Add_PartObject(LEVEL::GAMEPLAY, TEXT("Prototype_Player_Body"), PARTOBJ::BODY, &BodyDesc);

    // Weapon: 부모 행렬 + 상태 + 소켓 본 행렬 전달
    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pParentState = &m_iState;
    WeaponDesc.pSocketBoneMatrix =
        dynamic_cast<CBody*>(m_PartObjects[BODY])->Get_SocketBoneMatrix_Ptr("SWORD");
    Add_PartObject(LEVEL::GAMEPLAY, TEXT("Prototype_Player_Weapon"), PARTOBJ::WEAPON, &WeaponDesc);
}
```

---

## 3. CBody — 애니메이션 몸체

### DESC 구조

```cpp
struct BODY_DESC : public PART_OBJECT_DESC {
    const _uint* pParentState;  // Player의 상태 포인터
};
// PART_OBJECT_DESC 자체에 pParentMatrix 포함
```

### Update — 상태→애니메이션 매핑

```cpp
void CBody::Update(_float fTimeDelta)
{
    // 상태에 따라 애니메이션 선택
    if (*m_pParentState & CPlayer::STATE::IDLE)
        m_pModelCom->Set_Animation(3, true);   // Idle 루프

    if (*m_pParentState & CPlayer::STATE::RUN)
        m_pModelCom->Set_Animation(4, true);   // Run 루프

    // 애니메이션 재생 (본 행렬 갱신)
    m_pModelCom->Play_Animation(fTimeDelta);

    // Combined 월드 행렬 = 로컬 × 부모
    m_CombinedWorldMatrix = m_pTransformCom->World × m_pParentMatrix;

    // 충돌체 갱신
    m_pColliderCom->Update(m_CombinedWorldMatrix);
}
```

### Render — 메시별 스키닝

```cpp
HRESULT CBody::Render()
{
    Bind_ShaderResources();  // WVP 행렬

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_ShaderResource(i, m_pShaderCom,
            "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices");

        m_pShaderCom->Begin(0);    // 패스 0: 디퍼드 스키닝
        m_pModelCom->Render(i);
    }
}
```

### 그림자 패스

```cpp
HRESULT CBody::Render_Shadow()
{
    // 라이트 공간 VP 행렬 바인딩
    m_pShaderCom->Bind_Matrix("g_ViewMatrix", Shadow_View);
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", Shadow_Proj);

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices");
        m_pShaderCom->Begin(1);    // 패스 1: 그림자 전용
        m_pModelCom->Render(i);
    }
}
```

### 소켓 본 행렬 제공

```cpp
const _float4x4* CBody::Get_SocketBoneMatrix_Ptr(const _char* pBoneName) const
{
    return m_pModelCom->Get_SocketBoneMatrix_Ptr(pBoneName);
}
```

**"SWORD" 본의 CombinedTransformationMatrix 포인터**를 반환.
이 포인터는 매 프레임 `Play_Animation()` 후 자동 갱신된다.

---

## 4. CWeapon — 소켓 본 부착

### DESC 구조

```cpp
struct WEAPON_DESC : public PART_OBJECT_DESC {
    const _float4x4* pSocketBoneMatrix;  // Body의 소켓 본 행렬 포인터
    const _uint* pParentState;
};
```

### Update — 소켓 월드 행렬 조합

```cpp
void CWeapon::Update(_float fTimeDelta)
{
    // ① 소켓 본 행렬 로드
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);

    // ② 스케일 제거 (정규화) — 본 행렬에 포함된 스케일 방지
    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    // ③ Combined = 무기 로컬 × 소켓 본 × 부모(Player) 월드
    m_CombinedWorldMatrix =
        m_pTransformCom->World × SocketMatrix × m_pParentMatrix;

    m_pColliderCom->Update(m_CombinedWorldMatrix);
}
```

### 행렬 조합 시각화

```
무기 로컬 행렬            소켓 본 행렬              부모 월드 행렬
(스케일 0.1, 90도 회전,  (본 Combined: 바인드포즈   (Player 위치/회전)
 오프셋 (0.8,0,0))       → 현재 포즈 변환)

   ×                        ×                        =
                                                   최종 무기 위치

무기가 캐릭터의 손에 달라붙어 애니메이션을 따라간다
```

### 스케일 정규화의 이유

```cpp
SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
```

본의 Combined 행렬에는 스케일이 포함될 수 있다.
무기 모델은 자체 스케일(`Set_Scale(0.1, 0.1, 0.1)`)을 가지므로,
소켓 본에서 스케일을 제거하여 **이중 스케일링을 방지**한다.

---

## 5. 프레임별 실행 순서

```
Player::Priority_Update
  → Body::Priority_Update
  → Weapon::Priority_Update

Player::Update
  ├── 입력 처리 → 상태 갱신
  ├── Body::Update
  │     ├── 상태→애니메이션 선택
  │     ├── Play_Animation → 본 행렬 갱신 (소켓 포함!)
  │     └── CombinedWorld = Local × Parent
  ├── Weapon::Update
  │     └── CombinedWorld = Local × SocketBone × Parent
  └── Player 충돌체 갱신

Player::Late_Update
  → Body::Late_Update → RenderGroup 등록 (NONBLEND + SHADOW)
  → Weapon::Late_Update → RenderGroup 등록 (NONBLEND)
  → Debug 컴포넌트 등록
```

**중요**: Body의 `Play_Animation()`이 Weapon의 `Update()` 보다 **먼저** 실행.
소켓 본 행렬이 갱신된 후에야 Weapon이 올바른 위치를 계산할 수 있다.

---

## 6. 핵심 정리

| 항목 | 설명 |
|------|------|
| **컨테이너 패턴** | Player = 입력/상태, 파츠 = 렌더링/충돌 |
| **상태 공유** | `const _uint*` 포인터로 파츠가 부모 상태 읽기 |
| **부모 행렬** | `const _float4x4*` 포인터로 파츠 월드 행렬 조합 |
| **소켓 본** | Body의 본 CombinedMatrix 포인터를 Weapon이 참조 |
| **스케일 정규화** | 소켓 행렬에서 스케일 제거 (이중 스케일 방지) |
| **월드 행렬** | Body: Local×Parent, Weapon: Local×Socket×Parent |
| **실행 순서** | Body(애니메이션) → Weapon(소켓 참조) 순서 필수 |
| **렌더 분리** | Body: Anim 셰이더(패스0+그림자패스1), Weapon: Static 셰이더 |

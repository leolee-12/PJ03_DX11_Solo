# O07-C: Monster + 충돌 판정 로직

## 1. CMonster 구조

```cpp
class CMonster final : public CGameObject {
    CCollider* m_pColliderCom[3];   // AABB, Sphere, OBB (3종 동시)
    CShader*   m_pShaderCom;        // AnimMesh 셰이더
    CModel*    m_pModelCom;         // Fiona 모델 (Anim)
};
```

Player의 컨테이너/파츠 패턴과 달리, Monster는 **단일 GameObject**로 모든 것을 처리한다.
3종 충돌체를 동시에 보유하여 디버그 시 비교 가능.

---

## 2. 초기화 — 랜덤 배치

```cpp
HRESULT CMonster::Initialize(void* pArg)
{
    __super::Initialize(pArg);
    Ready_Components();

    // 랜덤 위치 (XZ 평면 0~10, Y=1)
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_pGameInstance->Random(0.f, 10.f),
        1.0f,
        m_pGameInstance->Random(0.f, 10.f),
        1.f));

    m_pModelCom->Set_Animation(0, false);
}
```

### 3종 충돌체 설정

```cpp
// [0] AABB — 축 정렬 박스
CBounding_AABB::AABB_DESC AABBDesc{};
AABBDesc.vSize   = _float3(0.8f, 1.2f, 0.8f);
AABBDesc.vCenter = _float3(0.f, 0.6f, 0.f);  // 바닥에서 올림

// [1] Sphere — 구체 (머리 부근)
CBounding_Sphere::SPHERE_DESC SphereDesc{};
SphereDesc.fRadius = 0.2f;
SphereDesc.vCenter = _float3(0.f, 1.1f, 0.f);

// [2] OBB — 45도 회전된 방향 박스
CBounding_OBB::OBB_DESC OBBDesc{};
OBBDesc.vSize    = _float3(0.8f, 0.8f, 0.8f);
OBBDesc.vCenter  = _float3(0.f, 0.4f, 0.f);
OBBDesc.vRadians = _float3(0.f, XMConvertToRadians(45.0f), 0.f);
```

---

## 3. Update — 애니메이션 + 충돌

```cpp
void CMonster::Update(_float fTimeDelta)
{
    // 애니메이션 재생
    m_pModelCom->Set_Animation(0, true);
    m_pModelCom->Play_Animation(fTimeDelta);

    // 3개 충돌체 모두 월드 행렬로 갱신
    for (size_t i = 0; i < 3; i++)
        m_pColliderCom[i]->Update(WorldMatrix);

    // Player와 충돌 검사
    Intersect_ToPlayer();
}
```

---

## 4. Intersect_ToPlayer — 레이어 간 충돌

```cpp
_bool CMonster::Intersect_ToPlayer()
{
    // ① GameInstance를 통해 Player의 Collider 획득
    CCollider* pTargetCollider = dynamic_cast<CCollider*>(
        m_pGameInstance->Get_Component(
            ENUM_CLASS(LEVEL::GAMEPLAY),     // 레벨
            TEXT("Layer_Player"),             // 레이어 이름
            0,                               // 첫 번째 오브젝트
            TEXT("Com_Collider")));           // 컴포넌트 태그

    if (nullptr == pTargetCollider)
        return false;

    // ② OBB(인덱스 2)로 교차 검사
    return m_pColliderCom[2]->Intersect(pTargetCollider);
}
```

### 호출 체인

```
Monster::Intersect_ToPlayer()
  → GameInstance::Get_Component(GAMEPLAY, "Layer_Player", 0, "Com_Collider")
    → Object_Manager::Get_Component(...)
      → Layer::Get_Component(0, "Com_Collider")  // 0번째 오브젝트
        → GameObject::Get_Component("Com_Collider")
          → m_Components에서 태그로 검색
            → CCollider 포인터 반환

  → m_pColliderCom[2]->Intersect(pTargetCollider)
    → Monster.OBB vs Player.OBB 교차 검사
```

**핵심**: `Get_Component`로 다른 레이어의 오브젝트 컴포넌트에 접근.
하드코딩된 레이어/태그 이름에 의존한다.

---

## 5. Late_Update — 프러스텀 컬링

```cpp
void CMonster::Late_Update(_float fTimeDelta)
{
    // 월드 프러스텀 안에 있는 경우만 렌더 등록
    if (m_pGameInstance->isIn_Frustum_WorldSpace(
            m_pTransformCom->Get_State(STATE::POSITION), 1.f))
    {
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
        for (size_t i = 0; i < 3; i++)
            m_pGameInstance->Add_DebugComponent(m_pColliderCom[i]);
#endif
    }
}
```

**오브젝트 단위 프러스텀 컬링**: 바운딩 반지름 1.0으로 프러스텀 테스트.
화면 밖 몬스터는 렌더 큐에 등록하지 않는다.
Terrain의 QuadTree 컬링과 달리, 여기서는 **개별 오브젝트** 단위.

---

## 6. Render — 스키닝 메시

```cpp
HRESULT CMonster::Render()
{
    m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
    m_pShaderCom->Bind_Matrix("g_ViewMatrix", ViewMatrix);
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", ProjMatrix);

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_ShaderResource(i, m_pShaderCom,
            "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices");

        m_pShaderCom->Begin(0);  // 패스 0: 디퍼드 스키닝
        m_pModelCom->Render(i);
    }
}
```

Body와 동일한 스키닝 렌더 패턴. 단, Body처럼 그림자 패스는 없다.

---

## 7. Player vs Monster 구조 비교

| 항목 | CPlayer | CMonster |
|------|---------|---------|
| **기반 클래스** | CContainerObject | CGameObject |
| **파츠** | Body + Weapon | 없음 (단일) |
| **충돌체** | Player: OBB, Body: Sphere | AABB + Sphere + OBB (3종) |
| **렌더링** | 파츠에 위임 | 직접 수행 |
| **그림자** | Body에서 처리 | 없음 |
| **프러스텀 컬링** | 없음 (항상 렌더) | isIn_Frustum_WorldSpace |
| **Nav Mesh** | 사용 | 미사용 |

---

## 8. 핵심 정리

| 항목 | 설명 |
|------|------|
| **3종 충돌체** | AABB + Sphere + OBB 동시 보유 (디버그 비교) |
| **랜덤 배치** | 초기화 시 XZ (0~10) 랜덤 위치 |
| **레이어 간 충돌** | Get_Component로 Player 충돌체 획득 → OBB Intersect |
| **프러스텀 컬링** | Late_Update에서 화면 밖 오브젝트 렌더 제외 |
| **애니메이션** | Fiona 모델 공유, 인덱스 0 루프 재생 |
| **디버그 표시** | 3개 충돌체 모두 와이어프레임 표시 |

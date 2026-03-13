# O03-D: Light + Shadow + Frustum 구현

## 1. 조명 시스템 (CLight + CLight_Manager)

### LIGHT_DESC 구조체

```cpp
// Engine_Struct.h
struct LIGHT_DESC {
    LIGHT    eType;       // DIRECTIONAL 또는 POINT
    _float4  vDirection;  // 방향광 방향 (방향광 전용)
    _float4  vPosition;   // 점광원 위치 (점광원 전용)
    _float   fRange;      // 점광원 반경 (점광원 전용)
    _float4  vDiffuse;    // 확산 색상
    _float4  vAmbient;    // 환경 색상
    _float4  vSpecular;   // 반사 색상
};
```

### CLight — 개별 광원

```cpp
class CLight final : public CBase {
    LIGHT_DESC m_LightDesc;  // 광원 속성
};
```

CLight는 상태만 보유하는 단순한 데이터 객체이다.
핵심 로직은 `Render` 함수에서 셰이더에 데이터를 바인딩하고 패스를 실행하는 것이다.

### CLight::Render — 광원별 렌더링

```cpp
HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    _uint iPassIndex = {};

    if (LIGHT::DIRECTIONAL == m_LightDesc.eType)
    {
        // 방향광: 방향 벡터 바인딩, Pass 1
        pShader->Bind_RawValue("g_vLightDir",
            &m_LightDesc.vDirection, sizeof(_float4));
        iPassIndex = 1;
    }
    else
    {
        // 점광원: 위치 + 반경 바인딩, Pass 2
        pShader->Bind_RawValue("g_vLightPos",
            &m_LightDesc.vPosition, sizeof(_float4));
        pShader->Bind_RawValue("g_fLightRange",
            &m_LightDesc.fRange, sizeof(_float));
        iPassIndex = 2;
    }

    // 공통 속성 바인딩
    pShader->Bind_RawValue("g_vLightDiffuse",
        &m_LightDesc.vDiffuse, sizeof(_float4));
    pShader->Bind_RawValue("g_vLightAmbient",
        &m_LightDesc.vAmbient, sizeof(_float4));
    pShader->Bind_RawValue("g_vLightSpecular",
        &m_LightDesc.vSpecular, sizeof(_float4));

    // 풀스크린 쿼드 드로우 (디퍼드 라이팅)
    pShader->Begin(iPassIndex);
    pVIBuffer->Bind_Buffers();
    pVIBuffer->Render();

    return S_OK;
}
```

**핵심 포인트:**
- 방향광과 점광원이 **서로 다른 셰이더 패스**를 사용한다
- Pass 1 (방향광): 모든 픽셀에 균일한 방향으로 라이팅
- Pass 2 (점광원): 거리 감쇠(Attenuation)를 적용한 라이팅
- 각 광원마다 **풀스크린 쿼드를 한 번씩** 렌더한다

### CLight_Manager — 광원 관리

```cpp
class CLight_Manager : public CBase {
    list<CLight*> m_Lights;  // 광원 리스트

    HRESULT Add_Light(const LIGHT_DESC& LightDesc);
    HRESULT Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer);
};
```

```cpp
HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    for (auto& pLight : m_Lights)
        pLight->Render(pShader, pVIBuffer);  // 각 광원마다 쿼드 드로우
    return S_OK;
}
```

**라이팅 축적(Light Accumulation)**: MRT_LightAcc에 바인딩된 상태에서
각 광원이 순서대로 쿼드를 그린다. 셰이더의 블렌드 상태가 **Additive**로 설정되어 있어
여러 광원의 결과가 자동으로 합산된다:

```
Target_Shade    = Light1.Shade + Light2.Shade + Light3.Shade + ...
Target_Specular = Light1.Spec  + Light2.Spec  + Light3.Spec  + ...
```

### Level_GamePlay에서의 광원 설정

```cpp
HRESULT CLevel_GamePlay::Ready_Lights()
{
    LIGHT_DESC LightDesc{};

    // 방향광 (태양)
    LightDesc.eType     = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse  = _float4(0.6f, 0.6f, 0.6f, 1.f);
    LightDesc.vAmbient  = _float4(0.3f, 0.3f, 0.3f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    m_pGameInstance->Add_Light(LightDesc);

    // 점광원 1 (빨간색, 위치 (20, 5, 10), 반경 10)
    LightDesc.eType     = LIGHT::POINT;
    LightDesc.vPosition = _float4(20.0f, 5.0f, 10.f, 1.f);
    LightDesc.fRange    = 10.0f;
    LightDesc.vDiffuse  = _float4(1.f, 0.f, 0.f, 1.f);
    m_pGameInstance->Add_Light(LightDesc);

    // 점광원 2 (초록색, 위치 (30, 5, 10), 반경 10)
    LightDesc.eType     = LIGHT::POINT;
    LightDesc.vPosition = _float4(30.0f, 5.0f, 10.f, 1.f);
    LightDesc.vDiffuse  = _float4(0.f, 1.f, 0.f, 1.f);
    m_pGameInstance->Add_Light(LightDesc);
}
```

---

## 2. 그림자 시스템 (CShadow)

### CShadow — 라이트 공간 행렬 저장소

```cpp
class CShadow final : public CBase {
    _float4x4 m_TransformationMatrices[ENUM_CLASS(D3DTS::END)];
    // [VIEW] = 라이트 뷰 행렬
    // [PROJECTION] = 라이트 프로젝션 행렬
};
```

CShadow는 그림자 매핑을 위한 **라이트 공간의 뷰/프로젝션 행렬**을 저장한다.
PipeLine이 카메라의 행렬을 관리하듯, CShadow는 광원 시점의 행렬을 관리한다.

### Add_Shadow_Light — 라이트 뷰/프로젝션 생성

```cpp
HRESULT CShadow::Add_Shadow_Light(const SHADOW_DESC& ShadowDesc)
{
    _float3 vUpDir = { 0.f, 1.f, 0.f };

    // 라이트 뷰 행렬 (라이트 위치에서 대상을 바라봄)
    XMStoreFloat4x4(&m_TransformationMatrices[VIEW],
        XMMatrixLookAtLH(
            XMVectorSetW(XMLoadFloat3(&ShadowDesc.vEye), 1.f),
            XMVectorSetW(XMLoadFloat3(&ShadowDesc.vAt), 1.f),
            XMLoadFloat3(&vUpDir)));

    // 라이트 프로젝션 행렬 (원근 투영)
    XMStoreFloat4x4(&m_TransformationMatrices[PROJECTION],
        XMMatrixPerspectiveFovLH(
            ShadowDesc.fFovy, ShadowDesc.fAspect,
            ShadowDesc.fNear, ShadowDesc.fFar));
}
```

**SHADOW_DESC:**
```cpp
struct SHADOW_DESC {
    _float3 vEye;      // 광원 위치 (-10, 20, -10)
    _float3 vAt;       // 바라보는 지점 (0, 0, 0)
    _float  fFovy;     // FOV
    _float  fAspect;   // 종횡비
    _float  fNear;     // Near 평면
    _float  fFar;      // Far 평면
};
```

### 그림자 매핑 전체 흐름

```
[1] Shadow Pass (Render_Shadow):
    - 뷰포트를 8192×4608으로 변경
    - SHADOW 그룹 오브젝트들이 Render_Shadow() 호출
    - 셰이더에서 라이트의 View × Proj로 변환
    - Target_Shadow에 라이트 공간 깊이 저장

[2] Combined Pass (Render_Combined):
    - Target_Shadow를 셰이더 입력으로 바인딩
    - 카메라 공간 픽셀의 월드 좌표를 라이트 공간으로 변환
    - 라이트 공간 깊이와 Shadow 맵 값을 비교
    - 그림자 여부를 최종 색상에 반영
```

```
카메라 시점:                       라이트 시점:
  ┌─────────────────┐              ┌─────────────────┐
  │  ████  ←그림자  │              │      █████      │
  │  ████           │              │      █████      │
  │                 │              │ (깊이 기록)      │
  │        █████    │              │        ←차폐물   │
  │        █████    │              └─────────────────┘
  └─────────────────┘
   카메라가 보는 화면                Shadow Map (깊이)
```

---

## 3. 절두체 컬링 (CFrustum)

### 절두체 정의

절두체(Frustum)는 카메라가 볼 수 있는 **잘린 피라미드** 형태의 공간이다.
이 공간 밖의 오브젝트는 그리지 않음으로써 렌더링 비용을 줄인다.

### 8개 꼭짓점 (투영 공간의 NDC 큐브)

```cpp
HRESULT CFrustum::Initialize()
{
    // NDC 큐브의 8개 꼭짓점 (Near z=0, Far z=1, Left-Handed)
    m_vOriginalPoints[0] = _float3(-1.f,  1.f, 0.f);  // Near 좌상
    m_vOriginalPoints[1] = _float3( 1.f,  1.f, 0.f);  // Near 우상
    m_vOriginalPoints[2] = _float3( 1.f, -1.f, 0.f);  // Near 우하
    m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);  // Near 좌하
    m_vOriginalPoints[4] = _float3(-1.f,  1.f, 1.f);  // Far 좌상
    m_vOriginalPoints[5] = _float3( 1.f,  1.f, 1.f);  // Far 우상
    m_vOriginalPoints[6] = _float3( 1.f, -1.f, 1.f);  // Far 우하
    m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);  // Far 좌하
}
```

```
     4─────────5
    /│        /│       Far 평면 (z=1)
   / │       / │
  0─────────1  │
  │  7──────│──6       Near 평면 (z=0)
  │ /       │ /
  │/        │/
  3─────────2
```

### Update — NDC → 월드 변환

```cpp
void CFrustum::Update()
{
    _matrix ViewInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
    _matrix ProjInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJECTION);

    _float3 vPoints[8] = {};

    for (size_t i = 0; i < 8; i++)
    {
        // NDC → 뷰 공간 (프로젝션 역행렬)
        XMStoreFloat3(&vPoints[i],
            XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]),
                ProjInverse));
        // 뷰 공간 → 월드 공간 (뷰 역행렬)
        XMStoreFloat3(&vPoints[i],
            XMVector3TransformCoord(XMLoadFloat3(&vPoints[i]),
                ViewInverse));
    }

    // 8개 꼭짓점으로 6개 평면 생성
    Make_Planes(vPoints, m_vWorldPlanes);
    memcpy(m_vWorldPoints, vPoints, sizeof(_float3) * 8);
}
```

**변환 순서**: NDC → (ProjInverse) → 뷰 공간 → (ViewInverse) → 월드 공간

### Make_Planes — 6개 평면 생성

```cpp
void CFrustum::Make_Planes(const _float3* pPoints, _float4* pPlanes)
{
    // 우측 평면: 점 1, 5, 6
    XMStoreFloat4(&pPlanes[0],
        XMPlaneFromPoints(pts[1], pts[5], pts[6]));
    // 좌측 평면: 점 4, 0, 3
    XMStoreFloat4(&pPlanes[1],
        XMPlaneFromPoints(pts[4], pts[0], pts[3]));
    // 상단 평면: 점 4, 5, 1
    XMStoreFloat4(&pPlanes[2],
        XMPlaneFromPoints(pts[4], pts[5], pts[1]));
    // 하단 평면: 점 3, 2, 6
    XMStoreFloat4(&pPlanes[3],
        XMPlaneFromPoints(pts[3], pts[2], pts[6]));
    // 원거리 평면: 점 5, 4, 7
    XMStoreFloat4(&pPlanes[4],
        XMPlaneFromPoints(pts[5], pts[4], pts[7]));
    // 근거리 평면: 점 0, 1, 2
    XMStoreFloat4(&pPlanes[5],
        XMPlaneFromPoints(pts[0], pts[1], pts[2]));
}
```

`XMPlaneFromPoints`는 3개 점으로 평면 방정식 `Ax + By + Cz + D = 0`을 계산한다.
노멀 방향은 **절두체 내부를 향한다** (점 순서로 결정).

### 절두체 판정 — isIn_WorldSpace

```cpp
_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRadius)
{
    for (size_t i = 0; i < 6; i++)
    {
        // 점과 평면의 부호 거리 계산
        _float dist = XMVectorGetX(
            XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos));

        // 반경보다 멀리 벗어났으면 절두체 밖
        if (fRadius < dist)
            return false;
    }
    return true;  // 6개 평면 모두 통과 → 절두체 안
}
```

**fRadius**: 바운딩 스피어의 반경. 중심이 평면 밖이라도 반경만큼 여유를 두어
부분적으로 보이는 오브젝트도 포함한다.

### 로컬 공간 컬링 — Transform_ToLocalSpace

```cpp
void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
    _float3 vPoints[8] = {};

    for (size_t i = 0; i < 8; i++)
    {
        // 월드 공간 꼭짓점 → 로컬 공간으로 변환
        XMStoreFloat3(&vPoints[i],
            XMVector3TransformCoord(
                XMLoadFloat3(&m_vWorldPoints[i]),
                WorldMatrixInverse));
    }
    Make_Planes(vPoints, m_vLocalPlanes);
}
```

**로컬 컬링의 용도**: Terrain의 QuadTree처럼 오브젝트 로컬 공간에서
컬링을 수행해야 할 때 사용한다. 월드 → 로컬 변환 후 로컬 평면으로 판정.

### 갱신 타이밍

```
GameInstance::Update_Engine():
    ① Priority_Update (카메라가 View/Proj 설정)
    ② PipeLine::Update  (역행렬 계산)
    ③ Frustum::Update   (NDC → 월드 평면 생성)  ← 여기서 갱신
    ④ Update            (오브젝트에서 절두체 컬링 사용 가능)
```

카메라가 행렬을 설정하고, PipeLine이 역행렬을 계산한 후에
Frustum이 갱신되므로 최신 카메라 상태가 반영된다.

---

## 4. PipeLine 상세 (O02-C 보충)

PipeLine, Shadow, Frustum의 관계를 다시 정리하면:

| 컴포넌트 | 저장하는 행렬 | 갱신 시점 | 사용처 |
|----------|-------------|----------|--------|
| **PipeLine** | 카메라 View/Proj + 역행렬 | Priority_Update 후 | 모든 3D 렌더링 |
| **Shadow** | 라이트 View/Proj | 레벨 초기화 시 (1회) | 그림자 패스 |
| **Frustum** | 6개 월드/로컬 평면 | PipeLine::Update 후 | 오브젝트 컬링 |

```
Camera → PipeLine (View/Proj) → Frustum (6 Planes)
                                     ↓
Shadow (Light View/Proj) ────→ Shadow Pass
```

---

## 5. 핵심 정리

| 항목 | 설명 |
|------|------|
| **광원 타입** | DIRECTIONAL (Pass 1) + POINT (Pass 2), 셰이더 패스로 분기 |
| **라이팅 축적** | 광원마다 풀스크린 쿼드 드로우, Additive 블렌딩으로 합산 |
| **Shadow** | 라이트 뷰/프로젝션 행렬 저장, 8192×4608 깊이맵 |
| **절두체** | NDC 큐브 → 역행렬 → 월드 6평면, 반경 고려 판정 |
| **로컬 컬링** | 월드 꼭짓점 → 로컬 변환 → 로컬 평면 (QuadTree용) |
| **갱신 순서** | Camera → PipeLine → Frustum (의존성 순서 보장) |

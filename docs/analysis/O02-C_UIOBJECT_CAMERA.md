# O02-C: UIObject 직교 투영 + Camera 구현

## 1. DESC 상속 체인

이 프레임워크의 가장 독특한 설계 중 하나는 **구조체 상속을 통한 DESC 체인**이다.
생성 시 필요한 매개변수를 구조체 상속으로 계층화하여, 상위 클래스의 Initialize에서
자신이 필요한 부분만 캐스팅해서 읽는다.

### Camera 계열

```
TRANSFORM_DESC { fSpeedPerSec, fRotationPerSec }
  ← GAMEOBJECT_DESC : TRANSFORM_DESC { }         // 빈 확장 (향후 필드 추가용)
    ← CAMERA_DESC : GAMEOBJECT_DESC { vEye, vAt, fFovy, fNear, fFar }
      ← CAMERA_FREE_DESC : CAMERA_DESC { fSensor }
```

### UI 계열

```
TRANSFORM_DESC { fSpeedPerSec, fRotationPerSec }
  ← GAMEOBJECT_DESC : TRANSFORM_DESC { }
    ← UIOBJECT_DESC : GAMEOBJECT_DESC { fX, fY, fSizeX, fSizeY }
      ← BACKGROUND_DESC : UIOBJECT_DESC { }      // 빈 확장
```

### Initialize 체인에서의 활용

```cpp
// Camera_Free::Initialize (최하위)
CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
m_fSensor = pDesc->fSensor;        // 자기 고유 필드
__super::Initialize(pArg);          // 상위로 전달 → pArg 그대로 전달

// Camera::Initialize (중간)
CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
m_fFovy = pDesc->fFovy;            // 자기 고유 필드
__super::Initialize(pArg);          // 상위로 전달

// GameObject::Initialize (상위)
GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
// → 내부에서 CTransform 생성 시 pArg를 TRANSFORM_DESC*로 캐스팅
```

**핵심**: `void* pArg`를 통해 **하나의 포인터**로 모든 계층의 초기화 데이터를 전달한다.
구조체 상속 덕분에 메모리 레이아웃이 연속적이어서 어느 레벨에서든 캐스팅이 안전하다.

---

## 2. PipeLine — 행렬 저장소

### 역할

`CPipeLine`은 **뷰/프로젝션 행렬의 중앙 저장소**이다.
3D 렌더링에 필요한 행렬을 저장하고, 모든 오브젝트가 셰이더에 바인딩할 때 참조한다.

### 자료 구조

```cpp
class CPipeLine : public CBase {
    // D3DTS enum: VIEW, PROJECTION, END
    _float4x4  m_TransformationMatrix[ENUM_CLASS(D3DTS::END)];         // 원본 행렬
    _float4x4  m_TransformationMatrix_Inverse[ENUM_CLASS(D3DTS::END)]; // 역행렬
    _float4    m_vCamPosition;                                          // 카메라 월드 위치
};
```

### 갱신 타이밍

```
GameInstance::Update_Engine(fTimeDelta)
{
    m_pObject_Manager->Priority_Update();   // ① Camera가 여기서 행렬 설정
    m_pPipeLine->Update();                  // ② 역행렬 계산 + 카메라 위치 추출
    m_pObject_Manager->Update();            // ③ 일반 오브젝트들이 행렬 참조 가능
    m_pObject_Manager->Late_Update();       // ④ 렌더 큐 등록
}
```

**카메라가 Priority_Update에서 행렬을 설정**하고, PipeLine::Update에서 역행렬을 계산한 뒤,
이후 단계의 오브젝트들이 그 행렬을 사용할 수 있다.

### PipeLine::Update 구현

```cpp
void CPipeLine::Update()
{
    // 모든 행렬(View, Projection)의 역행렬 계산
    for (size_t i = 0; i < ENUM_CLASS(D3DTS::END); i++)
    {
        XMStoreFloat4x4(&m_TransformationMatrix_Inverse[i],
            XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrix[i])));
    }

    // 카메라 월드 위치 = 뷰 역행렬의 4번째 행 (= 카메라의 월드 변환)
    memcpy(&m_vCamPosition,
        &m_TransformationMatrix_Inverse[ENUM_CLASS(D3DTS::VIEW)].m[3],
        sizeof(_float4));
}
```

**왜 뷰 역행렬의 4행이 카메라 위치인가?**

뷰 행렬은 "월드 → 카메라 공간" 변환이다. 그 역행렬은 "카메라 공간 → 월드" 변환,
즉 카메라 자체의 월드 행렬이다. 4×4 행렬의 4번째 행(Row-Major)은 이동(Translation) 성분이므로,
이것이 곧 카메라의 월드 위치가 된다.

---

## 3. CCamera — 3D 카메라

### 클래스 구조

```cpp
class CCamera abstract : public CGameObject {
    _float  m_fFovy;     // Field of View (Y축 기준, 라디안)
    _float  m_fNear;     // Near 클리핑 평면
    _float  m_fFar;      // Far 클리핑 평면
    _float  m_fAspect;   // 종횡비 (Width / Height)
};
```

### Initialize 흐름

```cpp
HRESULT CCamera::Initialize(void* pArg)
{
    // ① 상위 클래스: CTransform 생성 (fSpeedPerSec, fRotationPerSec 적용)
    __super::Initialize(pArg);

    // ② 카메라 고유 파라미터 추출
    CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
    m_fFovy = pDesc->fFovy;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;

    // ③ 뷰포트에서 종횡비 계산
    D3D11_VIEWPORT ViewportDesc{};
    _uint iNumViewports = 1;
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
    m_fAspect = ViewportDesc.Width / ViewportDesc.Height;

    // ④ 카메라 초기 위치/방향 설정
    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
    m_pTransformCom->LookAt(
        XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));

    // ⑤ 파이프라인에 초기 행렬 전송
    Update_PipeLine();
    return S_OK;
}
```

### Update_PipeLine — 뷰/프로젝션 행렬 생성

```cpp
void CCamera::Update_PipeLine()
{
    // 뷰 행렬 = 카메라 월드 행렬의 역행렬
    m_pGameInstance->Set_Transform(D3DTS::VIEW,
        m_pTransformCom->Get_WorldMatrix_Inverse());

    // 원근 투영 행렬 (Left-Handed)
    m_pGameInstance->Set_Transform(D3DTS::PROJECTION,
        XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}
```

**뷰 행렬 생성 원리:**
- CTransform의 WorldMatrix는 카메라의 **"오브젝트로서의"** 월드 행렬이다
- 카메라의 뷰 행렬은 이 월드 행렬의 **역행렬**이다
- `D3DXMatrixLookAtLH`를 사용하지 않고 직접 역행렬을 계산하는 방식

**장점**: 카메라도 일반 GameObject처럼 Transform으로 위치/회전을 조작할 수 있다.
Go_Straight, Turn 등의 함수를 그대로 사용할 수 있어 코드 재사용성이 높다.

---

## 4. Camera_Free — 자유 카메라 구현

### DESC 확장

```cpp
struct CAMERA_FREE_DESC : public CAMERA_DESC {
    _float fSensor;  // 마우스 감도
};
```

### Priority_Update — 입력 처리

카메라는 **Priority_Update**에서 입력을 처리한다. 다른 오브젝트보다 먼저 실행되어
뷰 행렬이 갱신된 후 일반 오브젝트들이 사용할 수 있도록 한다.

```cpp
void CCamera_Free::Priority_Update(_float fTimeDelta)
{
    // WASD 이동
    if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
        m_pTransformCom->Go_Straight(fTimeDelta);
    if (GetKeyState('S') & 0x8000)
        m_pTransformCom->Go_Backward(fTimeDelta);
    if (GetKeyState('A') & 0x8000)
        m_pTransformCom->Go_Left(fTimeDelta);
    if (GetKeyState('D') & 0x8000)
        m_pTransformCom->Go_Right(fTimeDelta);

    // 마우스 회전 — X축 이동 → Y축(월드 업) 기준 회전
    _long MouseMove = {};
    if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f),
            MouseMove * m_fSensor * fTimeDelta);

    // 마우스 회전 — Y축 이동 → Right 벡터 기준 회전 (상하 회전)
    if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
        m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT),
            MouseMove * m_fSensor * fTimeDelta);

    // 행렬 갱신 → PipeLine에 전송
    Update_PipeLine();
}
```

**마우스 회전 분석:**

| 마우스 이동 | 회전 축 | 효과 |
|------------|---------|------|
| X (좌우) | 월드 Y축 `(0,1,0)` | 좌우 회전 (Yaw) |
| Y (상하) | 카메라 Right 벡터 | 상하 회전 (Pitch) |

상하 회전에 **카메라의 Right 벡터**를 사용하는 이유:
- 월드 X축 기준으로 회전하면 카메라가 기울어진 상태에서 부자연스럽게 동작
- 현재 카메라의 Right 벡터 기준으로 회전하면 FPS 카메라처럼 자연스러운 상하 회전

**주의**: `DIK_W`는 DirectInput, `GetKeyState('S')`는 Win32 API — 혼용되어 있다.
실제 프로젝트에서는 하나로 통일하는 것이 바람직하다.

### Transform::Turn 구현

```cpp
void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _vector vRight = Get_State(STATE::RIGHT);
    _vector vUp    = Get_State(STATE::UP);
    _vector vLook  = Get_State(STATE::LOOK);

    // 축 기준 회전 행렬 생성 (속도 × 시간 = 프레임 독립적)
    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis,
        m_fRotationPerSec * fTimeDelta);

    // 3개 축 벡터를 모두 회전 적용
    vRight = XMVector3TransformNormal(vRight, RotationMatrix);
    vUp    = XMVector3TransformNormal(vUp, RotationMatrix);
    vLook  = XMVector3TransformNormal(vLook, RotationMatrix);

    Set_State(STATE::RIGHT, vRight);
    Set_State(STATE::UP, vUp);
    Set_State(STATE::LOOK, vLook);
}
```

`XMVector3TransformNormal`은 4×4 행렬의 **3×3 회전 부분만** 적용한다 (이동 무시).
Right/Up/Look 3개 축을 모두 같은 행렬로 변환하므로 직교성이 유지된다.

---

## 5. CUIObject — 2D UI 시스템

### 3D와 완전히 분리된 행렬 체계

CUIObject는 PipeLine의 View/Projection 행렬을 사용하지 않는다.
대신 **자체 행렬**을 보유한다:

```cpp
class CUIObject : public CGameObject {
    _float4x4  m_ViewMatrix;   // 항등 행렬 (Identity)
    _float4x4  m_ProjMatrix;   // 직교 투영 행렬 (Orthographic)
    _float     m_fX, m_fY;             // 스크린 좌표 (좌상단 기준)
    _float     m_fSizeX, m_fSizeY;     // UI 크기 (픽셀)
    _float     m_fViewportSizeX, m_fViewportSizeY;  // 뷰포트 크기
};
```

### 초기화 — 직교 투영 설정

```cpp
HRESULT CUIObject::Initialize(void* pArg)
{
    __super::Initialize(pArg);  // CTransform 생성

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
    m_fX = pDesc->fX;
    m_fY = pDesc->fY;
    m_fSizeX = pDesc->fSizeX;
    m_fSizeY = pDesc->fSizeY;

    // 뷰포트 크기 획득
    D3D11_VIEWPORT ViewPortDesc{};
    _uint iNumViewports = 1;
    m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
    m_fViewportSizeX = ViewPortDesc.Width;
    m_fViewportSizeY = ViewPortDesc.Height;

    Update_State();  // 월드 행렬 설정

    // 뷰 행렬 = 항등 (카메라 변환 없음)
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    // 직교 투영: 뷰포트 크기만큼의 직교 공간
    XMStoreFloat4x4(&m_ProjMatrix,
        XMMatrixOrthographicLH(m_fViewportSizeX, m_fViewportSizeY, 0.f, 1.f));
}
```

### 좌표 변환 — Update_State

```cpp
void CUIObject::Update_State()
{
    // 크기 설정 (2D이므로 X, Y만 사용)
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY);

    // 스크린 좌표 → 직교 투영 좌표 변환
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_fX - m_fViewportSizeX * 0.5f,      // 좌상단 기준 → 중앙 기준
        -m_fY + m_fViewportSizeY * 0.5f,      // Y축 반전 (화면↓ → 수학↑)
        0.0f,                                  // Z = 0 (2D 평면)
        1.f
    ));
}
```

### 좌표 변환 상세 설명

화면 좌표계와 직교 투영 좌표계의 차이:

```
화면 좌표 (Screen Space)          직교 투영 좌표 (Ortho Space)
(0,0)────────────(W,0)            (-W/2, H/2)────(W/2, H/2)
  │                │                    │              │
  │   (fX, fY)    │                    │  (x', y')    │
  │      ●         │        →          │     ●         │
  │                │                    │              │
(0,H)────────────(W,H)            (-W/2,-H/2)────(W/2,-H/2)
  Y축↓                                 Y축↑
```

변환 공식:
```
x' = fX - W/2     (좌측 기준 → 중앙 기준)
y' = -fY + H/2    (상단 기준↓ → 중앙 기준↑, Y반전)
```

**예시** (뷰포트 1280×720):
- fX=150, fY=150 → x'=150-640=-490, y'=-150+360=210
- 화면 좌상단 근처에 위치

### 렌더링 파이프라인에서의 행렬 흐름

```
3D 오브젝트:  WorldMatrix × PipeLine::View × PipeLine::Projection
UI 오브젝트:  WorldMatrix × m_ViewMatrix(I) × m_ProjMatrix(Ortho)
```

3D 오브젝트는 PipeLine에서 카메라의 뷰/프로젝션 행렬을 가져오지만,
UI는 자체 행렬을 사용한다. 이 분리 덕분에 카메라가 어디를 보든 UI는 항상 같은 위치에 표시된다.

---

## 6. BackGround — UI 오브젝트 구현 예시

### 상속 체인

```
CBase → CGameObject → CUIObject → CBackGround
```

### Initialize 흐름

```cpp
HRESULT CBackGround::Initialize(void* pArg)
{
    // ① DESC 하드코딩 (프로토타입이므로 pArg 무시)
    BACKGROUND_DESC Desc{};
    Desc.fX = 150.0f;          // 화면 X 위치
    Desc.fY = 150.0f;          // 화면 Y 위치
    Desc.fSizeX = 300.0f;      // 너비 (픽셀)
    Desc.fSizeY = 300.0f;      // 높이 (픽셀)
    Desc.fSpeedPerSec = 5.f;   // Transform용 (UIObject에서는 미사용)

    // ② 상위 초기화 체인: UIObject → GameObject → Transform 생성
    __super::Initialize(&Desc);

    // ③ 컴포넌트 복제
    Ready_Components();
    return S_OK;
}
```

### 컴포넌트 구성

```cpp
HRESULT CBackGround::Ready_Components()
{
    // VIBuffer_Rect: STATIC 레벨 (모든 레벨에서 공유)
    Add_Component(ENUM_CLASS(LEVEL::STATIC),
        TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, nullptr);

    // Texture: LOGO 레벨 (로고 전용)
    Add_Component(ENUM_CLASS(LEVEL::LOGO),
        TEXT("Prototype_Component_Texture_BackGround"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom, nullptr);

    // Shader: STATIC 레벨 (모든 레벨에서 공유)
    Add_Component(ENUM_CLASS(LEVEL::STATIC),
        TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr);
}
```

**STATIC vs LOGO 차이**: VIBuffer_Rect와 Shader는 게임플레이에서도 사용되므로 STATIC,
BackGround 텍스처는 로고에서만 사용되므로 LOGO. 레벨 전환 시 LOGO 슬롯이 정리되면
텍스처 프로토타입은 해제되지만, Clone된 텍스처 컴포넌트는 오브젝트와 함께 해제된다.

### Render — 셰이더 바인딩

```cpp
HRESULT CBackGround::Render()
{
    // 월드 행렬 (Transform에서 직접 바인딩)
    m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

    // 뷰/프로젝션: PipeLine이 아닌 **자체 행렬** 사용!
    m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    // 텍스처 바인딩
    m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

    // 셰이더 패스 0 실행
    m_pShaderCom->Begin(0);

    // 버텍스/인덱스 버퍼 바인딩 + 드로우
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    return S_OK;
}
```

### RENDERGROUP::UI에 등록

```cpp
void CBackGround::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
    __super::Late_Update(fTimeDelta);  // → Update_State() 호출
}
```

UI 렌더 그룹은 **가장 마지막**에 그려져서 다른 모든 오브젝트 위에 표시된다.
깊이 테스트를 비활성화하여 Z 값에 관계없이 항상 앞에 그려진다.

---

## 7. 3D vs 2D 렌더링 비교 정리

| 항목 | 3D 오브젝트 | UI 오브젝트 |
|------|------------|------------|
| **뷰 행렬** | PipeLine (카메라 역행렬) | Identity (변환 없음) |
| **프로젝션** | Perspective (원근) | Orthographic (직교) |
| **좌표계** | 월드 공간 (미터 단위) | 스크린 공간 (픽셀 단위) |
| **깊이** | Z-Buffer 사용 | Z=0, 깊이 무시 |
| **렌더 그룹** | NONBLEND / BLEND 등 | UI (최후순위) |
| **카메라 영향** | 카메라 이동/회전에 따라 변화 | 영향 없음 (고정) |

---

## 8. Transform의 2D 오버로드

UIObject는 `Set_Scale`의 **2인자 오버로드**를 사용한다:

```cpp
// Transform.h
void Set_Scale(_float fSizeX = 1.f, _float fSizeY = 1.f, _float fSizeZ = 1.f);

// Transform.cpp
void CTransform::Set_Scale(_float fSizeX, _float fSizeY, _float fSizeZ)
{
    Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fSizeX);
    Set_State(STATE::UP,    XMVector3Normalize(Get_State(STATE::UP))    * fSizeY);
    Set_State(STATE::LOOK,  XMVector3Normalize(Get_State(STATE::LOOK))  * fSizeZ);
}
```

UIObject에서 `Set_Scale(m_fSizeX, m_fSizeY)` 호출 시 `fSizeZ = 1.f` (기본값).
Right/Up/Look 벡터의 길이를 설정하여 월드 행렬의 스케일을 조정한다.

VIBuffer_Rect는 `-0.5 ~ +0.5` 범위의 쿼드이므로, Scale로 픽셀 크기를 지정하면
직교 투영과 합쳐서 정확한 픽셀 크기로 렌더링된다.

---

## 9. 핵심 정리

| 항목 | 설명 |
|------|------|
| **DESC 상속 체인** | 구조체 상속으로 void* pArg 하나에 전체 초기화 데이터 전달 |
| **PipeLine** | View/Projection 행렬의 중앙 저장소, Priority_Update 후 갱신 |
| **Camera 뷰 행렬** | Transform 월드 행렬의 역행렬 (LookAt 대신 역행렬 방식) |
| **UI 독립 행렬** | Identity View + Orthographic Proj → 카메라 영향 없음 |
| **좌표 변환** | 스크린(좌상단↓) → 직교(중앙↑): x-W/2, -y+H/2 |
| **렌더 그룹 분리** | 3D는 NONBLEND/BLEND, UI는 UI 그룹에서 최후에 렌더 |

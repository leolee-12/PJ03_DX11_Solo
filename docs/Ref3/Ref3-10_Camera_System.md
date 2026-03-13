# 참고프로젝트3 — 카메라 시스템 심화 분석

> **분석 대상**: CCamera(엔진), CCamera_Manager + CStaticCamera, CDynamicCamera, CActionCamera, CRecordCamera, CTowerCamera, CSummonCamera, CStandCamera, CFloatCamera, CGoblin_Camera, CVictory_Camera(클라이언트) + CDataManager(카메라 행렬 공유)
> **모드**: [분석]

---

## 1. 핵심 책임과 시스템 경계

### 이 시스템이 담당하는 것
- **View/Projection 행렬 계산**: CCamera가 Eye/At/Up → `D3DXMatrixLookAtLH` → DX 디바이스에 세팅
- **카메라 전환 관리**: Camera_Manager가 10종 카메라를 배열로 보유 → `Change_Camera()`로 전환
- **부드러운 전환**: `Flow_Camera()` — 목표 위치/방향으로 보간 이동
- **카메라 셰이크**: StaticCamera/Goblin_Camera — sin 기반 흔들림
- **컷씬 재생**: ActionCamera — 파일에서 Eye/At 읽어 재생, RecordCamera — 파일에 기록
- **프러스텀 갱신**: Camera_Manager가 매 프레임 View/Proj → FrustumManager에 전달

### 시스템 경계 — 이것은 하지 않는다
- **렌더링**: Camera는 행렬만 제공, Renderer가 실제 렌더
- **플레이어 조작**: 입력은 받지만 이동은 플레이어 오브젝트가 처리
- **충돌/클리핑**: 카메라-지형 충돌은 별도 처리 없음

---

## 2. 클래스 간 소유/참조 관계

```
CBase
├── CGameObject
│   └── CCamera (엔진 — 카메라 베이스) ★
│       │   소유: m_pGraphicDev (AddRef)
│       │   참조: m_pDataMgr → CDataManager* (AddRef)
│       │   값: m_vEye, m_vAt, m_vUp, m_vDir (위치/방향)
│       │   값: m_matView, m_matProj (행렬)
│       │   값: m_fFovy, m_fAspect, m_fNear, m_fFar (투영 파라미터)
│       │   값: m_vDestEye, m_fDistance, m_vDestDir, m_fAngle (Flow 전환용)
│       │
│       ├── CStaticCamera (3인칭 추적 — 메인 게임 카메라)
│       │   ├── 소유: m_pPlayerObserver (Create + Subscribe)
│       │   ├── 참조: m_pInputDev (AddRef)
│       │   ├── 참조: m_pPlayerMatrix → 플레이어 월드 행렬 포인터
│       │   └── 값: m_fCameraAngle, m_fPlayerRatio[3], 셰이크 파라미터
│       │
│       ├── CDynamicCamera (자유 비행 — 디버그용)
│       │   ├── 참조: m_pInputDev (AddRef)
│       │   └── 값: m_fCamSpeed
│       │
│       ├── CActionCamera (컷씬 재생)
│       │   ├── 값: m_hFile (HANDLE — 카메라 데이터 파일)
│       │   ├── 값: m_eCutSceneType, m_eNextCamType
│       │   └── 옵저버 데이터: m_bShowCutScene, m_eSceneId (InfoSubject에 등록)
│       │
│       ├── CRecordCamera (카메라 경로 녹화 도구)
│       │   ├── 참조: m_pInputDev (AddRef)
│       │   ├── 소유: m_pRendererCom (Clone, UI 렌더용)
│       │   ├── 소유: m_pD3DXFont (ID3DXFont)
│       │   └── 값: m_hFile, m_bRecording, m_fRecordTime
│       │
│       ├── CTowerCamera / CSummonCamera (건설/소환 특수 시점)
│       │   └── 참조: m_pPlayerObserver (플레이어 위치 추적)
│       │
│       ├── CStandCamera / CFloatCamera (대기/부유 카메라)
│       │
│       └── CGoblin_Camera / CVictory_Camera (PvP 모드 카메라)
│           └── 참조: m_pLastManObserver (PvP 플레이어 추적)
│
└── CCamera_Manager (싱글톤 — 카메라 전환 관리) ★
    ├── 소유: m_ArrCamera[CAMERA_END] → CCamera* 배열 (각 카메라 소유)
    ├── 참조: m_pInputDev (AddRef)
    ├── 참조: m_pFrustumMgr (GetInstance)
    └── 값: m_eCamType (현재 활성 카메라 인덱스)
```

### 소유 원칙

| 관계 | 방식 |
|------|------|
| Manager → Camera 배열 | **소유** — `Add_Camera()`로 등록, `Free()`에서 전체 `Safe_Release` |
| Camera → DX 디바이스 | `AddRef()` 소유 |
| Camera → DataManager | `Add_Ref()` → 행렬 공유 허브 |
| StaticCamera → PlayerObserver | **소유** — Create + Subscribe, Free에서 Unsubscribe + Release |
| ActionCamera → 파일 핸들 | **소유** — CreateFile/CloseHandle |

---

## 3. 한 프레임 호출 흐름

### 3-1. 초기화 (씬 시작)

```
Scene::Ready_Scene()
  ├─ CStaticCamera::Create(pDev)
  │   └─ Ready_GameObject()
  │       ├─ PlayerObserver 생성 + InfoSubject에 Subscribe
  │       ├─ Eye/At/Up 초기값, Fovy/Aspect/Near/Far 설정
  │       └─ CCamera::Ready_GameObject()
  │           ├─ D3DXMatrixLookAtLH → m_matView
  │           ├─ SetTransform(D3DTS_VIEW)
  │           ├─ D3DXMatrixPerspectiveFovLH → m_matProj
  │           ├─ SetTransform(D3DTS_PROJECTION)
  │           └─ DataManager에 View/Proj/ViewInv/CamPos 세팅
  │
  ├─ CDynamicCamera::Create(pDev)
  ├─ CActionCamera::Create(pDev)
  ├─ CRecordCamera::Create(pDev)
  ├─ ...기타 카메라 생성...
  │
  └─ Camera_Manager::Add_Camera(pCamera, CAMERA_STATIC)  × 10종
```

### 3-2. 매 프레임 (Update)

```
MainApp::Update()
  └─ Camera_Manager::Update_Camara(fTimeDelta)
      │
      ├─ KeyCheck()  ← 디버그 카메라 전환 키 (주석 처리됨)
      │
      ├─ m_ArrCamera[m_eCamType]->Update_GameObject(fTimeDelta)
      │   │
      │   ├─ [StaticCamera 예시]
      │   │   ├─ PlayerObserver에서 플레이어 정보 Pull
      │   │   ├─ KeyCheck() → Q키로 마우스 고정 토글
      │   │   │
      │   │   ├─ [Flow 전환 중] (m_bCamAttach == false)
      │   │   │   └─ CCamera::Flow_Camera(fTimeDelta, destEye, dist, destDir, angle, axis, time)
      │   │   │       ├─ m_vEye += dir × fTimeDelta × fDistance × fTime
      │   │   │       └─ D3DXMatrixRotationAxis → m_vDir 회전
      │   │   │
      │   │   ├─ [일반 모드] (m_bCamAttach == true)
      │   │   │   └─ TargetRenewal(fTimeDelta)
      │   │   │       ├─ 플레이어 월드 행렬에서 Right/Look/Pos 추출
      │   │   │       ├─ m_fPlayerRatio[3] 기반 오프셋 계산
      │   │   │       ├─ 카메라 앵글 기반 X축 회전
      │   │   │       └─ m_vEye = 계산된 위치, m_vAt = Eye + Look
      │   │   │
      │   │   └─ ShakeCamera(fTimeDelta)
      │   │       └─ sin(angle) 기반 Right 방향 흔들림
      │   │
      │   └─ CCamera::Update_GameObject(fTimeDelta)  ← 베이스 호출
      │       ├─ D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp)
      │       ├─ SetTransform(D3DTS_VIEW, &m_matView)
      │       ├─ D3DXMatrixInverse → ViewInverse
      │       └─ DataManager에 View/ViewInv/CamPos 갱신
      │
      └─ Update_Frustum()
          ├─ View/Proj 행렬 획득
          └─ FrustumManager::MakeFrustumPlane(matView, matProj)
```

### 3-3. 카메라 전환

```
[트리거] WaveObserver → Camera_Manager::Change_Camera(CAMERA_ACTION, CUTSCENE_BOSS)

Camera_Manager::Change_Camera(eCamType, uCutSceneNum, fAttachTime)
  ├─ if (m_eCamType == eCamType && !bForceChange) return  ← 중복 방지
  ├─ m_eCamType = eCamType  ← 활성 카메라 교체
  └─ m_ArrCamera[eCamType]->Change_Camera(uCutSceneNum, fAttachTime)
      │
      ├─ [StaticCamera::Change_Camera]
      │   ├─ Initialize_Camera()  ← 현재 View 역변환으로 Eye/Dir 초기화
      │   ├─ m_bCamAttach = false  ← Flow 모드 진입
      │   ├─ 플레이어 기준 목표 위치/방향 계산 (m_vDestEye, m_vDestDir)
      │   ├─ m_fDistance = |destEye - currentEye|
      │   ├─ m_fAngle = acos(dot(destDir, currentDir))
      │   └─ m_vAxis = cross(currentDir, destDir)  ← 회전 축
      │
      └─ [ActionCamera::Change_Camera]
          ├─ m_eCutSceneType = uNum
          └─ Set_CutScene()
              ├─ m_bShowCutScene = true
              ├─ InfoSubject::Notify(MESSACE_CUTSCENE)
              └─ CreateFile("../CamData/Boss.dat") → m_hFile
```

---

## 4. 카메라 종류별 특성

| 카메라 | 용도 | 입력 | 추적 대상 | 특수 기능 |
|--------|------|------|----------|----------|
| **Static** | 메인 게임 | 마우스+Q키 | 플레이어 (옵저버) | 셰이크, Flow 전환, 앵글 추적 |
| **Dynamic** | 디버그 자유 비행 | WASD+마우스 | 없음 | View 역변환으로 Right 추출 |
| **Action** | 컷씬 재생 | Space(스킵) | 파일 데이터 | .dat에서 Eye/At 읽기, 종료 시 다음 카메라 전환 |
| **Record** | 컷씬 녹화 | WASD+F2/F3/F4 | 없음 | .dat에 Eye/At 기록, 폰트 렌더링 |
| **Tower** | 타워 건설 | 마우스 | 플레이어 (옵저버) | 마우스 커서 제한 |
| **Summon** | 소환 연출 | - | 플레이어 (옵저버) | - |
| **Stand** | 메뉴 대기 | 좌우 키 | 고정 위치 | - |
| **Float** | 부유 시점 | - | 고정 위치 | - |
| **Goblin** | PvP 메인 | 마우스 | LastMan 플레이어 | 셰이크 |
| **Victory** | 승리 연출 | - | LastMan 플레이어 | - |

---

## 5. 사용된 디자인 패턴

### 5-1. 전략 패턴 (Strategy)
```cpp
// Camera_Manager가 활성 카메라 "전략"을 교체
m_ArrCamera[m_eCamType]->Update_GameObject(fTimeDelta);  // 다형성
m_ArrCamera[m_eCamType]->Change_Camera(uNum, fTime);     // 다형성
```
- CCamera 배열에 10종 구현체 → 인덱스만 바꾸면 카메라 행동 교체
- 각 카메라가 독립적으로 Update/Change 구현

### 5-2. 싱글톤 (Camera_Manager)
- `DECLARE_SINGLETON` / `IMPLEMENT_SINGLETON`
- 어디서든 `CCamera_Manager::GetInstance()->Change_Camera(...)` 호출 가능

### 5-3. 옵저버 패턴 (StaticCamera ↔ InfoSubject)
- StaticCamera가 `CPlayerObserver` 소유 + `InfoSubject`에 Subscribe
- 플레이어 정보(위치, 앵글)를 Push 통보 → Pull 방식으로 획득
- ActionCamera도 `m_bShowCutScene`을 InfoSubject에 등록 → 컷씬 상태 공유

### 5-4. 팩토리 메서드 (Create 정적 함수)
- 각 카메라가 `static Create(pDev)` → `new` + `Ready_GameObject`

### 5-5. 녹화-재생 패턴 (Record/Action)
```
[녹화] RecordCamera → WriteFile(m_hFile, &CAM_INFO{Eye, At}, sizeof)
[재생] ActionCamera → ReadFile(m_hFile, &CAM_INFO, sizeof) → m_vEye/m_vAt에 적용
```
- 같은 `CAM_INFO` 구조체(`{_vec3 vecEye, vecAt}`)로 직렬화/역직렬화
- 프레임 단위 기록 → 재생 시 프레임 단위 읽기 (고정 프레임레이트 전제)

---

## 6. DirectX API 호출 지점과 래핑 방식

### CCamera (베이스 — 모든 카메라 공통)

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `D3DXMatrixLookAtLH` | Ready / Update | Eye/At/Up → View 행렬 |
| `SetTransform(D3DTS_VIEW)` | Ready / Update | 디바이스에 View 세팅 |
| `D3DXMatrixPerspectiveFovLH` | Ready | Fovy/Aspect/Near/Far → Proj 행렬 |
| `SetTransform(D3DTS_PROJECTION)` | Ready | 디바이스에 Proj 세팅 |
| `D3DXMatrixInverse` | Ready / Update | View → ViewInverse (카메라 월드 위치) |

### StaticCamera

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `D3DXMatrixTranslation` | TargetRenewal / Change_Camera | 오프셋 행렬 생성 |
| `D3DXMatrixRotationAxis` | TargetRenewal / Change_Camera | 카메라 앵글 회전 |
| `D3DXVec3TransformCoord/Normal` | TargetRenewal | 행렬 적용하여 Eye 계산 |
| `D3DXVec3Dot/Cross/Length` | Change_Camera | Flow 전환 파라미터 계산 |

### DynamicCamera

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `D3DXMatrixInverse(matView)` | KeyCheck | View 역변환 → Right 벡터 추출 |
| `D3DXMatrixRotationAxis` | MouseMove | 마우스 이동 → 시점 회전 |
| `D3DXVec3TransformNormal` | MouseMove | Dir 벡터 회전 |

### RecordCamera

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `D3DXCreateFontIndirect` | Ready | 녹화 상태 표시용 폰트 생성 |
| `DrawTextW` | Render | 녹화 상태/시간 텍스트 렌더링 |
| `D3DXMatrixRotationX/Y` | MouseMove | 누적 앵글 기반 Dir 계산 |

### CCamera::Flow_Camera (부드러운 전환)

| DX9 API | 위치 | 용도 |
|---------|------|------|
| `D3DXMatrixRotationAxis` | Flow_Camera | 축 기반 회전 → Dir 보간 |
| `D3DXVec3TransformNormal` | Flow_Camera | Dir 벡터에 회전 적용 |
| `D3DXVec3Normalize` | Flow_Camera | 이동 방향 정규화 |

---

## 7. 설계 판단 분석 — 참고할 점과 한계

### 참고할 점

**1) 배열 기반 카메라 관리 — 즉시 전환**
```cpp
CCamera* m_ArrCamera[CAMERA_END];  // enum 크기 배열
m_eCamType = eCamType;             // 인덱스만 교체
m_ArrCamera[m_eCamType]->Update_GameObject(fTimeDelta);
```
- 모든 카메라가 씬 시작 시 생성되어 배열에 상주
- 전환 시 `new`/`delete` 없이 인덱스만 변경 → O(1) 전환
- 비활성 카메라는 Update 호출되지 않음 (메모리만 점유)

**2) Flow_Camera — 범용 부드러운 전환**
```cpp
void Flow_Camera(fTimeDelta, vecDest, fDistance, vecDir, fAngle, vecAxis, fTime)
{
    if (m_bCamAttach) { /* 즉시 적용 */ }
    else {
        m_vEye += dir × fTimeDelta × fDistance × fTime;     // 위치 보간
        D3DXMatrixRotationAxis(&matRot, &vecAxis, fAngle × fTimeDelta × fTime);
        D3DXVec3TransformNormal(&m_vDir, &m_vDir, &matRot); // 방향 보간
        // fTime초 후 m_bCamAttach = true → 전환 완료
    }
}
```
- 어떤 카메라에서 어떤 카메라로든 부드러운 전환 가능
- `Initialize_Camera()` → 현재 View 역변환으로 Eye/Dir 복원 → 목표까지 보간

**3) DataManager를 통한 행렬 공유**
```cpp
// Camera::Update에서
m_pDataMgr->Set_ViewMatrix(m_matView);
m_pDataMgr->Set_CamPoistion(m_vEye);

// 셰이더/다른 시스템에서
matView = CDataManager::GetInstance()->Get_ViewMatrix();
```
- 카메라가 행렬을 DataManager에 Push → 다른 시스템이 Pull
- 카메라 ↔ 렌더링 시스템 간 직접 의존 없음

**4) Record/Action 녹화-재생 파이프라인**
- RecordCamera: WASD+마우스로 자유 비행 → F2 녹화시작 → 매 프레임 `CAM_INFO{Eye,At}` 기록 → F4 종료
- ActionCamera: .dat 파일 → 매 프레임 `ReadFile` → Eye/At 적용 → EOF 시 다음 카메라로 전환
- **같은 구조체(CAM_INFO)로 직렬화** → 호환성 보장, 도구와 런타임 분리

**5) 카메라 셰이크 — sin 기반**
```cpp
float fAngle = m_fTime * 180.f / m_fShakeTime * m_iShakeCount;
float fOffset = abs(m_fShakePower * sin(D3DXToRadian(fAngle)));
m_vEye += Player_Right * fOffset;
```
- 시간 경과에 따라 sin 진동 → iShakeCount로 진동 횟수, fShakePower로 강도
- 플레이어의 Right 방향으로 흔들림 → 화면 수평 방향 셰이크

**6) Change_Camera 가상 함수 — 전환 시 이벤트**
```cpp
virtual void Change_Camera(_uint uNum, float fAttachTime) PURE;
```
- 전환 "되었을 때" 각 카메라가 수행할 로직을 다형성으로 처리
- StaticCamera: Flow 전환 준비, ActionCamera: 컷씬 파일 열기
- 전환 "트리거"는 Manager, 전환 "반응"은 각 카메라

### 한계/개선 가능 포인트

**1) 프레임 종속 컷씬 재생**
```cpp
// ActionCamera: 매 프레임 ReadFile
ReadFile(m_hFile, pCamInfo, sizeof(CAM_INFO), &dwByte, NULL);
m_vEye = pCamInfo->vecEye;
```
- 프레임당 1개의 CAM_INFO 읽기 → 프레임 레이트가 달라지면 재생 속도 변화
- 개선: 타임스탬프 기반 보간 (각 CAM_INFO에 시간 정보 추가)

**2) Flow_Camera의 선형 보간 한계**
```cpp
m_vEye += dir × fTimeDelta × fDistance × fTime;
```
- 단순 선형 이동 → 시작/끝이 급격함 (ease-in/out 없음)
- 개선: 3차 보간(cubic), 또는 `D3DXVec3Hermite`/`Catmull-Rom` 사용

**3) Camera_Manager의 dynamic_cast 사용**
```cpp
void Shake_Camera(...) {
    if (CAMERA_STATIC == m_eCamType)
        dynamic_cast<CStaticCamera*>(m_ArrCamera[m_eCamType])->Set_CameraShake(...);
    else if (CAMERA_GOBLIN == m_eCamType)
        dynamic_cast<CGoblin_Camera*>(m_ArrCamera[m_eCamType])->Set_CameraShake(...);
}
```
- 카메라 타입별 `dynamic_cast` → 다형성 원칙 위반, 새 카메라 추가 시 분기 증가
- 개선: `Set_CameraShake`를 CCamera 가상 함수로 승격

**4) m_pPlayerMatrix 원시 포인터 참조**
```cpp
m_pPlayerMatrix = pInfo->pTransform->Get_WorldMatrix();
```
- 플레이어가 소멸하면 댕글링 포인터
- 이 프로젝트에서는 플레이어가 카메라보다 오래 살기에 문제없지만, 일반화 시 위험

**5) Projection 행렬이 Ready에서만 설정**
```cpp
// Ready_GameObject에서 한 번만
D3DXMatrixPerspectiveFovLH(&m_matProj, ...);
m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
```
- Update에서는 View만 갱신, Proj는 그대로
- FOV 변화(줌/런 등) 시 별도 처리 필요 → 현재는 미지원

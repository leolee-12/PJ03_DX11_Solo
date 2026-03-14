# O01. 프로젝트 구조 & 아키텍처 총괄

## 1. 솔루션 구성

```
3D최종버전(참고)/
├── Framework.sln          ← VS 솔루션 (Engine + Client)
├── UpdateLib.bat           ← 빌드 후 SDK/Bin 자동 복사
├── Engine/                 ← 엔진 라이브러리 (DLL)
│   ├── public/             ← 공개 헤더 (55개)
│   ├── private/            ← 구현 cpp (54개)
│   ├── ThirdPartyLib/      ← 서드파티 .lib
│   └── default/            ← vcxproj
├── EngineSDK/              ← 빌드 산출물 배포용
│   ├── Inc/                ← public/ 헤더 복사본
│   └── Lib/                ← Engine.lib + 서드파티 lib
└── Client/                 ← 게임 클라이언트 (EXE)
    ├── Default/            ← 진입점(wWinMain), 리소스
    ├── Public/             ← 클라이언트 헤더 (18개)
    └── Private/            ← 클라이언트 구현 (17개)
```

### 빌드 파이프라인
1. **Engine** → `Engine.dll` + `Engine.lib` 생성 (DLL Export)
2. **UpdateLib.bat** → 헤더를 EngineSDK/Inc, lib를 EngineSDK/Lib, 셰이더를 Client/Bin에 복사
3. **Client** → EngineSDK의 헤더/lib를 링크하여 EXE 생성

> **현재 프로젝트(Framework-WY)와의 차이**: 현재 프로젝트는 Engine이 **정적 라이브러리(.lib)**이지만, 참고 프로젝트는 **DLL** 방식. `ENGINE_DLL` 매크로(`__declspec(dllexport/dllimport)`)로 심볼 노출 제어.

---

## 2. DX9 → DX11 핵심 변경점

| 항목 | 현재 (DX9) | 참고 (DX11) |
|------|-----------|------------|
| **디바이스** | `IDirect3DDevice9` 단일 | `ID3D11Device` + `ID3D11DeviceContext` 분리 |
| **수학 라이브러리** | D3DX (`_vec3`, `_matrix`) | DirectXMath SIMD (`_float3`, `_float4x4`, `_vector`, `_matrix`) |
| **셰이더** | 고정 파이프라인 / D3DX Effect | Effects11(`.fx`), `d3dcompiler` |
| **모델 로딩** | 자체 또는 X파일 | **Assimp** 라이브러리 (FBX, OBJ 등) |
| **텍스처** | `D3DXCreateTextureFromFile` | **DirectXTK** (`DDSTextureLoader`, `WICTextureLoader`) |
| **인스턴싱** | 미지원 | GPU Instancing (VIBuffer_*_Instancing) |
| **렌더타겟** | 단일 백버퍼 위주 | **MRT(Multi Render Target)** + 디퍼드 셰이딩 |

### 타입 별칭 비교
```
DX9: _vec3 = D3DXVECTOR3,   _matrix = D3DXMATRIX
DX11: _float3 = XMFLOAT3,   _matrix = XMMATRIX (SIMD 레지스터)
      _vector = XMVECTOR,    _fvector = FXMVECTOR (호출규약용)
```

---

## 3. 아키텍처 패턴

### 3-1. 싱글톤 퍼사드: CGameInstance
현재 프로젝트는 매니저마다 개별 싱글톤(`CManagement`, `CRenderer`, `CTimerMgr` 등)을 직접 호출하지만, 참고 프로젝트는 **CGameInstance 하나로 통합**.

```
CGameInstance (싱글톤 퍼사드)
├── CGraphic_Device     ← DX11 디바이스/컨텍스트
├── CInput_Device       ← DirectInput
├── CTimer_Manager      ← 타이머
├── CLevel_Manager      ← 레벨(씬) 전환
├── CPrototype_Manager  ← 프로토타입 관리
├── CObject_Manager     ← 오브젝트/레이어 관리
├── CRenderer           ← 렌더 큐 + 디퍼드 렌더링
├── CPipeLine           ← 뷰/프로젝션 행렬
├── CLight_Manager      ← 조명
├── CFont_Manager       ← 폰트
├── CTarget_Manager     ← 렌더타겟(MRT)
├── CShadow             ← 그림자
├── CPicking            ← 마우스 피킹
└── CFrustum            ← 절두체 컬링
```

**장점**: 클라이언트 코드가 `CGameInstance::GetInstance()->XXX()`만 호출하면 됨. 엔진 내부 구조 변경에 Client가 영향받지 않음.

### 3-2. 상속 계층
```
CBase (레퍼런스 카운팅 - AddRef/Release/Free)
├── CGameInstance (싱글톤 퍼사드)
├── CComponent (abstract)
│   ├── CTransform       ← 위치/회전/스케일
│   ├── CShader          ← 셰이더 바인딩
│   ├── CTexture         ← 텍스처
│   ├── CVIBuffer        ← 버텍스/인덱스 (abstract)
│   │   ├── CVIBuffer_Rect / Cube / Terrain / Cell
│   │   └── CVIBuffer_*_Instancing
│   ├── CModel           ← 3D 모델 (Assimp)
│   ├── CCollider        ← 충돌 (AABB/OBB/Sphere)
│   └── CNavigation      ← 내비메시
├── CGameObject (abstract)
│   ├── CContainerObject ← 파츠 기반 (abstract)
│   ├── CPartObject      ← 개별 파츠
│   └── CUIObject        ← UI 전용
├── CLevel (abstract)    ← 레벨(씬)
└── 각종 Manager         ← 내부 관리 클래스
```

> **현재 vs 참고 차이**: `CScene` → `CLevel`, `CManagement` → `CGameInstance`, `CProtoMgr` → `CPrototype_Manager`. 개념은 동일하나 명명/구조 정교화.

### 3-3. 오브젝트 조합 구조 (신규)
```
CContainerObject (예: Player = 몸통 + 무기)
├── CPartObject (Body)   ← 자체 Transform + Model
├── CPartObject (Weapon) ← 부모 뼈에 부착, 독립 렌더
└── ...
```
현재 프로젝트에는 없는 **파츠 오브젝트 시스템**. 복수의 메시를 개별 파츠로 분리하여 부모 뼈 소켓에 동적 부착 가능.

---

## 4. 메인 루프 비교

### 현재 프로젝트 (DX9)
```
Ready → Update → LateUpdate → Render (씬 단위 호출)
```

### 참고 프로젝트 (DX11)
```
Client.cpp::wWinMain()
  └─ MainApp::Create()
       └─ Initialize_Engine() → 디바이스, 입력, 타이머 등 초기화

  루프:
    Timer_Default → 시간 누적
    Timer_60 → 실제 업데이트용 델타타임
    MainApp::Update(fTimeDelta)
      └─ GameInstance::Update_Engine(fTimeDelta)
           └─ Input_Device::Update()    ← 입력 갱신
           └─ Object_Manager::Update()  ← 모든 오브젝트 Tick/LateTick
    MainApp::Render()
      └─ Begin_Draw()  ← 백버퍼 클리어
      └─ Draw()        ← Renderer가 렌더 큐 처리 (디퍼드 패스 포함)
      └─ End_Draw()    ← Present (화면 출력)
```

**변경점**: `LateUpdate`가 `LateTick`으로 변경, 렌더링이 디퍼드 방식으로 다단계 패스.

---

## 5. 렌더 그룹 비교

| 현재 (DX9) | 참고 (DX11) | 용도 |
|------------|------------|------|
| RENDER_PRIORITY | PRIORITY | 배경/스카이박스 |
| RENDER_NONALPHA | NONBLEND | 불투명 오브젝트 |
| - | **SHADOW** | 그림자 맵 패스 (신규) |
| - | **NONLIGHT** | 조명 미적용 렌더 (신규) |
| RENDER_ALPHA | BLEND | 반투명 오브젝트 |
| RENDER_UI | UI | UI 요소 |

---

## 6. 레벨 시스템 & 리소스 관리

### 레벨 단위 리소스 격리
```cpp
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
```
- **STATIC(0번)**: 레벨 전환에도 유지되는 영구 리소스 (셰이더, 공용 텍스처)
- **LOADING**: 로딩 화면 전용 리소스
- **LOGO/GAMEPLAY**: 해당 레벨에서만 사용하는 리소스

`Clear_Resources(iLevelID)` 호출 시 해당 레벨의 프로토타입/오브젝트만 정리 → 메모리 관리 효율화.

### 비동기 로딩 (Loader)
`Level_Loading`에서 스레드를 사용하여 다음 레벨 리소스를 비동기 로드하는 구조.

---

## 7. 서드파티 라이브러리

| 라이브러리 | 역할 | DX9 대응 |
|-----------|------|---------|
| **DirectXTK** | 텍스처 로딩, 폰트, 스프라이트, 입력, 기하도형 | D3DX 유틸리티 |
| **Effects11** | .fx 셰이더 효과 프레임워크 | D3DXEffect |
| **Assimp** | FBX/OBJ 등 3D 모델 임포트 | X파일/자체 파서 |

---

## 8. 핵심 설계 원칙 (현재와 동일/강화)

1. **레퍼런스 카운팅**: `CBase::AddRef()/Release()` - 변경 없음
2. **프로토타입 패턴**: `Clone()` - `PROTOTYPE::GAMEOBJECT` / `PROTOTYPE::COMPONENT`로 타입 구분 추가
3. **팩토리 패턴**: `ClassName::Create(...)` - 변경 없음
4. **싱글톤**: `DECLARE/IMPLEMENT_SINGLETON` 매크로 - 변경 없음
5. **DLL Export**: `ENGINE_DLL` 매크로 - 현재는 정적 라이브러리에서만 사용하지만 참고에서는 실제 DLL

---

## 9. 다음 분석 연결

| 문서 | 핵심 탐구 포인트 |
|------|-----------------|
| O02 | GameInstance 내부 구현, Level/Object/Layer 생명주기 |
| O03 | 디퍼드 렌더링 파이프라인, MRT, Shadow/Light 패스 |
| O04 | VIBuffer 계열 + Assimp Model 로딩 + GPU Instancing |
| O05 | Bounding 볼륨 + Navigation Mesh + QuadTree |
| O06 | 스켈레탈 애니메이션: Bone/Channel/KeyFrame |
| O07 | Client의 실제 게임플레이 구현 패턴 |

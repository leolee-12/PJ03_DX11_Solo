# O08-A: D3D11Device 참조카운트 누수 분석

## 결론 요약

**코드 레벨에서 명시적 AddRef/Release 체인은 균형이 맞음.** 디버그 리포트 시점에서 외부 참조카운트 2는 정상(m_pDevice 자체 + d3dDebug QueryInterface). 만약 3이 나타난다면 D3D11 디버그 레이어의 내부 참조(IntRef) 포함 가능성이 높음.

단, **CBackGround의 멤버 변수 섀도잉** 문제와 **ClearState 미호출** 문제는 별도로 수정 권장.

---

## 1. 참조카운트 소유자 추적 (Logo 레벨 기준)

### 생성/증가 경로

| # | 위치 | 동작 | 누적 |
|---|------|------|------|
| 1 | `Graphic_Device.cpp:22-23` | `D3D11CreateDevice` - 디바이스 생성 | **1** |
| 2 | `Graphic_Device.cpp:59` | `Safe_AddRef(m_pDevice)` - MainApp 출력용 | **2** |
| 3 | `Level.cpp:10` | CLevel 생성자 `Safe_AddRef(m_pDevice)` | **3** |
| 4 | `Loader.cpp:12` | CLoader 생성자 `Safe_AddRef(m_pDevice)` | **4** |
| 5 | `GameObject.cpp:7` | CBackGround 프로토타입 생성 `Safe_AddRef` | **5** |

Loading 완료 후 Logo 전환 시:
- CLevel_Loading 해제 -> CLevel::Free() `-1`, CLoader::Free() `-1` = **3**
- CLevel_Logo 생성 -> CLevel 생성자 `+1` = **4**
- BackGround Clone 생성 -> CGameObject 복사생성자 `+1` = **5**

### Logo 레벨 진입 후 소유자 목록

| 소유자 | 참조 위치 | 참조수 |
|--------|-----------|--------|
| CGraphic_Device | `m_pDevice` (D3D11CreateDevice 원본) | 1 |
| CMainApp | `m_pDevice` (출력 파라미터) | 1 |
| CLevel_Logo | `m_pDevice` (CLevel 베이스) | 1 |
| CBackGround 프로토타입 | `m_pDevice` (CGameObject 베이스) | 1 |
| CBackGround 클론 | `m_pDevice` (CGameObject 베이스) | 1 |
| **합계** | | **5** |

---

## 2. 해제 순서 추적 (MainApp::Free)

```
MainApp::Free()
  |-- Safe_Release(m_pDevice)              // -1 -> 4
  |-- Safe_Release(m_pContext)
  |-- m_pGameInstance->Release_Engine()
  |     |-- Safe_Release(m_pObject_Manager)
  |     |     |-- Layer::Free() -> Clone::Free() -> CGameObject::Free()
  |     |           -> Safe_Release(m_pDevice)    // -1 -> 3
  |     |-- Safe_Release(m_pPrototype_Manager)
  |     |     |-- Prototype::Free() -> CGameObject::Free()
  |     |           -> Safe_Release(m_pDevice)    // -1 -> 2
  |     |-- Safe_Release(m_pLevel_Manager)
  |     |     |-- CLevel_Logo::Free() -> CLevel::Free()
  |     |           -> Safe_Release(m_pDevice)    // -1 -> 1
  |     |-- Safe_Release(m_pGraphic_Device)
  |           |-- CGraphic_Device::Free()
  |                 |-- Safe_Release(m_pSwapChain)
  |                 |-- Safe_Release(m_pDepthStencilView)
  |                 |-- Safe_Release(m_pBackBufferRTV)
  |                 |-- Safe_Release(m_pContext)
  |                 |-- QueryInterface(ID3D11Debug) // +1 -> 2
  |                 |-- ReportLiveDeviceObjects     // *** 여기서 리포트 ***
  |                 |-- d3dDebug->Release()         // -1 -> 1
  |                 |-- Safe_Release(m_pDevice)     // -1 -> 0 (해제)
```

### 리포트 시점 예상 참조카운트: **2** (m_pDevice + d3dDebug)

---

## 3. 만약 리포트가 3을 표시한다면

### 가능한 원인

**A. D3D11 디버그 레이어의 IntRef (가장 가능성 높음)**

D3D11_RLDO_DETAIL 출력은 외부 Refcount와 내부 IntRef를 별도로 표시:
```
Live ID3D11Device at 0x..., Refcount: 2, IntRef: 1
```
Refcount(2) + IntRef(1) = 3으로 오인할 수 있음.

**B. Context의 미정리 바인딩 상태**

`OMSetRenderTargets`로 바인딩된 RTV/DSV가 Context에 내부 참조를 유지할 수 있음. `ClearState()` 호출 없이 Context를 해제하면 D3D11 런타임이 추가 내부 참조를 보유할 가능성 존재.

**C. DXGI 인프라 참조**

SwapChain 생성 시 DXGI 런타임이 디바이스에 대한 내부 참조를 유지할 수 있으며, SwapChain 해제 후에도 DXGI 정리 지연이 발생할 수 있음.

---

## 4. 발견된 실제 문제점

### 문제 1: CBackGround 멤버 변수 섀도잉

`BackGround.h:23-24`에서 CGameObject의 `m_pDevice`/`m_pContext`를 재선언:

```cpp
// BackGround.h - CGameObject에 이미 동일 멤버가 존재
ID3D11Device*         m_pDevice = { nullptr };    // 섀도잉!
ID3D11DeviceContext*  m_pContext = { nullptr };    // 섀도잉!
```

현재는 CBackGround가 자체 멤버에 값을 할당하지 않아 누수가 발생하지 않지만, 향후 CBackGround에서 `m_pDevice`에 접근할 때 부모의 멤버가 아닌 자신의 멤버(nullptr)에 접근하게 되어 예상치 못한 버그 발생 가능.

**권장**: CBackGround의 `m_pDevice`/`m_pContext` 선언 제거.

### 문제 2: ClearState/Flush 미호출

`Graphic_Device.cpp`의 `Free()`에서 Context를 해제하기 전에 바인딩 상태를 정리하지 않음:

```cpp
// 권장: Safe_Release(m_pContext) 전에 추가
m_pContext->ClearState();
m_pContext->Flush();
```

이 호출이 없으면 Context가 내부적으로 RTV/DSV 참조를 유지하여 D3D11 디버그 리포트에서 추가 참조가 표시될 수 있음.

### 문제 3: d3dDebug 미초기화

`Graphic_Device.cpp:229`에서 `d3dDebug`가 nullptr로 초기화되지 않음:

```cpp
ID3D11Debug* d3dDebug;  // 미초기화 -> QueryInterface 실패 시 UB
```

`QueryInterface` 실패 시 `d3dDebug`에 쓰레기 값이 남아, `if (d3dDebug != nullptr)` 검사에서 잘못된 Release 호출(정의되지 않은 동작) 가능.

---

## 5. 검증 방법

리포트 출력에서 `Refcount`과 `IntRef`를 분리 확인:
- `Refcount: 2, IntRef: 0` -> 정상 (d3dDebug + m_pDevice, 이후 모두 해제)
- `Refcount: 2, IntRef: 1` -> D3D11 내부 참조 (ClearState 추가로 해결 가능)
- `Refcount: 3` -> 추가 외부 참조 존재 (위 수정사항 적용 후 재확인 필요)

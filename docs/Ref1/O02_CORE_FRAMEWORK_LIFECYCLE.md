# O02. 코어 프레임워크 & 생명주기

## 1. CGameInstance - 퍼사드 싱글톤 상세

### 초기화 순서 (Initialize_Engine)
```
1. CGraphic_Device   ← DX11 Device + DeviceContext 생성
2. CInput_Device     ← DirectInput 키보드/마우스
3. CTarget_Manager   ← MRT 렌더타겟 관리
4. CPicking          ← 마우스 피킹 (뷰포트 크기 필요)
5. CRenderer         ← 렌더 큐 + 디퍼드 렌더링
6. CTimer_Manager    ← 타이머 관리
7. CPrototype_Manager← 프로토타입 저장소 (레벨 수만큼 배열)
8. CObject_Manager   ← 오브젝트/레이어 관리 (레벨 수만큼 배열)
9. CLevel_Manager    ← 레벨 전환
10. CPipeLine        ← View/Projection 행렬 보관
11. CLight_Manager   ← 조명 관리
12. CFont_Manager    ← DirectXTK SpriteFont
13. CShadow          ← 그림자 맵
14. CFrustum         ← 절두체 컬링
```

### 해제 순서 (Release_Engine) - 초기화의 역순
```
Frustum → Picking → Shadow → Target_Manager → Font_Manager
→ Light_Manager → PipeLine → Renderer → Object_Manager
→ Prototype_Manager → Level_Manager → Input_Device
→ Graphic_Device → Timer_Manager → DestroyInstance()
```

> **핵심 원칙**: 생성 역순 해제. 의존하는 쪽을 먼저 해제해야 댕글링 포인터 방지.

---

## 2. Update 파이프라인

### 매 프레임 실행 순서 (Update_Engine)
```
① Picking::Update()              ← 마우스 레이 갱신
② Input_Device::Update()         ← 키보드/마우스 상태 갱신
③ Object_Manager::Priority_Update() ← 전체 오브젝트 1차 업데이트
④ PipeLine::Update()             ← View/Proj 역행렬 계산
⑤ Frustum::Update()              ← 절두체 평면 갱신
⑥ Object_Manager::Update()       ← 전체 오브젝트 2차 업데이트
⑦ Object_Manager::Late_Update()  ← 전체 오브젝트 후처리
⑧ Level_Manager::Update()        ← 현재 레벨 업데이트
```

### 현재 프로젝트(DX9)와의 비교
| 현재 (DX9) | 참고 (DX11) | 비고 |
|-----------|------------|------|
| `Update_GameObject` | `Priority_Update` | 1차 업데이트 |
| - | `Update` | 2차 업데이트 (신규 단계) |
| `LateUpdate_GameObject` | `Late_Update` | 후처리 |

**3단계 업데이트 이유**: Priority_Update에서 입력/이동 처리 → PipeLine/Frustum 갱신 → Update에서 카메라 위치 기반 로직 → Late_Update에서 렌더 큐 등록.

---

## 3. GameObject 생명주기

### 가상 함수 체인
```cpp
// CGameObject 인터페이스
Initialize_Prototype()     ← 프로토타입 최초 생성 시 1회
Initialize(void* pArg)     ← Clone 후 개별 초기화
Priority_Update(fTimeDelta)← 매 프레임 1차
Update(fTimeDelta)         ← 매 프레임 2차
Late_Update(fTimeDelta)    ← 매 프레임 후처리
Render()                   ← 렌더 큐에서 호출
Render_Shadow()            ← 그림자 패스에서 호출 (신규)
Free()                     ← Release에 의한 소멸
```

### 컴포넌트 장착 패턴
```cpp
// GameObject::Add_Component 내부 흐름
CComponent* pProto = GameInstance->Clone_Prototype(
    PROTOTYPE::COMPONENT, iLevelID, strTag, pArg);
m_Components.emplace(strComponentTag, pProto);
*ppOut = pProto;  // 외부에서 직접 접근용 포인터 보관
```

### 컴포넌트 맵 구조
```
map<_wstring, CComponent*> m_Components;
  "Com_Transform"  → CTransform*
  "Com_Shader"     → CShader*
  "Com_Model"      → CModel*
  "Com_Collider"   → CCollider*
  ...
```

> **현재와 차이**: 현재는 `ID_STATIC/ID_DYNAMIC` 2배열로 구분. 참고에서는 **단일 map**으로 통합, 태그명으로 검색.

---

## 4. GameObject 파생 계층 (신규 개념)

### CContainerObject (복합 오브젝트)
```
CGameObject
└── CContainerObject (abstract)
      │ m_PartObjects: vector<CPartObject*>
      │ m_iNumPartObjects: _uint
      │
      ├── Player (Body + Weapon)
      └── Monster (Body + Weapon + ...)
```

**역할**: 여러 파츠(메시)를 하나의 논리 엔티티로 묶음. 각 파츠는 독립적으로 렌더링/애니메이션.

```cpp
// 파츠 추가 패턴
Add_PartObject(iLevelID, L"Proto_Weapon", PART_WEAPON, &PartDesc);
```

### CPartObject (개별 파츠)
```cpp
struct PART_OBJECT_DESC {
    const _float4x4* pParentMatrix;  // 부모 결합 행렬 포인터
};
// m_CombinedWorldMatrix = 자신의 로컬 * *m_pParentMatrix
```

**핵심**: 부모의 월드 행렬(또는 특정 본 행렬)을 포인터로 참조 → 매 프레임 곱하여 결합 행렬 생성.

### CUIObject (UI 전용)
```cpp
struct UIOBJECT_DESC {
    _float fX, fY, fSizeX, fSizeY;  // 스크린 좌표/크기
};
// 자체 View(항등행렬) + Proj(직교투영) 사용
```

---

## 5. Level(씬) 관리

### CLevel_Manager
```
멤버:
  m_iLevelID     ← 현재 레벨 ID
  m_pCurrentLevel← 현재 활성 레벨
  m_pGameInstance← 리소스 정리용

Change_Level(iLevelID, pNewLevel):
  1. m_pGameInstance->Clear_Resources(m_iLevelID)  ← 이전 레벨 리소스 정리
  2. Safe_Release(m_pCurrentLevel)                  ← 이전 레벨 해제
  3. m_pCurrentLevel = pNewLevel                    ← 새 레벨 설정
  4. m_iLevelID = iLevelID
```

### 레벨 전환 흐름 (Client 기준)
```
MainApp::Start_Level(LEVEL::LOGO)
  └─ Change_Level(LOADING, CLevel_Loading::Create(LOGO))
       └─ CLevel_Loading: 별도 스레드로 LOGO 리소스 로드
            로드 완료 시 → Change_Level(LOGO, CLevel_Logo)
                 └─ 유저 입력 시 → Change_Level(LOADING, CLevel_Loading(GAMEPLAY))
                      └─ 로드 완료 → Change_Level(GAMEPLAY, CLevel_GamePlay)
```

**Loading 레벨이 허브 역할**: 항상 Loading을 거쳐 다음 레벨로 전환 → 비동기 로딩 보장.

---

## 6. Object_Manager 레벨별 격리

### 자료구조
```cpp
map<const _wstring, CLayer*>* m_pLayers;  // 동적 배열[레벨 수]
// m_pLayers[LEVEL::STATIC]   → { "Layer_BackGround": CLayer*, ... }
// m_pLayers[LEVEL::GAMEPLAY] → { "Layer_Player": CLayer*, ... }
```

### Add_GameObject 흐름
```
1. Clone_Prototype(GAMEOBJECT, protoLevelID, tag, arg)
   → Prototype_Manager에서 찾아 Clone
2. Find_Layer(layerLevelID, layerTag)
   → 없으면 CLayer::Create()로 신규 생성
3. Layer->Add_GameObject(pClonedObj)
```

### Clear(iLevelID)
```
해당 레벨의 모든 Layer를 Safe_Release → map clear
→ STATIC 레벨은 Clear하지 않으므로 공유 리소스 유지
```

---

## 7. Prototype_Manager 레벨별 격리

### 자료구조
```cpp
map<const _wstring, CBase*>* m_pPrototypes;  // 동적 배열[레벨 수]
// m_pPrototypes[STATIC]   → 공용 프로토타입 (셰이더, 공용 텍스처)
// m_pPrototypes[GAMEPLAY] → 게임플레이 전용 프로토타입
```

### Clone 분기
```cpp
Clone_Prototype(PROTOTYPE eType, iLevelID, strTag, pArg)
  if (GAMEOBJECT) → dynamic_cast<CGameObject*>→Clone(pArg)
  else            → dynamic_cast<CComponent*>→Clone(pArg)
```

> **현재와 차이**: 현재 프로젝트의 `CProtoMgr`은 레벨 구분 없이 단일 map. 참고 프로젝트는 **레벨별 배열**로 격리 → 레벨 전환 시 해당 레벨 프로토타입만 정리 가능.

---

## 8. Component 시스템

### 프로토타입 vs 클론
```cpp
// 프로토타입 생성 (1회)
CComponent(pDevice, pContext)  ← 원본 생성자
  Initialize_Prototype()       ← 프로토타입 초기화
  m_isCloned = false

// 클론 생성 (복제 시)
CComponent(const CComponent& Proto)  ← 복사 생성자
  m_isCloned = true
  Initialize(pArg)                    ← 개별 초기화
```

`m_isCloned` 플래그로 프로토타입/클론 구분 → Free()에서 원본 리소스 해제 여부 결정에 활용.

### Component가 보유하는 공통 멤버
```cpp
ID3D11Device*      m_pDevice;       // DX11 디바이스
ID3D11DeviceContext* m_pContext;     // DX11 컨텍스트
CGameInstance*     m_pGameInstance;  // 퍼사드 접근
_bool              m_isCloned;       // 클론 여부
```

---

## 9. Layer 구조

```cpp
class CLayer {
    list<CGameObject*> m_GameObjects;  // 순서 보장 리스트
    // Priority_Update / Update / Late_Update → 리스트 순회
};
```

> **현재와 차이**: 현재는 `multimap<_wstring, CGameObject*>`로 태그 기반 검색. 참고에서는 **list** 사용, 인덱스 기반 접근(`Get_Component(iIndex, strTag)`).

---

## 10. 현재 → 참고 프로젝트 용어 대응표

| 현재 (DX9) | 참고 (DX11) | 역할 |
|-----------|------------|------|
| CManagement | CGameInstance | 싱글톤 퍼사드 |
| CScene | CLevel | 씬/레벨 |
| CScene::Ready_Scene | CLevel::Initialize | 씬 초기화 |
| Update_GameObject | Priority_Update | 1차 업데이트 |
| LateUpdate_GameObject | Late_Update | 후처리 |
| CProtoMgr | CPrototype_Manager | 프로토타입 관리 |
| ID_STATIC / ID_DYNAMIC | 단일 map (태그명) | 컴포넌트 분류 |
| multimap (Layer) | list (Layer) | 오브젝트 컬렉션 |
| - | CContainerObject | 복합 오브젝트 (신규) |
| - | CPartObject | 파츠 오브젝트 (신규) |
| - | CUIObject | UI 전용 오브젝트 (신규) |

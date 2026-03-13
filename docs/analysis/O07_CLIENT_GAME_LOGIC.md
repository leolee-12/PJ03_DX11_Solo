# O07. Client 게임 로직 분석

## 1. 레벨 흐름 & 비동기 로딩

### 레벨 전환 시퀀스
```
MainApp::Start_Level(LOGO)
  └─ Change_Level(LOADING, CLevel_Loading(LOGO))
       ├── CLoader 생성 → _beginthreadex(ThreadMain)
       │     └── Loading_For_Logo(): 텍스처, 셰이더, VIBuffer 등록
       └── Update에서 Loader 완료 감지 + SPACE 키 입력
            └── Change_Level(LOGO, CLevel_Logo)
                 └── SPACE 입력
                      └── Change_Level(LOADING, CLevel_Loading(GAMEPLAY))
                           └── Loading_For_GamePlay(): 모든 리소스 로드
                                └── Change_Level(GAMEPLAY, CLevel_GamePlay)
```

### CLoader - 멀티스레드 로딩
```cpp
// _beginthreadex로 별도 스레드 생성
// CRITICAL_SECTION으로 동기화
// CoInitializeEx: COM 초기화 (텍스처 로딩에 필요)
// m_isFinished 플래그로 완료 통보
// Free()에서 WaitForSingleObject → CloseHandle → DeleteCriticalSection
```

---

## 2. 프로토타입 등록 구조

### STATIC 레벨 (영구 리소스)
```
Shader_VtxPosTex     ← 2D UI/스프라이트용
VIBuffer_Rect        ← 2D 사각형
```

### GAMEPLAY 레벨 (게임 중 리소스)
| 카테고리 | 프로토타입 |
|---------|----------|
| 텍스처 | Snow, Terrain(Tile/Mask/Brush), Sky(4장), Explosion(90장) |
| 버퍼 | VIBuffer_Terrain(높이맵), VIBuffer_Cube, Snow 인스턴싱(3000개), Explosion 인스턴싱(300개) |
| 모델 | Fiona.fbx(ANIM), ForkLift.fbx(NONANIM) |
| 셰이더 | VtxNorTex, VtxMesh, VtxAnimMesh, VtxCube, VtxPosTexInstance, VtxPosInstance |
| 충돌 | Collider_AABB, Collider_OBB, Collider_Sphere |
| 내비 | Navigation.dat |
| 오브젝트 | Terrain, Camera_Free, Monster, Sky, Player, Body, Weapon, Snow, Particle_Explosion, Sprite, ForkLift |

---

## 3. Level_GamePlay 레이어 구성

```
Ready_Lights()              ← 디렉셔널/포인트 라이트 + 그림자
Ready_Layer_Camera()        ← 프리 카메라
Ready_Layer_Player()        ← 플레이어(Body + Weapon)
Ready_Layer_Monster()       ← 몬스터 배치
Ready_Layer_BackGround()    ← 지형, 스카이박스, 눈
Ready_Layer_Effect()        ← 폭발 파티클, 스프라이트
```

---

## 4. Player - 복합 오브젝트 (CContainerObject)

### 구조
```
CPlayer (CContainerObject)
├── m_iState: _uint (비트 플래그)
│   IDLE=0x01, RUN=0x02, ATTACK=0x04
├── m_pNavigationCom: CNavigation*
├── m_pColliderCom: CCollider*
└── m_PartObjects[]: vector<CPartObject*>
      [BODY]   → CBody   (스켈레탈 애니메이션 모델)
      [WEAPON] → CWeapon (본 소켓 부착 무기)
      [EFFECT] → (이펙트 슬롯)
```

### 플레이어 상태 머신
```
Priority_Update:
  m_iState = IDLE (매 프레임 초기화)
  WASD 입력 → m_iState |= RUN, Go_Straight/Left/Right/Backward
  마우스 클릭 → m_iState |= ATTACK
  → 부모(CContainerObject)의 Priority_Update → 모든 파츠 갱신

Late_Update:
  Nav 높이 보정 → SetUp_OnNavigation
  렌더 그룹 등록 (SHADOW + NONBLEND)
  충돌 판정 → Intersect_ToPlayer (몬스터와)
```

### CBody - 플레이어 몸체
```
Initialize: Fiona.fbx 모델 + 애니메이션 셰이더 장착
Priority_Update:
  상태에 따라 애니메이션 전환:
    ATTACK → Set_Animation(공격 인덱스, false)
    RUN    → Set_Animation(달리기 인덱스, true)
    IDLE   → Set_Animation(대기 인덱스, true)
  Play_Animation(fTimeDelta)

Get_SocketBoneMatrix_Ptr("Bip001_R_Hand"):
  무기 부착용 소켓 본 행렬 제공

Render: 메시별 텍스처 바인딩 + 본 행렬 바인딩 → Draw
Render_Shadow: 그림자 맵용 별도 셰이더 패스
```

### CWeapon - 무기 (CPartObject)
```cpp
struct WEAPON_DESC {
    const _float4x4* pParentMatrix;      // 부모(Body)의 결합 행렬
    const _float4x4* pSocketBoneMatrix;  // 손 본 행렬
    const _uint*     pParentState;       // 플레이어 상태
};

Late_Update:
  m_CombinedWorldMatrix = 자신의 WorldMatrix
                        × *m_pSocketBoneMatrix (손 본)
                        × *m_pParentMatrix (부모 월드)
  셰이더에 CombinedWorldMatrix 바인딩
```

---

## 5. Monster - 단일 오브젝트

```
CMonster (CGameObject)
├── m_pShaderCom: CShader*
├── m_pModelCom: CModel* (ForkLift - 정적 모델)
└── m_pColliderCom[3]: CCollider* (AABB + OBB + Sphere)

Intersect_ToPlayer():
  GameInstance에서 플레이어 Collider 컴포넌트 가져오기
  → 3종 충돌체 각각 교차 판정
```

---

## 6. 환경 오브젝트

### CTerrain
```
컴포넌트: Shader + Texture(Diffuse/Mask/Brush) + Navigation + VIBuffer_Terrain
Update: Frustum 컬링 → VIBuffer_Terrain::Culling(WorldMatrix)
Render: 멀티 텍스처 바인딩 → NONBLEND 그룹
```

### CSky (스카이박스)
```
컴포넌트: Shader(VtxCube) + Texture(큐브맵) + VIBuffer_Cube
Priority_Update: 카메라 위치를 자신의 위치로 설정
  → 항상 카메라 중심에 위치
Render: PRIORITY 그룹 (가장 먼저, 깊이 테스트 없이)
```

### CCamera_Free
```cpp
struct CAMERA_FREE_DESC : CAMERA_DESC {
    _float fSensor;  // 마우스 감도
};
// CCamera 상속 (엔진에서 제공)
// Priority_Update에서:
//   WASD → 카메라 이동
//   마우스 이동량 → Turn (회전)
//   뷰 행렬 계산 → PipeLine에 Set_Transform(VIEW)
```

---

## 7. 파티클 & 이펙트

### CSnow (눈 - Rect Instancing)
```
VIBuffer_Rect_Instancing (3000개 인스턴스)
  vScale: 0.2~0.6, vCenter: (0,20,0), vRange: 129×1×129
  vLifeTime: 5~8초, vSpeed: 1~3, isLoop: true

Update: Drop(fTimeDelta) → Y축 하강
Render: NONLIGHT 그룹 (조명 미적용)
```

### CParticle_Explosion (폭발 - Point Instancing)
```
VIBuffer_Point_Instancing (300개 인스턴스)
  vScale: 0.1~0.3, vRange: 1×1×1
  vLifeTime: 0.5~2초, vSpeed: 2~4, isLoop: true

Update: Spread(fTimeDelta) → 중심에서 방사형 확산
Render: BLEND 그룹 (반투명)
```

### CSprite (2D 애니메이션)
```
m_fFrame += fTimeDelta × 속도
Bind_ShaderResource(pShader, "g_Texture", (_uint)m_fFrame)
→ 텍스처 인덱스 순환으로 프레임 애니메이션
Render: NONLIGHT 그룹
```

---

## 8. UI 시스템

### CBackGround (CUIObject 상속)
```
UIOBJECT_DESC { fX, fY, fSizeX, fSizeY }
→ 스크린 좌표 기반 직교 투영
→ 자체 View(항등) + Proj(직교) 사용
Render: UI 그룹 (최후 렌더)
```

---

## 9. 셰이더 종류 (총 8개)

| 셰이더 | 정점 포맷 | 용도 |
|--------|----------|------|
| Shader_VtxPosTex | VTXPOSTEX | 2D UI, 배경 |
| Shader_VtxNorTex | VTXNORTEX | 지형 |
| Shader_VtxMesh | VTXMESH | 정적 3D 모델 |
| Shader_VtxAnimMesh | VTXSKINMESH | 스키닝 애니메이션 모델 |
| Shader_VtxCube | VTXCUBE | 스카이박스 |
| Shader_VtxPosTexInstanceParticle | VTXPOSTEX_INSTANCE | Rect 인스턴싱 |
| Shader_VtxPosInstanceParticle | VTXPOS_INSTANCE | Point 인스턴싱 |
| Shader_Deferred | VTXPOSTEX | 디퍼드 렌더링 (합성) |

---

## 10. 오브젝트별 렌더 그룹 배분

```
PRIORITY  → Sky (스카이박스)
SHADOW    → Body (그림자 캐스터)
NONBLEND  → Terrain, Player(Body), Monster, ForkLift
NONLIGHT  → Snow, Sprite (조명 미적용)
BLEND     → Particle_Explosion (반투명)
UI        → BackGround
```

---

## 11. 현재 → 참고 Client 비교 요약

| 항목 | 현재 (DX9) | 참고 (DX11) |
|------|-----------|------------|
| 비동기 로딩 | 없음 (동기 초기화) | **멀티스레드 CLoader** |
| 플레이어 구조 | 단일 CGameObject | **CContainerObject** (Body+Weapon 파츠) |
| 상태 관리 | 개별 변수 | **비트 플래그** (IDLE\|RUN\|ATTACK) |
| 무기 부착 | 없음 | **소켓 본 행렬** 참조 |
| 파티클 | 없음 | **GPU 인스턴싱** (Snow 3000, Explosion 300) |
| 스카이박스 | 없음 | **큐브맵** 텍스처 |
| 지형 | 고정 렌더 | **QuadTree 컬링** |
| UI | 없음 | **CUIObject** (직교 투영) |
| 그림자 | 없음 | **Render_Shadow** 별도 패스 |

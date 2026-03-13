# O08. 이펙트 시스템 심화 분석

> 분석 대상: 참고프로젝트3 (DDProject)
> 범위: Engine CEffect / CEffectMgr + Client Effect_2D / Effect_Single / Effect_Frame / Effect_Particle / Effect_RectParticle / Effect_Trail / Effect_Decal / Effect_Mesh / EffectAdd_Manager

---

## 1. 핵심 책임 및 경계

### CEffect (Engine) — 이펙트 베이스
- **CGameObject 상속**, 모든 이펙트 타입의 공통 인터페이스
- 빌보드(BILL_ALL/BILL_Y), FadeIn/Out, 위치 추적(FixMatrix/ParentMatrix), 방향 이동(Toward_FinalPos), DotProduct 기반 회전
- `SHADER_EFFECT_PASS` enum으로 셰이더 패스 관리 (14종)
- 순수 가상: `Clone()` — 프로토타입 패턴 강제

### CEffectMgr (Engine) — 이펙트 매니저 싱글톤
- **프로토타입 레지스트리** (`m_mapEffectKind`: tag → list<CEffect*>`)
- **활성 이펙트 풀** (`m_listEffect`: 매 프레임 Update/Render)
- **파티클 프리셋** (`m_mapParticleInfo`: tag → PARTICLE_INFO, .dat 파일 로딩)
- Add_Effect 7개 오버로드 — Clone 후 속성 세팅하여 활성 리스트에 추가

### CEffect_2D (Client) — 2D 이펙트 중간 베이스
- CEffect 상속, **RectTexture 기반 빌보드** 이펙트의 공통 기능
- Scaling(증가/감소/루프/스프링), Rotating, UV Animation, MoveDir, FadeOut
- `Perform_Function()` — 매 프레임 스케일링/회전/이동/페이드 일괄 처리

### CEffect_Mesh (Client) — 3D 메시 이펙트 중간 베이스
- CEffect 상속, **StaticMesh 기반** 3D 이펙트의 공통 기능
- Scaling, Rotating, Flicker(깜빡임), UV Animation, ScalingLikeSpring
- `Perform_Function()` + `Perform_Flicker()` 분리

---

## 2. 상속 구조

```
CBase → CGameObject → CEffect (Engine)
                          ├── CEffect_2D (Client, abstract)
                          │     ├── CEffect_Single    (단일 텍스처)
                          │     └── CEffect_Frame     (프레임 애니메이션)
                          ├── CEffect_Particle         (포인트 스프라이트)
                          ├── CEffect_RectParticle     (사각형 파티클, 셰이더 기반)
                          ├── CEffect_Trail            (무기 궤적)
                          ├── CEffect_Decal            (데칼 투영)
                          └── CEffect_Mesh (abstract)
                                ├── Effect_Mesh_Shield
                                ├── Effect_Mesh_Bolt
                                ├── Effect_Mesh_Wave
                                ├── Effect_Mesh_AuraSphere
                                ├── Effect_Mesh_Spiral
                                ├── Effect_Mesh_ElectricGroup1/2
                                └── Effect_Mesh_LightningTree
```

---

## 3. 소유권 및 참조 관계

```
CEffectMgr (Singleton)
  ├─ m_mapEffectKind   : map<tchar*, list<CEffect*>>  [프로토타입 소유 — Release 책임]
  ├─ m_listEffect      : list<CEffect*>               [활성 Clone 소유 — dead시 Release]
  └─ m_mapParticleInfo : map<tchar*, PARTICLE_INFO*>   [프리셋 소유 — Safe_Delete]

CEffect (Base)
  ├─ m_pmatFix         : const _matrix*    [외부 참조, 비소유 — 본 행렬 등]
  ├─ m_pmatParent      : const _matrix*    [외부 참조, 비소유 — 부모 월드 행렬]
  └─ m_pDataMgr        : CDataManager*     [싱글톤 참조, Add_Ref/Release]

CEffect_2D 계열
  ├─ m_pRendererCom    : CRenderer*        [Clone시 Add_Ref, Free에서 Release]
  ├─ m_pTextureCom     : CTexture*         [Clone시 Add_Ref]
  └─ m_pBufferCom      : CRect_Texture*    [Clone시 Add_Ref]

CEffect_Particle
  ├─ m_pVb             : IDirect3DVertexBuffer9*  [Clone시 새로 CreateVertexBuffer]
  └─ m_vecParticle     : list<PARTICLE_ATT*>      [new/Safe_Delete 직접 관리]

CEffect_Trail
  ├─ m_pShaderCom      : CShader*          [Clone시 Add_Ref]
  ├─ m_pBufferCom      : CTrail_Texture*   [Clone시 Add_Ref]
  ├─ m_pmatPlayerInfo  : const _matrix*    [외부 참조, 비소유]
  └─ m_pmatWeaponRef   : const _matrix*    [외부 참조, 비소유]

CEffect_Decal
  ├─ m_pTextureMaskCom : CTexture*         [마스크 텍스처, Clone시 Add_Ref]
  └─ m_pBufferCom      : CCube_Texture*    [큐브 버퍼]
```

---

## 4. 이펙트 타입별 렌더링 방식

### 4-1. Effect_Single — 단일 텍스처 빌보드
- **렌더 방식**: RectTexture 쿼드 + Effect 셰이더
- **기본 패스**: `EFFECT_2D_ALPHABLEND`
- **셰이더 변수**: WorldViewProj, DiffuseTexture, Color, UV, UVAnimation, DepthTexture(소프트)
- **특징**: 가장 단순한 2D 이펙트, UV 스크롤 지원

### 4-2. Effect_Frame — 프레임 애니메이션 빌보드
- **렌더 방식**: 텍스처 배열에서 fFrame 인덱스로 셀렉트
- **기본 패스**: `EFFECT_2D_SOFTBLEND` (Soft Particle)
- **프레임 제어**: `FRAME(fFrame, fMax, fCount)` — fCount*dt로 프레임 진행
- **Clone 시 랜덤화**: `m_bMixFrame=TRUE`면 Clone시 랜덤 시작 프레임 → 동시 생성해도 비동기 재생
- **루프 제어**: `m_bIsLoop`, `m_iLoopLimit` — 지정 횟수 재생 후 사망
- **프러스텀 컬링**: `CFrustumManager::IsSphereInFrustum()` 으로 화면 밖 이펙트 스킵

### 4-3. Effect_Particle — 포인트 스프라이트 파티클
- **렌더 방식**: Fixed Pipeline 포인트 스프라이트 (`D3DRS_POINTSPRITEENABLE`)
- **렌더 큐**: `RENDER_ALPHA` 또는 `RENDER_DEFAULT` (bAlphaBlend에 따라)
- **VB 관리**: Dynamic VB (2048개), Batch 512개씩 Lock/Draw/Unlock 반복
- **위치 업데이트**: 방향벡터 × 속도 × dt, 중력(GRAVITY × age), GatherToSpot(목표점 수렴)
- **생성 모드**: EmitRate 기반 점진 생성 vs CreateOnce 일괄 생성
- **위치 퍼짐**: bStartPosSpread — 구면 랜덤 분포, bSpreadExceptY — Y축 제외
- **FadeOut**: 수명 - fadeTime 이후 alpha 감소 + 색상 변화(FadeColor)
- **FollowTrackingPath**: FixMatrix/ParentMatrix 기반 본 추적 파티클

### 4-4. Effect_RectParticle — 사각형 파티클 (셰이더 기반)
- **렌더 방식**: 개별 RectTexture 쿼드 × Transform 배열 + 셰이더
- **Transform 배열**: `CTransform** m_pTransformArr` — 파티클 개수만큼 동적 할당
- **패스**: `EFFECT_RECTPARTICLE`, `EFFECT_RECTPARTICLE_INV`, `EFFECT_MESH_FOGMASK`(연기)
- **추가 기능**: Z축 회전(m_bParticleRotZ), 버블 효과(사인파 횡이동), 연기 모드(마스크맵+페이더맵)
- **ViewZ 정렬**: `Compute_RectParticle_ViewZ()` — ALPHA_SORT 구조체로 거리순 정렬 후 역순 렌더
- **개별 셰이더 세팅**: 파티클마다 `Set_ConstantTable()` → `CommitChanges()` → `BeginPass/EndPass`

### 4-5. Effect_Trail — 무기 궤적
- **렌더 방식**: 포인트 리스트 → CTrail_Texture의 동적 버텍스로 변환 + 전용 셰이더
- **포인트 수집**: `Memorize_Point()` — 무기 본 행렬의 양쪽 끝점(±TrailSize/2) 매 EmitRate마다 기록
- **최대 포인트**: m_iTrailCount(160개) — 초과시 front pop
- **셰이더**: 별도 `Com_Shader_TrailEffect`, Pass 0, World/View/Proj 분리 전달
- **렌더 큐**: `RENDER_ALPHA`

### 4-6. Effect_Decal — 데칼 투영
- **렌더 방식**: CubeTexture(박스 메시) + 디퍼드 데칼 셰이더
- **셰이더 입력**: DepthTexture, NormalTexture, DiffuseTexture, MaskTexture
- **역투영 복원**: ProjInv, ViewInv, WorldInv 행렬로 깊이→월드 좌표 변환
- **페이드**: m_fDecalMask_FadeInTime이 0 이하가 되면 사망 (시간 기반 소멸)
- **마스크맵**: 선택적 마스크 텍스처로 데칼 형상 제어

### 4-7. Effect_Mesh — 3D 메시 이펙트 (추상 베이스)
- **StaticMesh 기반**: 실린더, 구체, 스파이럴 등 3D 메시를 이펙트로 사용
- **파생 클래스**: Shield, Bolt, Wave, AuraSphere, Spiral, ElectricGroup1/2, LightningTree
- **기능**: Scaling(선형/루프/스프링), Rotating(3축), Flicker(알파 왕복), UV Animation

---

## 5. CEffectMgr — 매니저 핵심 동작

### 5-1. 프로토타입 등록 (`Ready_Proto`)
```
외부에서 CEffect* Create() → EffectMgr::Ready_Proto(tag, pEffect)
  → m_mapEffectKind[tag].push_back(pEffect)
```
- 하나의 태그에 **복수 프로토타입** 등록 가능 (리스트)
- Add_Effect 시 리스트에서 **랜덤** 또는 **인덱스 지정**으로 Clone

### 5-2. 이펙트 생성 (`Add_Effect` 7개 오버로드)
```cpp
// 오버로드 패턴:
Add_Effect(tag)                          // 태그로 Clone, 기본
Add_Effect(tag, pos)                     // + 위치
Add_Effect(tag, pos, scale)              // + 스케일
Add_Effect(tag, pos, scale, matFix)      // + 본 추적 행렬
Add_Effect(tag, pos, scale, matFix, matParent)  // + 부모 행렬
Add_Effect(pEffect)                      // 직접 만든 이펙트 삽입
Add_EffectList(tag, count, ...)          // 복수 생성
```
- 모든 오버로드 내부: **Clone → 속성 Set → m_listEffect.push_back**

### 5-3. 파티클 프리셋 로딩 (`Init_EffectData`)
```
.dat 파일에서 PARTICLE_INFO 구조체 ifstream::read
→ m_mapParticleInfo[tag] = new PARTICLE_INFO(loaded)
```
- ~25개 프리셋 (Shield, Fire, Blood, Snow, Smoke, Hit, Leaf 등)
- Find_ParticleInfo(tag)로 런타임 검색

### 5-4. 프레임 루프
```
Update_Effect(fTimeDelta):
  for each effect in m_listEffect:
    if effect->Update() returns 1 (dead):
      Safe_Release(effect) → erase
    else:
      continue

Render_Effect():
  for each effect in m_listEffect:
    effect->Render()
```

### 5-5. 특수 함수
- `Kill_TheseEffectlist(tag)` — 특정 태그의 활성 이펙트 전체 사망 처리
- `Release_Effect(tag)` — 프로토타입 해제 (씬 전환 시)
- `Reset_Effect()` — 전체 활성 이펙트 클리어

---

## 6. CEffect 베이스 — 공통 메커니즘

### 6-1. 빌보드 (`Bill_Perform`)
```cpp
BILL_ALL:  뷰 행렬 역행렬 → 전체 축 카메라 방향
BILL_Y:    Y축만 빌보드 (수직 유지)
```

### 6-2. 위치 추적 (`Revision_Pos`)
```cpp
if (m_pmatFix):
  pos = FixMatrix[3] (행렬의 위치 성분)
  if (m_pmatParent):
    pos = (FixMatrix * ParentMatrix)[3]
  pos += m_vDetailPos (미세 오프셋)
  → TransformCom에 반영
```

### 6-3. 방향 이동 (`Toward_FinalPos`)
```cpp
if (m_bUseFinalPos):
  dir = FinalPos - currentPos
  if (distance < 0.3f): return (도착)
  pos += normalize(dir) * speed * dt
```

### 6-4. DotProduct 회전 (`DotProduct_Calc`)
```cpp
vLook = normalize(targetPos - effectPos)
dot = D3DXVec3Dot(&vRight, &vLook)
angle = acos(dot)
→ Y축 회전으로 타겟 방향 자동 정렬
```

### 6-5. 사망 조건
- `m_bUseTimeDead && m_fTime <= 0` — 시간 초과
- `m_bDead == TRUE` — 명시적 사망 플래그
- `Update() return 1` — 매니저에서 감지 후 Release

---

## 7. DirectX API 호출 지점

| API | 위치 | 용도 |
|-----|------|------|
| `GetTransform(D3DTS_VIEW/PROJ)` | Effect_Single/Frame/Decal/Trail Render | View/Proj 행렬 획득 |
| `SetTransform(D3DTS_WORLD)` | Effect_Particle Pre_Render | 월드 행렬 초기화(Identity) |
| `CreateVertexBuffer` | Effect_Particle Clone/Initialize | 포인트 스프라이트 VB 생성 |
| `Lock/Unlock VB` | Effect_Particle Render | 동적 파티클 데이터 기록 |
| `DrawPrimitive(D3DPT_POINTLIST)` | Effect_Particle Render | 포인트 스프라이트 드로우 |
| `SetRenderState(POINTSPRITEENABLE)` | Effect_Particle Pre/Post_Render | 포인트 스프라이트 모드 |
| `SetRenderState(POINTSCALEENABLE)` | Effect_Particle Pre/Post_Render | 거리 기반 크기 조절 |
| `SetRenderState(POINTSIZE/SCALE_A/B/C)` | Effect_Particle Pre_Render | 포인트 크기 공식 파라미터 |
| `SetRenderState(ALPHABLENDENABLE)` | Effect_Particle Pre/Post_Render | 알파 블렌딩 On/Off |
| `SetRenderState(ALPHATESTENABLE)` | Effect_Particle Pre/Post_Render | 알파 테스트 모드 |
| `SetRenderState(ZWRITEENABLE=FALSE)` | Effect_Particle Pre_Render | 깊이 쓰기 비활성화 |
| `SetTextureStageState` | Effect_Particle Pre/Post_Render | 알파 연산 설정 |
| `SetFVF` | Effect_Particle Render | 파티클 정점 포맷 |
| `SetStreamSource` | Effect_Particle Render | VB 바인딩 |

**셰이더 기반 이펙트 (LPD3DXEFFECT):**

| 셰이더 API | 위치 | 용도 |
|-------------|------|------|
| `SetMatrix/SetVector/SetFloat` | 각 이펙트 Set_ConstantTable | 셰이더 상수 전달 |
| `SetUp_OnShader(pEffect, semantic)` | Texture/Transform 컴포넌트 | WVP/텍스처 바인딩 |
| `CommitChanges` | Effect_Single/Frame/Decal Render | 상수 적용 |
| `BeginPass/EndPass` | 모든 셰이더 이펙트 Render | 렌더 패스 실행 |
| `Begin/End` | Effect_Trail Render | 전체 이펙트 시작/종료 |
| `Get_EffectHandle` | Effect_Trail Render | LPD3DXEFFECT 핸들 획득 |

---

## 8. 셰이더 패스 체계 (SHADER_EFFECT_PASS)

| 패스 | 인덱스 | 용도 |
|------|--------|------|
| EFFECT_2D_SOFTBLEND | 0 | 소프트 파티클 (깊이 비교) |
| EFFECT_2D_ALPHABLEND | 1 | 단순 알파 블렌딩 |
| EFFECT_ALPHABLEND_NO_Z | 2 | Z 비교 없는 알파 블렌딩 |
| EFFECT_RECTPARTICLE | 3 | 사각형 파티클 기본 |
| EFFECT_RECTPARTICLE_INV | 4 | 사각형 파티클 역블렌드 |
| EFFECT_MESH_ALPHABLEND | 5 | 메시 알파 블렌딩 |
| EFFECT_MESH_ALPHABLEND_CULLNONE | 6 | 메시 양면 렌더링 |
| EFFECT_MESH_ALPHATEST | 7 | 메시 알파 테스트 |
| EFFECT_MESH_MASK | 8 | 메시 마스크맵 |
| EFFECT_MESH_FOGMASK | 9 | 연기 마스크 (RectParticle 연기) |
| EFFECT_DECAL | 10 | 디퍼드 데칼 |
| EFFECT_MESH_PROJECTILE | 11 | 투사체 메시 |

---

## 9. 디자인 패턴 정리

| 패턴 | 적용 위치 | 설명 |
|------|-----------|------|
| **Prototype** | CEffectMgr → Clone | 프로토타입 등록 후 Clone으로 인스턴스 생성 |
| **Singleton** | CEffectMgr | DECLARE_SINGLETON 매크로 |
| **Template Method** | CEffect → 파생 클래스 | Ready/Update/Render 가상 함수 오버라이드 |
| **Facade** | CEffectAdd_Manager | EFFECT_ID enum → 복잡한 이펙트 조합을 단일 호출로 |
| **Strategy** | SHADER_EFFECT_PASS | 런타임에 셰이더 패스 교체로 렌더링 전략 변경 |
| **Object Pool (부분적)** | CEffectMgr::m_listEffect | 활성 이펙트 리스트 관리 (단, 재활용 없이 매번 Clone→Release) |
| **Flyweight** | CEffect_2D/Mesh 컴포넌트 공유 | Renderer/Texture/Buffer는 원본 Add_Ref로 공유 |

---

## 10. 설계 인사이트 및 한계

### 강점
1. **타입별 최적 렌더링**: 파티클은 포인트 스프라이트(최소 오버헤드), 2D는 빌보드 쿼드, 3D는 StaticMesh — 용도에 맞는 렌더링 전략
2. **프로토타입 + Clone**: 한 번 세팅한 이펙트를 태그 기반으로 즉시 복제 — 생성 코드 최소화
3. **파티클 프리셋 시스템**: .dat 파일에서 PARTICLE_INFO 로딩 → 데이터 드리븐 파티클 설정
4. **Soft Particle**: 깊이 텍스처 비교로 지형/오브젝트 경계에서 부드러운 블렌딩
5. **디퍼드 데칼**: 깊이/노말 텍스처로 역투영 → 임의 곡면에 데칼 투영 가능
6. **EffectAdd_Manager**: 복잡한 이펙트 조합(메시 + 파티클 동시 생성)을 EFFECT_ID 하나로 추상화

### 한계/개선점
1. **Add_Effect 오버로드 폭발**: 7개 오버로드 — 빌더 패턴이나 EffectDesc 구조체로 통합 가능
2. **파티클 재활용 없음**: 매번 Clone → Release, 오브젝트 풀 적용 시 GC 부하 감소 가능
3. **RectParticle 개별 DrawCall**: 파티클마다 Set_ConstantTable + DrawCall — 인스턴싱으로 개선 여지
4. **Effect_Particle Fixed Pipeline**: 포인트 스프라이트가 FF 파이프라인 사용 — 다른 이펙트는 셰이더 기반인데 비일관
5. **Clone 시 Add_Ref 이중 호출**: Trail의 Clone에서 컴포넌트를 rhs로 복사 후 Add_Ref 하고, 다시 맵에 넣으며 Add_Ref — 참조 카운트 2회 증가, Free에서 1회만 Release → **잠재적 메모리 누수**
6. **EffectAdd_Manager 확장성**: switch-case 기반 — 이펙트 조합이 많아지면 비대해짐, 데이터 드리븐 조합 시스템 고려

### 자체 프레임워크 적용 시 고려사항
- **이펙트 디스크립터 구조체** 도입: 위치/스케일/행렬/FadeOut 등을 하나의 구조체로 묶어 Add_Effect 오버로드 제거
- **파티클 풀링**: PARTICLE_ATT를 프리리스트로 관리하여 new/delete 빈도 감소
- **GPU 파티클**: Compute Shader 기반 파티클로 CPU 부하 이전 (DX11+ 전환 시)
- **데칼 시스템 분리**: 디퍼드 데칼은 이펙트와 별도 렌더 패스로 관리하는 것이 일반적
- **이펙트 에디터 연동**: 현재 .dat 기반 프리셋 → 비주얼 에디터에서 실시간 편집 지원 고려

# 이펙트/파티클 시스템 심화 분석

> 참고프로젝트2 — Dragon Ball FighterZ 모작
> 분석 대상: CEffect, CEffect_Layer, CEffect_Manager, CEffect_Animation, CEffect_Blend/NoneLight/ZNone/Overlap, CParticle, CParticle_Manager

---

## 1. 핵심 책임과 경계

| 클래스 | 책임 | 위치 |
|--------|------|------|
| `CEffect` | 이펙트 베이스. 3D 모델 기반, 키프레임 애니메이션(S/R/T), 빌보딩, 셰이더 패스 | Client (CGameObject 상속) |
| `CEffect_Blend` | 알파 블렌딩 이펙트 (RG_BLEND 렌더 그룹) | Client (CEffect 상속) |
| `CEffect_NoneLight` | 라이팅 미적용 이펙트 | Client (CEffect 상속) |
| `CEffect_ZNone` | Z버퍼 미적용 이펙트 (항상 최상위) | Client (CEffect 상속) |
| `CEffect_Overlap` | 오버랩 이펙트 | Client (CEffect 상속) |
| `CEffect_Animation` | 이펙트 전용 키프레임 보간 (map 기반, S/R/T + isNotPlaying) | Client (CBase 상속) |
| `CEffect_Layer` | 이펙트 묶음 단위. 레이어 좌표계 + 추적(Following) + 재생 제어 | Client (CBase 상속) |
| `CEffect_Manager` | 이펙트 전체 관리. 프로토타입 저장, Clone→deque, 업데이트/정리 | Client (싱글톤) |
| `CParticle` | 파티클 베이스. GPU 인스턴싱 기반 | Client (CGameObject 상속) |
| `CParticle_Manager` | 파티클 오브젝트 풀 관리. Play/Stop 인터페이스 | Client (싱글톤) |

### 시스템 경계

- **이펙트 시스템**: 데이터 드리븐 (JSON/바이너리에서 키프레임 로드) → 3D 모델 + 셰이더 조합
- **파티클 시스템**: 코드 드리븐 (타입별 프로토타입 Clone) → GPU 인스턴싱
- 두 시스템은 **독립적으로 운영** — CEffect_Manager와 CParticle_Manager가 별도 싱글톤

---

## 2. 클래스 간 소유/참조 관계

```
CEffect_Manager (Singleton)
 ├─ map<wstring, CEffect_Layer*>  m_FinalEffects   [소유] 프로토타입 레이어
 ├─ deque<CEffect_Layer*>         m_UsingEffect     [소유] 활성 Clone 레이어
 ├─ vector<CEffect*>              m_TestEffect      [소유] 에디터 테스트용
 ├─ map<wstring, CModel*>         m_EffectModel     [소유] 이펙트 모델 리소스
 └─ map<wstring, CTexture*>       m_EffectTexture   [소유] 이펙트 텍스처 리소스

CEffect_Layer
 ├─ CTransform*           m_pTransformCom       [소유] 레이어 좌표계
 ├─ CTransform*           m_pCopyTransformCom   [소유] Clone용 보조 좌표계
 ├─ CTransform*           m_pRotationTransformCom [소유] 빌보딩 회전용
 ├─ vector<CEffect*>      m_MixtureEffects      [소유] 포함된 이펙트들
 ├─ const _float4x4*      m_pPlayerMatrix       [참조] 추적 대상 본 행렬
 └─ const CTransform*     m_pPlayerTransformCom [참조] 추적 대상 Transform

CEffect (CGameObject)
 ├─ CEffect_Animation*    m_pAnimation          [소유, Clone]
 ├─ CShader*              m_pShaderCom          [소유]
 ├─ CModel*               m_pModelCom           [소유]
 ├─ CTexture*             m_pDiffuseTextureCom  [소유]
 └─ CTexture*             m_pMaskTextureCom     [소유]

CParticle_Manager (Singleton)
 └─ unordered_map<PARTICLE_ID, vector<CParticle*>>  m_ParticlePools  [소유]
```

---

## 3. 한 프레임 기준 호출 흐름

### 이펙트 시스템

```
CEffect_Manager::Update(dt)
│
├─ m_TestEffect → Update (에디터)
│
└─ m_UsingEffect (deque) 순회:
    ├─ m_bIsDoneAnim == true → Free() + erase    ← 종료된 레이어 제거
    └─ CEffect_Layer::Update(dt)
        │
        ├─ for each Effect:
        │   ├─ LayerMatrix 계산 (Following 모드)
        │   │   ├─ m_bIsFollowing → CopyTransform × PlayerMatrix 합산
        │   │   └─ PlayerTransformCom 있으면 × WorldMatrix 추가 합산
        │   └─ pEffect->Get_Layer_Matrix(finalMatrix)   ← 레이어 행렬 전달
        │
        └─ Play_Effect_Animation(dt)
            ├─ pos += dt × tickPerSecond
            ├─ if pos > duration → m_bIsDoneAnim = true
            ├─ currentFrame = pos / (duration/numKeyFrames)
            └─ for each Effect:
                └─ CEffect::Play_Animation(currentFrame)
                    └─ CEffect_Animation::Play_Animation(frame, isLoop)
                        ├─ upper_bound(frame) → 전후 키프레임 찾기
                        ├─ Lerp(Scale), Lerp(Position), Lerp(Rotation)
                        └─ return EFFECT_KEYFRAME (보간된 S/R/T)
                    → Set_Effect_Scaled/Position/Rotation
                    → WorldMatrix = EffectLocal × LayerMatrix
                    → [빌보딩] LookAt(CamPosition)

CEffect_Manager::Late_Update(dt)
│
└─ for each UsingEffect:
    └─ CEffect_Layer::Late_Update(dt)
        └─ for each Effect (if !IsNotPlaying && !SpriteEnd):
            └─ CRenderer::Add_RenderObject(RG_BLEND/RG_BACKSIDE/etc, this)

Render 시점 (CRenderer가 호출):
CEffect_Blend::Priority_Render(dt)   ← 스프라이트 시트 UV 업데이트
CEffect_Blend::Render(dt)
 ├─ Bind_ShaderResources (WVP, Color, GlowFactor 등)
 ├─ DiffuseTexture + AlphaTexture 바인드
 ├─ CShader::Begin(passIndex)
 └─ CModel::Render(meshIndex)
```

### 파티클 시스템

```
CParticle_Manager::Update(dt)
 └─ for each pool → for each active particle → particle->Update(dt)

CParticle_Manager::Late_Update(dt)
 └─ for each pool → for each active particle → particle->Late_Update(dt)
     └─ CRenderer::Add_RenderObject(렌더 그룹, this)

재생 요청:
CParticle_Manager::Play(PARTICLE_ID, position)
 ├─ pool에서 비활성 파티클 검색
 ├─ 없으면 → Clone_GameObject로 새로 생성 + pool에 추가
 ├─ Set_Particle_Active(true)
 └─ Set_Position(position)
```

---

## 4. 이펙트 키프레임 애니메이션

### EFFECT_KEYFRAME 구조

```cpp
struct EFFECT_KEYFRAME {
    XMFLOAT3 vScale;       // 스케일
    XMFLOAT3 vRotation;    // 회전 (오일러)
    XMFLOAT3 vPosition;    // 위치
    bool     bIsNotPlaying; // 비활성 플래그
    float    fCurTime;
    float    fDuration;
};
```

### CEffect_Animation 보간 로직

```cpp
// map<_uint, EFFECT_KEYFRAME> — 키: 프레임 번호, 값: 키프레임
Play_Animation(curFrame, isLoop)
│
├─ upper_bound(curFrame) → it1 (다음 키프레임)
├─ prev(it1) → it2 (이전 키프레임)
├─ factor = (curFrame - it2.key) / (it1.key - it2.key)
├─ Position = Lerp(it2.Position, it1.Position, factor)
├─ Scale    = Lerp(it2.Scale, it1.Scale, factor)
├─ Rotation = Lerp(it2.Rotation, it1.Rotation, factor)   ← 오일러 Lerp
├─ bIsNotPlaying = it2.bIsNotPlaying
│
└─ 루프 모드: curFrame = fmod(curFrame, lastKey)
```

**캐릭터 애니메이션과의 차이:**
- 캐릭터: 채널별 본 키프레임, 쿼터니언 Slerp, 연속 시간 기반
- 이펙트: 오브젝트 단위 S/R/T 키프레임, 오일러 Lerp, 프레임 번호 기반

---

## 5. 이펙트 레이어와 Copy(Clone) 패턴

### 프로토타입 → 인스턴스 생성

```
전투 중 이펙트 생성 요청:
CEffect_Manager::Copy_Layer("BurstU-1", &copyDesc)
│
├─ Find_Effect_Layer("BurstU-1") → 프로토타입 레이어
├─ pLayer->Clone(copyDesc, isBillboarding)
│   ├─ CEffect_Layer 복사 생성자:
│   │   ├─ 각 CEffect를 pProtoEffect->Clone(ForCopyInform)
│   │   └─ IMGUI_Shader_Tab 텍스처 SRV 복사 등록
│   └─ Initialize(copyDesc):
│       ├─ PlayerMatrix → 방향 판단 (좌/우)
│       ├─ 좌향 시: LayerMatrix × RotationY(180°) + 위치 보정
│       └─ 우향 시: LayerMatrix + 위치만 합산
│
└─ m_UsingEffect.push_back(clone)   ← deque에 추가
```

### Following 모드 (본 추적)

```cpp
COPY_DESC {
    const _float4x4* pPlayertMatrix;   // 본 행렬 포인터 (매 프레임 자동 갱신)
    CTransform* pTransformCom;          // 캐릭터 Transform
    _int m_isPlayerDirRight;            // 방향
};
```

매 프레임 `Update`에서:
1. `m_bIsFollowing == true` → CopyTransform의 월드 행렬 기반
2. PlayerMatrix에서 Position 추출 → LayerMatrix에 합산
3. 결과 행렬을 각 Effect에 전달 → `EffectLocal × LayerMatrix` 로 최종 월드 행렬 계산

---

## 6. 이펙트 타입과 렌더 그룹

### 이펙트 타입 (EFFECT_TYPE)

| 타입 | 렌더 그룹 | 특성 |
|------|-----------|------|
| `EFFECT_NONELIGHT` | RG_NONLIGHT | 라이팅 미적용 |
| `EFFECT_BLEND` | RG_BLEND / RG_BACKSIDE_EFFECT | 알파 블렌딩 |
| `EFFECT_ZNONE` | (별도) | Z버퍼 무시 |
| `EFFECT_OVERLAP` | (별도) | 오버랩 합성 |

### 렌더 그룹 결정 로직 (CEffect_Blend 기준)

```
m_bIsBackSideEffect == true  → RG_BACKSIDE_EFFECT (캐릭터 뒤)
m_bIsBackSideEffect == false → RG_BLEND (캐릭터 앞)
```

### 더블 렌더 패턴

CEffect_Blend는 매 프레임 **두 번 렌더**:
1. 첫 렌더: passIndex=5 (글로우맵용 — 디퍼드 G-Buffer에 쓰기)
2. 둘째 렌더: passIndex=m_iChangePassIndex (실제 이펙트 렌더)

이를 통해 하나의 이펙트가 글로우 패스와 최종 패스 양쪽에 출력.

---

## 7. 파티클 오브젝트 풀

### 풀 구조

```
CParticle_Manager
 └─ unordered_map<PARTICLE_ID, vector<CParticle*>>
     ├─ COMMON_HIT_PARTICLE    → [10개 사전 생성]
     ├─ FREIZA_ULTIMATE_1      → [3개 사전 생성]
     ├─ FREIZA_ULTIMATE_3      → [3개 사전 생성]
     └─ ... (6종)
```

### Play 로직

```
Play(PARTICLE_ID, position)
│
├─ pool에서 IsActive()==false인 파티클 검색
├─ 찾으면 → 재사용
├─ 못 찾으면 → Clone_GameObject()로 새로 생성 + pool에 추가
│
├─ Set_Particle_Active(true)
└─ Set_Position(position)
```

### Stop 로직

```
Stop(PARTICLE_ID)
 └─ 해당 풀의 모든 활성 파티클 → Set_Particle_Active(false)
```

**풀 확장 전략:** 부족 시 동적 생성. 축소 없음 (High Water Mark 유지).

---

## 8. 사용된 디자인 패턴

### 1) 프로토타입-클론 (이펙트 시스템 전반)

```
m_FinalEffects["BurstU-1"] (프로토타입)
    → Clone() → m_UsingEffect (인스턴스)
        → 각 CEffect도 Clone → CEffect_Animation도 Clone
```

3단계 Clone 체인: Manager → Layer → Effect → Animation

### 2) 싱글톤 (매니저 2개)

- `CEffect_Manager::Get_Instance()` — 이펙트 전역 관리
- `CParticle_Manager::Get_Instance()` — 파티클 전역 관리

### 3) 오브젝트 풀 (파티클)

- 사전 생성 (Initialize) + 부족 시 동적 확장
- Active/Inactive 토글로 재사용

### 4) 컴포지트 (Effect_Layer → Effect[])

- Layer가 여러 Effect를 묶어 하나의 연출 단위로 관리
- Layer의 애니메이션 시간이 모든 자식 Effect에 전파

### 5) 전략 패턴 (이펙트 서브클래스)

각 서브클래스(Blend/NoneLight/ZNone/Overlap)가 렌더 그룹/셰이더 패스/바인딩 방식을 다르게 구현.

### 6) 데이터 드리븐 (이펙트 직렬화)

`EFFECT_LAYER_DATA` → `Set_Saved_Effects()` → 프로토타입 구축:
- 레이어 정보: position, scale, rotation, duration, tickPerSecond, keyFramesCount
- 이펙트별: type, 모델명, 텍스처명, 키프레임 배열

---

## 9. DirectX API 호출 지점과 래핑

| 래핑 함수 | DX API | 상황 |
|-----------|--------|------|
| `CShader::Bind_Matrix("g_WorldMatrix")` | `ID3DX11EffectMatrixVariable::SetMatrix` | 이펙트 월드 행렬 |
| `CShader::Bind_RawValue("g_vColor")` | `ID3DX11EffectVariable::SetRawValue` | 색상/글로우 파라미터 |
| `CShader::Bind_RawValue("g_fGlowFactor")` | `ID3DX11EffectVariable::SetRawValue` | 글로우 강도 |
| `CShader::Bind_RawValue("g_Time")` | `ID3DX11EffectVariable::SetRawValue` | 셰이더 시간 변수 |
| `CTexture::Bind_ShaderResource("g_DiffuseTexture")` | `ID3DX11EffectSRVVariable::SetResource` | 디퓨즈/알파 텍스처 |
| `CShader::Begin(passIndex)` | `ID3DX11EffectPass::Apply` | 셰이더 패스 적용 |
| `CModel::Render(meshIndex)` | `DrawIndexed` | 메시 드로우 |
| `CTransform::LookAt(camPos)` | — (수학 연산) | 빌보딩 |

### 이펙트 월드 행렬 합성 경로

```
CEffect::Play_Animation()
 ├─ EffectLocal = CTransform::Get_WorldMatrix()  (키프레임에서 설정된 S/R/T)
 ├─ m_WorldMatrix = EffectLocal × m_LayerMatrix   (레이어 좌표계 적용)
 └─ [빌보딩] → LookAt(CamPosition) 후 m_WorldMatrix 갱신

Render 시:
 → Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)
```

---

## 10. 설계 판단과 채택 가치

### 1) 이펙트 레이어 = 연출 묶음 단위

**구조:** CEffect_Layer가 N개 CEffect를 하나의 단위로 묶어 생성/소멸/시간 관리
**장점:** "폭발" = {화염 이펙트 + 연기 이펙트 + 빛 이펙트}를 하나의 Copy_Layer()로 생성. 레이어 단위로 Following/Position/Rotation 일괄 제어
**채택 가치:** ★★★ — 복합 이펙트 관리의 핵심 추상화

### 2) Clone 체인 (Manager → Layer → Effect → Animation)

**구조:** 프로토타입 레이어를 Clone → 내부 이펙트도 재귀적 Clone
**장점:** 프로토타입 하나로 무한 인스턴스 생성. 각 인스턴스는 독립적 재생 상태
**단점:** Clone 깊이가 깊어 생성 비용 존재
**채택 가치:** ★★★ — 프로토타입 패턴의 교과서적 적용

### 3) 이펙트 전용 키프레임 시스템 (CEffect_Animation)

**구조:** map<uint, KEYFRAME> — 프레임 번호 기반 Lerp 보간
**장점:** 캐릭터 애니메이션(CAnimation)과 독립적인 경량 시스템. 에디터(ImGui)에서 키프레임 추가/삭제 용이
**단점:** 오일러 Lerp는 짐벌락 가능 (이펙트는 대개 문제없음)
**채택 가치:** ★★☆ — 이펙트 특화된 간결한 설계

### 4) 파티클 오브젝트 풀

**구조:** 타입별 vector + Active 토글 + 부족 시 동적 확장
**장점:** 빈번한 파티클 생성/소멸 시 메모리 할당 최소화
**단점:** 풀 축소 없어 최대 사용량만큼 메모리 점유
**채택 가치:** ★★★ — 게임에서 필수적인 파티클 풀링 패턴

### 5) 더블 렌더 패턴 (글로우 + 실제)

**구조:** passIndex를 매 렌더마다 토글 (글로우맵용 → 실제 렌더용)
**장점:** 하나의 오브젝트로 글로우 파이프라인과 최종 렌더 양쪽에 출력
**단점:** 렌더 호출 2배 — 성능 고려 필요
**채택 가치:** ★★☆ — 글로우 파이프라인과의 통합 기법

### 6) Following 모드 (본 매트릭스 추적)

**구조:** COPY_DESC에 본 행렬 포인터 전달 → 매 프레임 자동 갱신
**장점:** 본에 부착된 이펙트(오라, 차지 이펙트 등)가 캐릭터 움직임을 자동 추적
**주의:** 포인터 유효성 — 캐릭터 소멸 시 댕글링 위험
**채택 가치:** ★★★ — 격투게임 이펙트의 핵심 기능

### 7) 에디터 통합 (IMGUI_Shader_Tab)

**구조:** CEffect 생성/Clone 시 ImGui 셰이더 탭에 텍스처 SRV 등록 → 에디터에서 실시간 수정
**장점:** 아티스트가 런타임 중 이펙트 파라미터 조정 가능
**채택 가치:** ★★☆ — 이펙트 에디터 워크플로우의 좋은 사례

---

## 부록: 이펙트 데이터 흐름 요약

```
[에디터/파일]
EFFECT_LAYER_DATA (JSON/바이너리)
 ├─ layerName, position, scale, rotation
 ├─ duration, tickPerSecond, keyFramesCount
 └─ effects[] → effectName, modelName, textureName, type, keyframes[]
      ↓
[로딩 시] CEffect_Manager::Set_Saved_Effects()
 → m_FinalEffects["BurstU-1"] = CEffect_Layer (프로토타입)
      ↓
[전투 중] Copy_Layer("BurstU-1", &copyDesc)
 → Clone → m_UsingEffect (활성 인스턴스)
      ↓
[매 프레임] Update → Play_Effect_Animation → 키프레임 보간 → WorldMatrix 합성
      ↓
[렌더] Late_Update → Add_RenderObject → Render → 셰이더 바인딩 → DrawIndexed
      ↓
[종료] m_bIsDoneAnim → Free() + erase
```

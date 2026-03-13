# PhysX 5.x SDK 기능 레퍼런스

> 작성: 2026-02-26 | PhysX 5.6 기준 (BSD-3 오픈소스, GPU 소스 포함)

---

## 1. CPU 물리 (CUDA 불필요)

### 1-1. Rigid Body (강체 시뮬레이션)

게임 물리의 핵심. 모든 PhysX 기능의 기반.

| 기능 | 설명 | 게임 활용 예시 |
|------|------|---------------|
| PxRigidStatic | 움직이지 않는 강체 | 지형, 벽, 건물 |
| PxRigidDynamic | 물리 시뮬레이션되는 강체 | 상자, 배럴, 물리 오브젝트 |
| Kinematic | 직접 위치 제어 (물리 무시) | 엘리베이터, 움직이는 플랫폼 |
| 슬립/웨이크 | 정지 상태 자동 감지 → CPU 절약 | 쌓여있는 물체들 |

```
물리 오브젝트 → PxRigidDynamic 생성 → Shape 부착 → Scene에 추가
→ simulate() 호출 → 중력/충돌 자동 처리
```

### 1-2. Collision Shapes (충돌 형상)

| Shape | 비용 | 용도 |
|-------|------|------|
| Sphere | 최저 | 총알, 파티클, 대략적 판정 |
| Capsule | 낮음 | 캐릭터, 사지 |
| Box | 낮음 | 상자, 건물 |
| Convex Mesh | 중간 | 불규칙 오브젝트 (최대 256 정점) |
| Triangle Mesh | 높음 | 정적 지형/레벨 (Static 전용) |
| Height Field | 중간 | 넓은 지형 |
| **Custom Geometry** (5.x 신규) | 가변 | 실린더, 복셀 지형 등 사용자 정의 형상 |

### 1-3. Joints (관절/조인트)

두 강체를 물리적으로 연결.

| 조인트 | 설명 | 게임 활용 |
|--------|------|----------|
| Fixed | 완전 고정 | 파괴 가능 오브젝트 연결 |
| Revolute | 경첩 (1축 회전) | 문, 뚜껑 |
| Spherical | 볼소켓 (다축 회전) | 래그돌 어깨/고관절 |
| Prismatic | 슬라이드 (1축 이동) | 서랍, 피스톤 |
| Distance | 최소/최대 거리 유지 | 줄, 체인 |
| **D6** | 6자유도 커스텀 | 범용 (GPU 최적화 대상) |

```
문 구현: PxRevoluteJoint(벽_Actor, 문_Actor) → 회전축/각도 제한 설정
래그돌: 각 신체 부위를 PxRigidDynamic + Spherical/Revolute Joint로 연결
```

### 1-4. Articulation (연결체)

관절 오류 없는 선형 시간 시뮬레이션. 일반 Joint보다 정밀하지만 비용 높음.

| 기능 | 설명 |
|------|------|
| Reduced Coordinate | 트리 구조 링크 체인 (루프 불가) |
| Sensor | 링크에 부착하는 힘/토크 센서 |
| Spatial Tendon | 링크 간 텐던(힘줄) 시뮬레이션 |
| Drive | 모터/스프링 기반 링크 구동 |

**활용**: 로봇 팔, 정밀한 래그돌, 기계 장치, 물리 기반 캐릭터 애니메이션

### 1-5. Character Controller (CCT)

물리 세계에서 캐릭터 이동을 위한 키네마틱 컨트롤러.

| 기능 | 설명 |
|------|------|
| Capsule/Box Controller | 캐릭터 충돌 형상 |
| 경사면 제한 | 최대 오르막 각도 설정 |
| 계단 오프셋 | 자동 계단 오르기 |
| 이동 플랫폼 탑승 | `eCCT_CAN_RIDE_ON_OBJECT` |
| 충돌 콜백 | 벽/바닥/천장 충돌 이벤트 |

```
캐릭터: PxController 생성 → move(displacement) 호출
  → 벽 슬라이딩, 경사면 제한, 계단 자동 처리
```

### 1-6. Vehicle (차량 시뮬레이션)

PhysX Extensions에 포함된 차량 물리.

| 기능 | 설명 |
|------|------|
| 서스펜션 | 스프링/댐퍼 모델 |
| 타이어 마찰 | 노면별 그립 차이 |
| 엔진/기어 | 토크 커브, 변속 시뮬레이션 |
| 안티롤바 | 롤링 안정화 |
| 탱크 구동 | 좌우 트랙 독립 제어 |

### 1-7. Scene Query (공간 질의)

물리 씬에 대한 질의. 게임 로직에서 가장 자주 사용.

| 질의 | 설명 | 게임 활용 |
|------|------|----------|
| **Raycast** | 광선을 쏴서 충돌점 검출 | 총 발사, 시야 판정, 피킹 |
| **Sweep** | 형상을 이동시키며 충돌 검출 | 캐릭터 이동 판정, 투사체 |
| **Overlap** | 영역 내 오브젝트 검출 | 범위 공격, 트리거 영역 |

```
레이캐스트: PxScene::raycast(origin, dir, maxDist, &hitBuffer)
  → hitBuffer에 충돌 Actor, 충돌 지점, 법선 정보 반환

필터링: Touching(관통하며 기록) vs Blocking(막힘) 구분 가능
배치 질의: PxBatchQueryExt로 여러 질의를 묶어서 실행
```

### 1-8. CCD (연속 충돌 감지)

빠른 물체가 얇은 벽을 뚫고 지나가는 "터널링" 방지.

| 방식 | 설명 |
|------|------|
| Sweep-based CCD | 프레임 간 궤적을 스윕하여 검출 (정밀) |
| Speculative CCD | 다음 프레임 위치를 예측하여 검출 (빠름) |
| Sweep + Speculative | 5.x에서 동시 사용 가능 (권장) |
| Raycast CCD (Ext) | 간단한 레이캐스트 기반 (가벼움) |

---

## 2. GPU 물리 (CUDA 필요)

### 2-1. GPU Rigid Body

CPU와 동일한 강체를 **GPU에서 병렬 처리**. 대량 오브젝트에 효과적.

| 항목 | 세부 |
|------|------|
| 지원 범위 | 전체 강체 파이프라인 + Articulation |
| D6 Joint | GPU 네이티브 (최고 성능) |
| 기타 Joint | 셰이더는 CPU, 솔버는 GPU |
| 제약 | Convex Hull 64정점/32정점-per-face 이내 권장 |

**활용**: 수백~수천 개 물리 오브젝트 동시 시뮬레이션 (건물 붕괴, 대량 파편)

### 2-2. Soft Body (FEM 변형체)

유한요소법(Finite Element Method) 기반 변형 시뮬레이션.

| 항목 | 세부 |
|------|------|
| 메시 구조 | 시뮬레이션 메시(사면체) + 충돌 메시 이중 구조 |
| 재질 속성 | Young's Modulus(강성), 동적 마찰, Poisson Ratio |
| 양방향 커플링 | 강체 ↔ 변형체 상호작용 |

**활용**: 젤리, 고무, 생체 조직, 변형 가능한 오브젝트

### 2-3. PBD Particle System (입자 시스템)

Position-Based Dynamics 기반 통합 솔버.

| 기능 | 설명 | 게임 활용 |
|------|------|----------|
| Fluid | 유체 시뮬레이션 (SPH) | 물, 용암, 피 |
| Granular | 입자 재질 | 모래, 자갈, 눈 |
| Cloth | 메시 기반 천 | 망토, 깃발, 텐트 |
| Inflatable | 압력 기반 팽창체 | 풍선, 타이어 |
| Rope | 1D 메시 시뮬레이션 | 밧줄, 전선 |

```
천 시뮬레이션: 메시 → PBD Particle로 변환 → 제약조건(거리, 굽힘) 설정
  → 양방향 커플링으로 강체와 상호작용
```

### 2-4. SDF Collision (Signed Distance Field)

복셀화된 거리 필드로 충돌 감지. Convex 분해 없이 복잡한 형상 처리.

**활용**: 기어, 캠, 복잡한 기계 부품의 정밀 충돌

---

## 3. 확장 라이브러리

### 3-1. Blast (파괴/파쇄)

물리와 독립된 파괴 시스템. PhysX 없이도 사용 가능.

| 레이어 | 설명 |
|--------|------|
| NvBlast (Low-Level) | C-style API, 메모리 할당 없음, 스테이트리스 |
| NvBlastTk (Toolkit) | 이벤트 시스템, Joint 관리 |
| NvBlastExt | PhysX 연동, 직렬화, 스트레스 솔버 |

| 기능 | 설명 |
|------|------|
| 계층적 파쇄 | 청크를 여러 단계로 분할 |
| 스트레스 기반 파괴 | 힘에 의한 자연스러운 균열 전파 |
| 충격 전달 | 강한 충격 → 파편이 빠르게 튀어나감 |
| 커스텀 데미지 셰이더 | 사용자 정의 파괴 로직 |

**활용**: 벽 부수기, 건물 붕괴, 파괴 가능 환경

### 3-2. Flow (연소/연기/화염)

가스 유체 시뮬레이션. GPU 가속.

| 기능 | 설명 |
|------|------|
| 연소 시뮬레이션 | 가연성 유체, 불꽃 전파 |
| 연기 | 밀도 기반 확산 |
| 양력/항력 | 열기류에 의한 상승 |

**활용**: 화염 이펙트, 폭발 연기, 횃불

---

## 4. 게임 기능별 PhysX 매핑

| 게임 기능 | 사용할 PhysX 기능 | CPU/GPU |
|-----------|-------------------|---------|
| 캐릭터 이동/충돌 | CCT + Raycast | CPU |
| 래그돌 | RigidDynamic + Joint | CPU |
| 물리 오브젝트 (상자, 배럴) | RigidDynamic | CPU |
| 총/스킬 판정 | Raycast / Sweep | CPU |
| 범위 공격/트리거 | Overlap | CPU |
| 문/스위치/장치 | Joint (Revolute, Prismatic) | CPU |
| 차량 | Vehicle Extension | CPU |
| 터널링 방지 | CCD | CPU |
| 건물/지형 파괴 | **Blast** + RigidDynamic | CPU |
| 대량 파편 | GPU RigidBody | **GPU** |
| 망토/깃발 | PBD Cloth | **GPU** |
| 물/용암 | PBD Fluid | **GPU** |
| 변형 오브젝트 | FEM Soft Body | **GPU** |
| 화염/연기 | **Flow** | **GPU** |

---

## 5. 도입 우선순위 권장

```
[1단계] RigidBody + Shape + Scene Query
         → 기본 물리, 충돌, 레이캐스트 (모든 게임의 기반)

[2단계] CCT + Joint
         → 캐릭터 컨트롤러, 래그돌, 상호작용 오브젝트

[3단계] CCD + Vehicle (필요 시)
         → 빠른 투사체 처리, 차량 물리

[4단계] Blast
         → 파괴 가능 환경

[5단계] GPU 물리 (Cloth, Fluid, Soft Body, Flow)
         → 컴퓨트 셰이더 학습 후 도전
```

---

## 참고

- [PhysX SDK 공식 페이지](https://developer.nvidia.com/physx-sdk)
- [PhysX 5.5.1 문서](https://nvidia-omniverse.github.io/PhysX/physx/5.5.1/)
- [Scene Queries](https://nvidia-omniverse.github.io/PhysX/physx/5.5.0/docs/SceneQueries.html)
- [CCD (Advanced Collision Detection)](https://nvidia-omniverse.github.io/PhysX/physx/5.5.0/docs/AdvancedCollisionDetection.html)
- [Blast SDK 문서](https://nvidia-omniverse.github.io/PhysX/blast/index.html)
- [PhysX GitHub (NVIDIA-Omniverse)](https://github.com/NVIDIA-Omniverse/PhysX)
- [PhysX Wikipedia](https://en.wikipedia.org/wiki/PhysX)

# O05-A: Collider + Bounding 교차 판정

## 1. 전체 구조

```
CCollider (CComponent)         ← 충돌 컴포넌트 (프로토타입/클론)
  └── CBounding (CBase)        ← 추상 바운딩 볼륨
        ├── CBounding_AABB     ← 축 정렬 박스 (BoundingBox)
        ├── CBounding_OBB      ← 방향 박스 (BoundingOrientedBox)
        └── CBounding_Sphere   ← 구체 (BoundingSphere)
```

**전략 패턴**: CCollider는 COLLIDER enum에 따라 다른 CBounding 파생 클래스를 생성한다.
교차 판정 로직은 각 CBounding이 담당한다.

---

## 2. CCollider — 충돌 컴포넌트

### 프로토타입: COLLIDER 타입 설정

```cpp
HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
    m_eType = eType;  // AABB, OBB, SPHERE

#ifdef _DEBUG
    // 디버그 렌더링용 DirectXTK 리소스 생성
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);
    m_pEffect->SetVertexColorEnabled(true);

    // BasicEffect에서 셰이더 바이트코드 추출 → InputLayout 생성
    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);
    m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, ...);
#endif
}
```

### 클론: DESC에 따라 Bounding 생성

```cpp
HRESULT CCollider::Initialize(void* pArg)
{
    CBounding::BOUNDING_DESC* pDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

    switch (m_eType) {
    case COLLIDER::AABB:
        m_pBounding = CBounding_AABB::Create(pDevice, pContext, pDesc);
        break;
    case COLLIDER::OBB:
        m_pBounding = CBounding_OBB::Create(pDevice, pContext, pDesc);
        break;
    case COLLIDER::SPHERE:
        m_pBounding = CBounding_Sphere::Create(pDevice, pContext, pDesc);
        break;
    }
}
```

**프로토타입은 타입만 결정**, 클론 시 DESC로 크기/위치를 설정한다.
같은 AABB 프로토타입에서 다양한 크기의 충돌체를 클론할 수 있다.

### Update + Intersect

```cpp
// 매 프레임 월드 행렬로 바운딩 갱신
void CCollider::Update(_fmatrix WorldMatrix) {
    m_pBounding->Update(WorldMatrix);
}

// 다른 충돌체와 교차 검사
_bool CCollider::Intersect(CCollider* pCollider) {
    return m_isColl = m_pBounding->Intersect(
        pCollider->m_eType, pCollider->m_pBounding);
}
```

---

## 3. CBounding 추상 클래스

```cpp
class CBounding abstract : public CBase {
public:
    typedef struct tagBoundingDesc {
        _float3 vCenter;           // 로컬 오프셋
    } BOUNDING_DESC;

    virtual _bool Intersect(COLLIDER eType, CBounding* pBounding) = 0;
    virtual void Update(_fmatrix WorldMatrix) = 0;
#ifdef _DEBUG
    virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch,
        _fvector vColor) = 0;
#endif
};
```

**Original + Desc 패턴**: 모든 파생 클래스가 2개의 디스크립터를 가진다:
- `m_pOriginalDesc`: 로컬 공간 원본 (초기화 시 고정)
- `m_pDesc`: 월드 공간 변환 결과 (매 프레임 갱신)

---

## 4. CBounding_AABB — 축 정렬 박스

### DESC 구조

```cpp
struct AABB_DESC : public BOUNDING_DESC {
    _float3 vSize;  // 전체 크기 (Extents = Size * 0.5)
};
```

### 초기화

```cpp
m_pOriginalDesc = new BoundingBox(
    pDesc->vCenter,                                        // 중심
    _float3(pDesc->vSize.x*0.5f, vSize.y*0.5f, vSize.z*0.5f)  // 반크기(Extents)
);
m_pDesc = new BoundingBox(*m_pOriginalDesc);
```

### Update — 회전 제거

```cpp
void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
    _matrix TransformMatrix = WorldMatrix;

    // 회전을 제거하고 스케일만 유지 (축 정렬 유지)
    TransformMatrix.r[0] = XMVectorSet(1,0,0,0) * XMVector3Length(WorldMatrix.r[0]);
    TransformMatrix.r[1] = XMVectorSet(0,1,0,0) * XMVector3Length(WorldMatrix.r[1]);
    TransformMatrix.r[2] = XMVectorSet(0,0,1,0) * XMVector3Length(WorldMatrix.r[2]);

    m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}
```

**핵심**: AABB는 항상 축에 정렬되어야 하므로 월드 행렬에서 **회전 성분을 제거**한다.
`Length(행벡터)` = 스케일을 추출하고, 단위 축 벡터에 곱한다.

```
원본 WorldMatrix:           변환된 TransformMatrix:
[Rx  Ry  Rz  0]            [Sx  0   0   0]
[Ux  Uy  Uz  0]     →      [0   Sy  0   0]
[Lx  Ly  Lz  0]            [0   0   Sz  0]
[Tx  Ty  Tz  1]            [Tx  Ty  Tz  1]
```

### AABB vs AABB — 수동 구현

```cpp
_bool CBounding_AABB::Intersect_AABB(CBounding_AABB* pTarget)
{
    _float3 vSourMin = Compute_Min();  // Center - Extents
    _float3 vSourMax = Compute_Max();  // Center + Extents
    _float3 vDestMin = pTarget->Compute_Min();
    _float3 vDestMax = pTarget->Compute_Max();

    // 각 축에서 겹치지 않으면 분리
    if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x)) return false;
    if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y)) return false;
    if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z)) return false;

    return true;
}
```

**분리 축 정리(SAT)의 AABB 특화 버전**: 3개 좌표축에서 구간이 겹치면 충돌.

---

## 5. CBounding_OBB — 방향 박스

### DESC 구조

```cpp
struct OBB_DESC : public BOUNDING_DESC {
    _float3 vSize;     // 전체 크기
    _float3 vRadians;  // 초기 회전 (Euler → Quaternion 변환)
};
```

### 초기화 — 쿼터니언 회전

```cpp
XMStoreFloat4(&vQuaternion,
    XMQuaternionRotationRollPitchYaw(vRadians.x, vRadians.y, vRadians.z));

m_pOriginalDesc = new BoundingOrientedBox(
    pDesc->vCenter,
    _float3(vSize.x*0.5f, vSize.y*0.5f, vSize.z*0.5f),
    vQuaternion);
```

### Update — 회전 포함 변환

```cpp
void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
    // AABB와 달리 회전을 그대로 적용!
    m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}
```

### OBB vs OBB — SAT (분리 축 정리) 수동 구현

```cpp
struct tagOBB {
    _float3 vCenter;        // 중심
    _float3 vCenterDir[3];  // 중심→면중점 벡터 (반크기 × 방향)
    _float3 vAlignDir[3];   // 정규화된 축 방향
};
```

**Compute_OBB**: `GetCorners()`로 8개 꼭짓점을 얻고, 인접 꼭짓점 차이로 3개 축 벡터를 계산.

```cpp
// 꼭짓점 4,5,7,0으로부터 3축 산출
vCenterDir[0] = (vPoints[5] - vPoints[4]) * 0.5f;  // Right
vCenterDir[1] = (vPoints[7] - vPoints[4]) * 0.5f;  // Up
vCenterDir[2] = (vPoints[0] - vPoints[4]) * 0.5f;  // Look
vAlignDir[i]  = Normalize(vCenterDir[i]);           // 정규화
```

**SAT 알고리즘 (6축 검사)**:

```cpp
for (i = 0..1)           // 두 OBB 각각의
  for (j = 0..2)         // 3개 축에 대해
  {
    fLength[0] = |Dot(Center2-Center1, AlignDir[i][j])|;  // 중심 거리 투영
    fLength[1] = Sum(|Dot(OBB1.CenterDir[k], AlignDir)|); // OBB1 반크기 투영
    fLength[2] = Sum(|Dot(OBB2.CenterDir[k], AlignDir)|); // OBB2 반크기 투영

    if (fLength[0] > fLength[1] + fLength[2])
        return false;  // 분리축 발견 → 충돌 없음
  }
return true;  // 분리축 없음 → 충돌
```

**참고**: 완전한 OBB-OBB SAT는 15축(3+3+9)이 필요하지만,
이 구현은 **6축(3+3)**만 검사한다. 교차곱 축(9축)은 생략되어 있다.

---

## 6. CBounding_Sphere — 구체

### DESC 구조

```cpp
struct SPHERE_DESC : public BOUNDING_DESC {
    _float fRadius;
};
```

### 초기화 + Update

```cpp
m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);

void CBounding_Sphere::Update(_fmatrix WorldMatrix) {
    m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}
```

BoundingSphere::Transform은 이동 + 균일 스케일을 적용한다.

### 교차 판정 — DirectXCollision 위임

```cpp
_bool CBounding_Sphere::Intersect(COLLIDER eType, CBounding* pBounding)
{
    switch (eType) {
    case COLLIDER::AABB:
        return m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pBounding)->Get_Desc());
    case COLLIDER::OBB:
        return m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pBounding)->Get_Desc());
    case COLLIDER::SPHERE:
        return m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pBounding)->Get_Desc());
    }
}
```

**Sphere vs X**: DirectXCollision 라이브러리의 `Intersects()`에 완전 위임.

---

## 7. 교차 판정 조합 매트릭스

| | AABB | OBB | Sphere |
|---|---|---|---|
| **AABB** | 수동 Min/Max | DXCollision | DXCollision |
| **OBB** | DXCollision | 수동 SAT(6축) | DXCollision |
| **Sphere** | DXCollision | DXCollision | DXCollision |

- **수동 구현**: AABB-AABB, OBB-OBB (학습 목적)
- **라이브러리 위임**: 나머지 조합 (DirectXCollision)

---

## 8. 디버그 렌더링

```cpp
#ifdef _DEBUG
HRESULT CCollider::Render()
{
    // BasicEffect로 View/Projection 설정
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJECTION));
    m_pEffect->Apply(m_pContext);

    m_pContext->IASetInputLayout(m_pInputLayout);
    m_pContext->GSSetShader(nullptr, nullptr, 0);  // GS 해제

    m_pBatch->Begin();
    // 충돌 시 빨간색, 미충돌 시 초록색
    m_pBounding->Render(m_pBatch,
        m_isColl ? XMVectorSet(1,0,0,1) : XMVectorSet(0,1,0,1));
    m_pBatch->End();
}
#endif
```

DirectXTK의 `DX::Draw()`로 와이어프레임 박스/구체를 그린다.
**PrimitiveBatch + BasicEffect** 조합은 디버그 용도 전용.

### Clone 시 디버그 리소스 공유

```cpp
CCollider::CCollider(const CCollider& Prototype)
    : m_eType { Prototype.m_eType }
    , m_pBatch { Prototype.m_pBatch }       // 공유
    , m_pEffect { Prototype.m_pEffect }     // 공유
    , m_pInputLayout { Prototype.m_pInputLayout }  // AddRef
{
    Safe_AddRef(m_pInputLayout);
}
```

**PrimitiveBatch와 BasicEffect**는 원본만 소유 (`m_isCloned` 체크로 원본만 delete).
InputLayout은 COM 객체이므로 AddRef/Release.

---

## 9. 핵심 정리

| 항목 | 설명 |
|------|------|
| **전략 패턴** | CCollider가 COLLIDER enum으로 CBounding 파생 클래스 선택 |
| **Original + Desc** | 로컬 원본 고정, 매 프레임 월드 변환 결과만 갱신 |
| **AABB Update** | 회전 제거, 스케일+이동만 적용 (축 정렬 유지) |
| **OBB Update** | 회전 포함 전체 변환 적용 |
| **AABB-AABB** | 3축 구간 겹침 검사 (수동) |
| **OBB-OBB** | SAT 6축 투영 (수동, 9축 교차곱은 생략) |
| **기타 조합** | DirectXCollision 라이브러리 위임 |
| **디버그** | PrimitiveBatch로 와이어프레임, 충돌=빨강/비충돌=초록 |

# O04. 지오메트리 & 모델 시스템

## 1. CVIBuffer 계층 구조

```
CVIBuffer (abstract, CComponent 상속)
├── CVIBuffer_Rect       ← 2D 사각형 (UI, 풀스크린 쿼드)
├── CVIBuffer_Cube       ← 큐브 메시 (스카이박스)
├── CVIBuffer_Terrain    ← 높이맵 지형 (QuadTree 컬링)
├── CVIBuffer_Cell       ← 내비메시 셀 (디버그 시각화)
├── CMesh                ← Assimp 모델의 서브메시
└── CVIBuffer_Instancing (abstract)
    ├── CVIBuffer_Rect_Instancing  ← 인스턴싱 사각형 (파티클)
    └── CVIBuffer_Point_Instancing ← 인스턴싱 포인트 (파티클)
```

### CVIBuffer 기본 멤버
```cpp
ID3D11Buffer*  m_pVB;              // 버텍스 버퍼
ID3D11Buffer*  m_pIB;              // 인덱스 버퍼
_uint          m_iNumVertices;     // 정점 수
_uint          m_iVertexStride;    // 정점 크기 (바이트)
_uint          m_iNumIndices;      // 인덱스 수
DXGI_FORMAT    m_eIndexFormat;     // 인덱스 포맷 (16/32bit)
D3D_PRIMITIVE_TOPOLOGY m_ePrimitive; // 삼각형 리스트 등
_float3*       m_pVertexPositions; // CPU측 정점 위치 (피킹용)
```

### DX9 → DX11 버퍼 비교
| DX9 | DX11 |
|-----|------|
| `CreateVertexBuffer` → Lock/Unlock | `CreateBuffer` → D3D11_SUBRESOURCE_DATA |
| `SetStreamSource` | `IASetVertexBuffers` |
| `SetIndices` | `IASetIndexBuffer` |
| `DrawIndexedPrimitive` | `DrawIndexed` / `DrawIndexedInstanced` |

---

## 2. 정점 포맷 (Engine_Struct.h)

### 기본 정점
| 구조체 | 구성 | 용도 |
|--------|------|------|
| VTXPOS | Position | 단순 위치 (셀 시각화) |
| VTXPOSTEX | Position + UV | 2D 사각형, UI |
| VTXCUBE | Position + UV3D | 큐브맵 (스카이박스) |
| VTXNORTEX | Position + Normal + UV | 지형 |
| VTXMESH | Position + Normal + UV + Tangent + Binormal | 정적 3D 모델 |
| VTXSKINMESH | VTXMESH + BlendIndex + BlendWeight | 스키닝 애니메이션 모델 |

### 인스턴싱 정점
| 구조체 | 구성 | 용도 |
|--------|------|------|
| VTXINSTANCEPARTICLE | Right/Up/Look/Translation + LifeTime | 인스턴스 데이터 |
| VTXPOSTEX_INSTANCEPARTICLE | 기본정점 + 인스턴스 행렬 + LifeTime | Rect 인스턴싱 |
| VTXPOS_INSTANCEPARTICLE | 기본정점 + World행렬 + LifeTime | Point 인스턴싱 |

### DX11 Input Element 자기선언 패턴
```cpp
struct VTXNORTEX {
    XMFLOAT3 vPosition, vNormal;
    XMFLOAT2 vTexcoord;
    static const _uint iNumElements = { 3 };
    static constexpr D3D11_INPUT_ELEMENT_DESC Elements[3] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, ... },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, ... },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, ... }
    };
};
```
> 각 정점 구조체가 자신의 InputLayout 정보를 static으로 보유 → 셰이더 생성 시 전달.

---

## 3. CModel - Assimp 기반 3D 모델

### 구조
```
CModel (CComponent)
├── m_pAIScene (const aiScene*)     ← Assimp 씬 (파일에서 로드)
├── m_Importer (Assimp::Importer)   ← 임포터 (씬 수명 관리)
├── m_eType (MODELTYPE)             ← ANIM / NONANIM
├── m_PreTransformMatrix            ← 사전 변환 (스케일/회전 보정)
├── m_Meshes (vector<CMesh*>)       ← 서브메시 배열
├── m_Materials (vector<CMaterial*>)← 재질 배열
├── m_Bones (vector<CBone*>)        ← 본 계층 (스켈레탈)
└── m_Animations (vector<CAnimation*>) ← 애니메이션 (ANIM 전용)
```

### 초기화 흐름 (Initialize_Prototype)
```
1. m_Importer.ReadFile(pModelFilePath, 플래그)
   플래그: aiProcess_PreTransformVertices (NONANIM만)
          aiProcess_ConvertToLeftHanded (DX 좌표계 변환)
2. Ready_Bones()     ← aiNode 트리 재귀 순회 → CBone 배열 생성
3. Ready_Meshes()    ← aiMesh마다 CMesh 생성
4. Ready_Materials() ← aiMaterial마다 CMaterial 생성 (텍스처 로드)
5. Ready_Animations()← aiAnimation마다 CAnimation 생성 (ANIM만)
```

### 렌더링 흐름
```cpp
// Client에서 메시 수만큼 반복
for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++) {
    m_pModelCom->Bind_ShaderResource(i, pShader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
    m_pModelCom->Bind_BoneMatrices(i, pShader, "g_BoneMatrices");  // ANIM만
    pShader->Begin(iPassIndex);
    m_pModelCom->Render(i);
}
```

### 소켓 본 (파츠 부착용)
```cpp
const _float4x4* Get_SocketBoneMatrix_Ptr(const _char* pBoneName) const;
// → CContainerObject가 특정 본의 변환 행렬 포인터를 CPartObject에 전달
// → 무기가 손 본을 따라 움직임
```

---

## 4. CMesh - 서브메시

### 정적 모델 (NONANIM)
```cpp
Ready_VertexBuffer_For_NonAnim(aiMesh, PreTransformMatrix):
  → VTXMESH 정점 생성 (Position, Normal, UV, Tangent, Binormal)
  → PreTransformMatrix 적용 (스케일/회전 보정)
```

### 애니메이션 모델 (ANIM)
```cpp
Ready_VertexBuffer_For_Anim(aiMesh, Bones):
  → VTXSKINMESH 정점 생성 (+ BlendIndex, BlendWeight)
  → 각 정점이 최대 4개 본의 영향을 받음
  → m_BoneIndices: 이 메시에 영향을 주는 본들의 전역 인덱스
  → m_OffsetMatrices: 각 본의 오프셋 행렬 (바인드포즈 역행렬)
```

### 본 행렬 바인딩
```cpp
Bind_BoneMatrices(pShader, "g_BoneMatrices", Bones):
  for each bone in m_BoneIndices:
    m_BoneMatrices[i] = OffsetMatrix × Bones[index].CombinedTransform
  → 최대 512개 본 (g_iMaxNumBones) 셰이더에 전달
```

---

## 5. CMaterial - 재질 (텍스처 관리)

```cpp
vector<ID3D11ShaderResourceView*> m_Textures[AI_TEXTURE_TYPE_MAX];
// aiTextureType: DIFFUSE, SPECULAR, NORMALS, HEIGHT 등
// 하나의 타입에 여러 텍스처 가능 (멀티 텍스처)
```

### 초기화
```
aiMaterial에서 각 텍스처 타입별 경로 추출
→ WICTextureLoader / DDSTextureLoader로 SRV 생성
```

---

## 6. CTexture - 독립 텍스처 컴포넌트

```cpp
vector<ID3D11ShaderResourceView*> m_Textures;  // 텍스처 배열
// 파일 경로에 %d 포맷 → 번호 순서대로 복수 텍스처 로드
```

### DX9 → DX11 텍스처 비교
```
DX9: IDirect3DTexture9 → SetTexture(stage, pTexture)
DX11: ID3D11ShaderResourceView → Bind_ShaderResource(pShader, name, index)
     셰이더 변수명으로 바인딩 (스테이지 번호가 아닌 이름 기반)
```

---

## 7. GPU 인스턴싱 시스템

### 개념
```
일반 렌더: Draw 호출 N번 (오브젝트마다 1번)
인스턴싱: Draw 호출 1번 (N개 인스턴스 한꺼번에)
→ 같은 메시를 다른 위치에 대량 배치 시 성능 향상 (파티클, 풀, 나무 등)
```

### CVIBuffer_Instancing 구조
```cpp
ID3D11Buffer* m_pVBInstance;    // 인스턴스 전용 버텍스 버퍼 (동적)
_uint m_iInstanceVertexStride;  // 인스턴스 데이터 크기
_uint m_iNumInstance;           // 인스턴스 수
_uint m_iIndexCountPerInstance; // 인스턴스당 인덱스 수
```

### 듀얼 버텍스 버퍼 바인딩
```cpp
Bind_Buffers():
  // 슬롯 0: 기본 버텍스 (Position, UV 등)
  // 슬롯 1: 인스턴스 데이터 (World행렬, LifeTime 등)
  IASetVertexBuffers(0, 2, {m_pVB, m_pVBInstance}, ...);

Render():
  DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, ...);
```

### 파티클 동작 (Rect/Point Instancing)
```cpp
INSTANCE_DESC {
    _uint  iNumInstance;  // 파티클 수
    _float2 vScale;       // 크기 범위
    _float3 vCenter;      // 중심 위치
    _float3 vRange;       // 분산 범위
};
// + vSpeed, vLifeTime, isLoop, vPivot (파티클 전용)

Drop(fTimeDelta):   // 아래로 떨어지는 파티클 (눈, 비)
Spread(fTimeDelta): // 중심에서 퍼지는 파티클 (폭발)
```

---

## 8. CVIBuffer_Terrain + CQuadTree

### 지형 생성
```cpp
Initialize_Prototype(pHeightMapFilePath):
  1. 높이맵 이미지 로드 → 픽셀값으로 Y좌표 결정
  2. VTXNORTEX 정점 생성 (Position, Normal, UV)
  3. 인덱스 버퍼 생성 (삼각형 리스트)
  4. CQuadTree 생성 → 정점 인덱스 기반 공간 분할
```

### QuadTree 컬링
```cpp
Culling(_fmatrix WorldMatrix):
  1. 절두체를 지형 로컬 공간으로 변환
  2. QuadTree 재귀 순회 → 절두체 내 노드만 인덱스 수집
  3. 인덱스 버퍼를 가시 삼각형만으로 갱신
  → 매 프레임 보이는 영역만 렌더링
```

### QuadTree 구조
```
CQuadTree
├── m_pChildren[4] (LT, RT, RB, LB)    ← 4분할 자식 노드
├── m_iCenterIndex                       ← 중심 정점 인덱스
├── m_iCornerIndices[4]                  ← 모서리 정점 인덱스
├── m_pNeighbors[4]                      ← 인접 노드 (LOD 크랙 방지)
└── isDraw(): 절두체 판정 → 가시 여부 결정
```

> **현재 프로젝트와 차이**: 현재는 지형 전체를 항상 렌더링. 참고에서는 QuadTree로 절두체 컬링 → 대규모 지형 최적화.

---

## 9. 현재 → 참고 지오메트리 비교

| 항목 | 현재 (DX9) | 참고 (DX11) |
|------|-----------|------------|
| 버텍스 포맷 | FVF 플래그 | **InputElement 자기선언** |
| 버퍼 생성 | Lock/Unlock | **D3D11_SUBRESOURCE_DATA** |
| 텍스처 바인딩 | 스테이지 번호 | **셰이더 변수명** |
| 3D 모델 | X파일 / 자체 | **Assimp** (FBX, OBJ 등) |
| 재질 시스템 | 단순 텍스처 | **CMaterial** (Diffuse+Normal+Specular) |
| 인스턴싱 | 없음 | **GPU 인스턴싱** (파티클) |
| 지형 최적화 | 없음 | **QuadTree 컬링** |
| 스키닝 | 없음 | **본 행렬 배열 바인딩** (최대 512본) |

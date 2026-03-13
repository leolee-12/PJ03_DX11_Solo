# O03-B: Shader Effects11 컴파일 / 바인딩

## 1. Effects11 프레임워크 개요

### DX9 vs DX11 셰이더 관리 비교

| 항목 | DX9 | DX11 (Effects11) |
|------|-----|-------------------|
| **셰이더 파일** | .fx (FX 프레임워크) | .hlsl (Effects11 래핑) |
| **컴파일** | D3DXCreateEffectFromFile | D3DX11CompileEffectFromFile |
| **변수 바인딩** | GetParameterByName | GetVariableByName |
| **패스 실행** | BeginPass/EndPass | Apply(0, pContext) |
| **입력 레이아웃** | FVF 또는 정점 선언 | ID3D11InputLayout |

### Effects11이란?

DX11에서는 순수 HLSL + ID3D11VertexShader/PixelShader를 개별 생성하는 것이 표준이다.
하지만 **Effects11** (fx11)은 DX9의 FX 프레임워크와 유사한 편의성을 제공하는
**오픈소스 래퍼 라이브러리**이다:

```
ID3DX11Effect        ← .hlsl 파일 전체를 하나의 Effect 객체로 관리
  └── Technique      ← 렌더링 기법 (보통 1개)
        └── Pass     ← 렌더링 패스 (VS + PS 조합)
              ├── VertexShader
              └── PixelShader
```

**장점**: 하나의 .hlsl 파일에 여러 패스를 정의하고, 변수를 이름으로 바인딩 가능
**단점**: 런타임 컴파일 의존, 최적화 제약, MS 더 이상 공식 지원하지 않음

---

## 2. CShader 클래스 구조

```cpp
class CShader final : public CComponent {
    ID3DX11Effect*              m_pEffect;       // Effects11 핵심 객체
    _uint                       m_iNumPasses;    // 패스 개수
    vector<ID3D11InputLayout*>  m_InputLayouts;  // 패스별 입력 레이아웃
};
```

### 프로토타입 / 클론 패턴

```cpp
// 프로토타입 생성 (원본)
CShader::Create(pDevice, pContext, filePath, Elements, iNumElements)
    → Initialize_Prototype()  // .hlsl 컴파일 + InputLayout 생성

// 클론 (복제)
CShader::Clone(pArg)
    → CShader(const CShader& Prototype)  // 복사 생성자
    → Initialize(pArg)                    // 빈 함수
```

**복사 생성자에서의 공유:**

```cpp
CShader::CShader(const CShader& Prototype)
    : CComponent{ Prototype }
    , m_pEffect { Prototype.m_pEffect }          // Effect 공유!
    , m_iNumPasses { Prototype.m_iNumPasses }
    , m_InputLayouts { Prototype.m_InputLayouts } // InputLayout도 공유!
{
    for (auto& pInputLayout : m_InputLayouts)
        Safe_AddRef(pInputLayout);   // 참조 카운트 증가
    Safe_AddRef(m_pEffect);           // 참조 카운트 증가
}
```

**중요**: Shader는 Clone 시 새 리소스를 생성하지 않고 **Effect와 InputLayout을 공유**한다.
이는 셰이더가 상태를 보유하지 않는 불변 리소스이기 때문에 안전하다.
(바인딩은 매 프레임 덮어쓰므로 공유해도 문제없음)

---

## 3. 셰이더 컴파일 — Initialize_Prototype

```cpp
HRESULT CShader::Initialize_Prototype(
    const _tchar* pShaderFilePath,
    const D3D11_INPUT_ELEMENT_DESC* pElements,
    _uint iNumElements)
{
    // ① 컴파일 플래그 설정
    _uint iHlslFlag = {};
#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

    // ② .hlsl 파일 컴파일 → ID3DX11Effect 생성
    D3DX11CompileEffectFromFile(
        pShaderFilePath,
        nullptr,                         // 매크로 정의 없음
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // #include 지원
        iHlslFlag,
        0,
        m_pDevice,
        &m_pEffect,
        nullptr                          // 에러 메시지 (주석 처리됨)
    );

    // ③ Technique 0의 패스 수 확인
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    D3DX11_TECHNIQUE_DESC TechniqueDesc{};
    pTechnique->GetDesc(&TechniqueDesc);
    m_iNumPasses = TechniqueDesc.Passes;

    // ④ 패스별 InputLayout 생성
    for (size_t i = 0; i < m_iNumPasses; i++)
    {
        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
        D3DX11_PASS_DESC PassDesc{};
        pPass->GetDesc(&PassDesc);

        ID3D11InputLayout* pInputLayout = nullptr;
        m_pDevice->CreateInputLayout(
            pElements, iNumElements,
            PassDesc.pIAInputSignature,      // VS의 입력 시그니처
            PassDesc.IAInputSignatureSize,
            &pInputLayout
        );
        m_InputLayouts.push_back(pInputLayout);
    }
}
```

### 컴파일 플래그 상세

| 플래그 | 모드 | 효과 |
|--------|------|------|
| `D3DCOMPILE_DEBUG` | Debug | 셰이더 디버깅 정보 포함 |
| `D3DCOMPILE_SKIP_OPTIMIZATION` | Debug | 최적화 건너뜀 (빠른 컴파일) |
| `D3DCOMPILE_OPTIMIZATION_LEVEL1` | Release | 기본 최적화 적용 |

### InputLayout 생성 원리

InputLayout은 **정점 구조체의 메모리 레이아웃**과 **셰이더의 입력 시그니처**를
연결하는 매핑 객체이다:

```
C++ 정점 구조체 (메모리)         HLSL 셰이더 (입력)
┌─────────────────────┐         ┌──────────────────┐
│ XMFLOAT3 vPosition  │ ──────> │ float3 POSITION   │
│ XMFLOAT3 vNormal    │ ──────> │ float3 NORMAL     │
│ XMFLOAT2 vTexcoord  │ ──────> │ float2 TEXCOORD   │
└─────────────────────┘         └──────────────────┘
      D3D11_INPUT_ELEMENT_DESC로 매핑 정보 정의
```

**패스별로 InputLayout이 다른 이유**: 각 패스의 VS가 다른 입력을 요구할 수 있다.
같은 정점 포맷이라도 VS의 시그니처가 다르면 별도의 InputLayout이 필요하다.

---

## 4. 자기 선언형 정점 구조체

이 프레임워크의 독특한 설계: **정점 구조체가 자신의 InputLayout을 선언**한다.

```cpp
typedef struct tagVertexNormalTexcoord
{
    XMFLOAT3  vPosition;    // 12 bytes (offset: 0)
    XMFLOAT3  vNormal;      // 12 bytes (offset: 12)
    XMFLOAT2  vTexcoord;    //  8 bytes (offset: 24)

    static const unsigned int iNumElements = { 3 };
    static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
} VTXNORTEX;
```

### D3D11_INPUT_ELEMENT_DESC 필드 설명

```cpp
{
    "POSITION",                     // SemanticName: HLSL 시맨틱과 매칭
    0,                              // SemanticIndex: 같은 이름이 여러 개일 때
    DXGI_FORMAT_R32G32B32_FLOAT,    // Format: 데이터 형식 (float3)
    0,                              // InputSlot: 버텍스 버퍼 슬롯 번호
    0,                              // AlignedByteOffset: 시작 오프셋
    D3D11_INPUT_PER_VERTEX_DATA,    // InputSlotClass: 정점별 or 인스턴스별
    0                               // InstanceDataStepRate: 인스턴싱 시 사용
}
```

### 정점 포맷 목록

| 구조체 | 용도 | 요소 |
|--------|------|------|
| `VTXPOS` | 기본 위치만 | Position |
| `VTXPOSTEX` | 2D UI, 화면 쿼드 | Position + UV |
| `VTXCUBE` | 스카이박스 | Position + UV3D |
| `VTXNORTEX` | 지형 | Position + Normal + UV |
| `VTXMESH` | 스태틱 메시 | Position + Normal + UV + Tangent + Binormal |
| `VTXSKINMESH` | 스켈레탈 메시 | 위 + BlendIndex + BlendWeight |

### 인스턴싱 정점 (듀얼 버퍼)

```cpp
typedef struct tagVertexPosTexInstanceParticle
{
    static const unsigned int iNumElements = { 7 };
    static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
        // 슬롯 0: 정점 데이터 (PER_VERTEX)
        { "POSITION", 0, ..., 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, ..., 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        // 슬롯 1: 인스턴스 데이터 (PER_INSTANCE)
        { "TEXCOORD", 1, ..., 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD", 2, ..., 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD", 3, ..., 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD", 4, ..., 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD", 5, ..., 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
} VTXPOSTEX_INSTANCEPARTICLE;
```

**슬롯 0**은 모든 인스턴스가 공유하는 메시 정점 (사각형),
**슬롯 1**은 인스턴스별 데이터 (월드 행렬, 색상 등).
`D3D11_INPUT_PER_INSTANCE_DATA`와 `InstanceDataStepRate=1`로 인스턴스 단위 읽기.

---

## 5. 셰이더 패스 실행 — Begin

```cpp
HRESULT CShader::Begin(_uint iPassIndex)
{
    // ① 해당 패스의 InputLayout을 Input Assembler에 설정
    m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

    // ② Effect의 패스를 적용 (VS + PS + 상태 등)
    m_pEffect->GetTechniqueByIndex(0)
              ->GetPassByIndex(iPassIndex)
              ->Apply(0, m_pContext);

    return S_OK;
}
```

**Apply()의 역할:**
- 해당 패스에 정의된 VS, PS를 DeviceContext에 바인딩
- 이전에 SetRawValue/SetMatrix 등으로 설정한 변수들을 GPU에 전송
- 블렌드/깊이/래스터라이저 상태도 패스에 정의되어 있으면 적용

### 일반적인 오브젝트 Render 흐름

```cpp
void CSomeObject::Render()
{
    // ① 월드/뷰/프로젝션 행렬 바인딩
    m_pShaderCom->Bind_Matrix("g_WorldMatrix", ...);
    m_pShaderCom->Bind_Matrix("g_ViewMatrix", ...);
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", ...);

    // ② 텍스처 바인딩
    m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

    // ③ 패스 실행 (InputLayout 설정 + Apply)
    m_pShaderCom->Begin(0);

    // ④ 버텍스/인덱스 버퍼 바인딩 + 드로우
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();
}
```

---

## 6. 변수 바인딩 API

### Bind_Matrix — 행렬 바인딩

```cpp
HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
    // 이름으로 변수 핸들 획득
    ID3DX11EffectVariable* pVariable =
        m_pEffect->GetVariableByName(pConstantName);

    // 행렬 타입으로 캐스팅
    ID3DX11EffectMatrixVariable* pMatrixVariable =
        pVariable->AsMatrix();

    // GPU에 행렬 데이터 전송
    return pMatrixVariable->SetMatrix(
        reinterpret_cast<const _float*>(pMatrix));
}
```

### Bind_Matrices — 행렬 배열 바인딩 (스키닝용)

```cpp
HRESULT CShader::Bind_Matrices(const _char* pConstantName,
    const _float4x4* pMatrix, _uint iNumMatrices)
{
    // ...
    return pMatrixVariable->SetMatrixArray(
        reinterpret_cast<const _float*>(pMatrix), 0, iNumMatrices);
}
```

스켈레탈 애니메이션에서 **본 행렬 배열** (최대 512개)을 한 번에 전송할 때 사용.

### Bind_ShaderResource — 텍스처 바인딩

```cpp
HRESULT CShader::Bind_ShaderResource(const _char* pConstantName,
    ID3D11ShaderResourceView* pSRV)
{
    // ...
    ID3DX11EffectShaderResourceVariable* pSRVariable =
        pVariable->AsShaderResource();
    return pSRVariable->SetResource(pSRV);
}
```

### Bind_RawValue — 범용 바인딩

```cpp
HRESULT CShader::Bind_RawValue(const _char* pConstantName,
    const void* pData, _uint iSize)
{
    // 타입 구분 없이 원시 바이트 전송
    return pVariable->SetRawValue(pData, 0, iSize);
}
```

조명 구조체, 카메라 위치 등 **비표준 타입**을 전송할 때 사용.

### 변수 바인딩 요약

| 메서드 | 용도 | HLSL 예시 |
|--------|------|-----------|
| `Bind_Matrix` | 단일 행렬 | `matrix g_WorldMatrix` |
| `Bind_Matrices` | 행렬 배열 | `matrix g_BoneMatrices[512]` |
| `Bind_ShaderResource` | 단일 텍스처 | `Texture2D g_Texture` |
| `Bind_ShaderResources` | 텍스처 배열 | `Texture2D g_Textures[2]` |
| `Bind_RawValue` | 범용 데이터 | `float4 g_vCamPosition` |

---

## 7. 사용되는 셰이더 목록

Loader에서 등록되는 셰이더 프로토타입:

| 셰이더 | 정점 포맷 | 용도 |
|--------|-----------|------|
| `Shader_VtxPosTex` | VTXPOSTEX | UI, 배경 (2D 사각형) |
| `Shader_VtxNorTex` | VTXNORTEX | 지형 (노멀+텍스처) |
| `Shader_VtxMesh` | VTXMESH | 스태틱 메시 |
| `Shader_VtxAnimMesh` | VTXSKINMESH | 스켈레탈 애니메이션 메시 |
| `Shader_VtxCube` | VTXCUBE | 스카이박스 |
| `Shader_VtxPosTexInstanceParticle` | VTXPOSTEX_INSTANCE | 빌보드 파티클 |
| `Shader_VtxPosInstanceParticle` | VTXPOS_INSTANCE | 포인트 파티클 |
| `Shader_Deferred` | VTXPOSTEX | 디퍼드 라이팅 (Renderer 내부) |

---

## 8. 핵심 정리

| 항목 | 설명 |
|------|------|
| **Effects11** | DX9 FX 스타일의 셰이더 관리 래퍼 (오픈소스) |
| **런타임 컴파일** | D3DX11CompileEffectFromFile (Debug: 최적화 없음) |
| **자기 선언 정점** | 구조체 내 static constexpr Elements[]로 InputLayout 정의 |
| **패스 실행** | IASetInputLayout + Apply(0, pContext) |
| **변수 바인딩** | 이름 기반: GetVariableByName → As* → Set* |
| **Clone 공유** | Effect + InputLayout을 참조 카운팅으로 공유 |
| **인스턴싱** | 듀얼 슬롯: 슬롯0(정점) + 슬롯1(인스턴스, PER_INSTANCE) |

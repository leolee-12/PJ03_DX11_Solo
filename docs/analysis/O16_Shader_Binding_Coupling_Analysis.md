# O16. 셰이더 바인딩 패턴 분석 — 컴포넌트 간 커플링 vs 캡슐화

> CTransform, CTexture가 CShader를 직접 받아 바인딩하는 현재 구조에 대한 평가와 대안 분석

---

## 1. 현재 구조 (코드 기준)

### 1.1 바인딩 패턴

```cpp
// Transform.h — CShader를 forward declaration으로 받음
HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

// Transform.cpp — CShader의 메서드를 직접 호출
HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}
```

```cpp
// Texture.h — 동일 패턴
HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);

// Texture.cpp
HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
    return pShader->Bind_SRV(pConstantName, m_Textures[iTextureIndex]);
}
```

### 1.2 include 의존성 경로

```
Transform.cpp / Texture.cpp
  → #include "GameInstance.h"
    → #include "Prototype_Manager.h"
      → #include "Shader.h"      ← 여기서 CShader 전체 정의 유입
      → #include "Texture.h"
      → #include "VIBuffer_Rect.h"
```

**Prototype_Manager.h가 Shader.h를 include하는 이유**: Clone 시 `dynamic_cast<CComponent*>` 수행을 위해 구체 타입이 필요하기 때문으로 추정되나, 실제 코드에서는 `CBase*`로 관리하고 있어 꼭 필요한 include는 아님.

### 1.3 호출 시점 (추정 — 현재 BackGround::Render()는 빈 구현이나, 참고프로젝트 패턴 기준)

```cpp
// GameObject의 Render()에서:
HRESULT CBackGround::Render()
{
    m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
    m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0);
    m_pShaderCom->Begin(0);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}
```

---

## 2. 강사님 의도 평가

### 캡슐화 관점 — 달성 여부

목적: **Getter 지양, 내부 데이터 노출 방지**

| 항목 | 평가 |
|------|------|
| `m_WorldMatrix` 외부 노출 방지 | **달성** — CTransform이 직접 Bind, Getter 불필요 |
| `m_Textures[i]` (SRV) 외부 노출 방지 | **달성** — CTexture가 직접 Bind, Getter 불필요 |
| 변경 영향 범위 제한 | **부분적** — 내부 타입 변경 시 외부 수정 불필요하지만, CShader API 변경 시 모든 컴포넌트 수정 필요 |

**캡슐화는 실질적으로 달성됨**. 데이터 소유자가 직접 바인딩을 수행하므로 Getter로 꺼내는 것보다 낫다.

### 커플링 관점 — 문제점

| 문제 | 설명 |
|------|------|
| **컴포넌트 → 컴포넌트 의존** | CTransform, CTexture가 CShader의 구체 메서드(`Bind_Matrix`, `Bind_SRV`)에 의존 |
| **include 연쇄** | Transform.cpp → GameInstance.h → Prototype_Manager.h → Shader.h |
| **역할 혼재** | "자신의 데이터를 관리하는 컴포넌트"가 "셰이더 바인딩 방법"까지 알아야 함 |
| **확장성 제한** | CShader 대신 다른 렌더링 백엔드를 사용하려면 모든 컴포넌트 수정 필요 |

include 경로 자체가 문제라기보다는, **엔진 컴포넌트끼리 구체 타입을 알아야 하는 구조**가 본질적인 문제. 현재는 `Prototype_Manager.h`가 `Shader.h`를 include하기 때문에 간접적으로 해결되지만, 이는 우연의 일치에 가까움.

---

## 3. 대안 분석

### 3.1 방안 A: Getter 방식 (가장 단순, 강사님이 지양한 방식)

```cpp
// Transform.h
const _float4x4* Get_WorldMatrix() const { return &m_WorldMatrix; }

// Texture.h
ID3D11ShaderResourceView* Get_SRV(_uint iIndex) const { return m_Textures[iIndex]; }

// GameObject::Render()
m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix());
m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pTextureCom->Get_SRV(0));
```

| 장점 | 단점 |
|------|------|
| Transform/Texture가 Shader를 모름 (의존 제거) | 내부 데이터 타입이 외부에 노출 |
| 컴포넌트 간 완전 독립 | `m_WorldMatrix`의 타입 변경 시 호출자 전체 수정 |
| 가장 직관적이고 간단 | Getter 남용의 시작점이 될 수 있음 |

**평가**: 커플링은 확실히 제거되지만 캡슐화가 약해짐. 다만 현실적으로 `_float4x4`나 `ID3D11ShaderResourceView*`는 DX11 API가 강제하는 타입이므로 변경 가능성이 극히 낮음. **실용적 관점에서는 큰 문제가 아님**.

### 3.2 방안 B: CShader에 오버로드 추가 (의존 방향 역전)

```cpp
// Shader.h — CShader가 각 컴포넌트 타입을 받음
HRESULT Bind_TransformMatrix(class CTransform* pTransform, const _char* pConstantName);
HRESULT Bind_Texture(class CTexture* pTexture, const _char* pConstantName, _uint iIndex);
```

```cpp
// Shader.cpp
HRESULT CShader::Bind_TransformMatrix(CTransform* pTransform, const _char* pConstantName)
{
    return Bind_Matrix(pConstantName, pTransform->Get_WorldMatrix());
}
```

| 장점 | 단점 |
|------|------|
| Transform/Texture가 Shader를 모름 | **Shader가 모든 컴포넌트를 알아야 함** — 더 나쁜 커플링 |
| | 새 컴포넌트 추가 시 Shader 수정 필요 |
| | 결국 Getter가 필요하므로 방안 A보다 복잡한 우회 |

**평가**: 의존 방향만 바꿨을 뿐, 커플링은 오히려 더 심해짐. **비추천**.

### 3.3 방안 C: 인터페이스(추상 클래스) 분리 — IShaderBindable

```cpp
// Engine/Public/ShaderBindable.h (신규)
NS_BEGIN(Engine)

// 셰이더 바인딩에 필요한 최소 인터페이스
class ENGINE_DLL IShaderBindable abstract
{
public:
    virtual HRESULT Bind_Matrix(const _char* pName, const _float4x4* pMatrix) = 0;
    virtual HRESULT Bind_SRV(const _char* pName, ID3D11ShaderResourceView* pSRV) = 0;
};

NS_END
```

```cpp
// Shader.h
class ENGINE_DLL CShader final : public CComponent, public IShaderBindable
{
    // 기존 Bind_Matrix, Bind_SRV가 이미 IShaderBindable을 충족
};
```

```cpp
// Transform.h — CShader 대신 IShaderBindable에 의존
HRESULT Bind_ShaderResource(IShaderBindable* pBinder, const _char* pConstantName);

// Transform.cpp
HRESULT CTransform::Bind_ShaderResource(IShaderBindable* pBinder, const _char* pConstantName)
{
    return pBinder->Bind_Matrix(pConstantName, &m_WorldMatrix);
}
```

```cpp
// GameObject::Render() — 호출 코드는 현재와 동일
m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");  // CShader*가 IShaderBindable*로 암시적 변환
```

| 장점 | 단점 |
|------|------|
| Transform/Texture가 CShader 구체 타입을 모름 | 인터페이스 클래스 1개 추가 |
| **캡슐화 완전 유지** (Getter 없음) | vtable 오버헤드 (무시 가능) |
| CShader를 다른 구현으로 교체 가능 | 다중 상속 (CComponent + IShaderBindable) |
| include 의존 제거 (ShaderBindable.h만 include) | |
| **호출 코드 변경 없음** | |

**include 구조 변화**:
```
Transform.cpp
  → #include "ShaderBindable.h"   ← CShader.h 대신 경량 인터페이스
  → #include "GameInstance.h"     ← Bind_ShaderResource와 무관하게 필요한 경우만
```

### 3.4 방안 D: CGameInstance 퍼사드에 바인딩 메서드 추가

```cpp
// GameInstance.h
HRESULT Bind_Matrix(CShader* pShader, const _char* pName, const _float4x4* pMatrix);
HRESULT Bind_SRV(CShader* pShader, const _char* pName, ID3D11ShaderResourceView* pSRV);
```

| 장점 | 단점 |
|------|------|
| 기존 퍼사드 패턴과 일관성 | **문제 해결이 안 됨** — 결국 CShader*를 인자로 받으므로 의존 동일 |
| | GameInstance가 비대해짐 |

**평가**: 현재 아키텍처에서 GameInstance는 매니저 위임 퍼사드인데, 개별 컴포넌트의 바인딩 메서드까지 넣으면 책임이 과도해짐. **비추천**.

---

## 4. 종합 평가

### 현재 구조에 대한 판단

현재 구조는 **"합리적인 트레이드오프"** 범위 안에 있다.

- **캡슐화는 달성됨**: 데이터 소유자가 바인딩을 수행, Getter 불필요
- **커플링은 존재하지만 한정적**: CTransform/CTexture → CShader 단방향 의존, CShader의 `Bind_Matrix`/`Bind_SRV` 시그니처가 바뀌지 않는 한 문제 없음
- **include 경로**: `Prototype_Manager.h`가 `Shader.h`를 직접 include하는 것이 근본 원인이나, 이것은 바인딩 패턴과 별개의 문제 (Prototype_Manager에서 구체 타입 include를 제거하면 독립적으로 해결 가능)

### 만약 개선한다면

| 우선순위 | 방안 | 이유 |
|---------|------|------|
| **1순위** | **C (IShaderBindable 인터페이스)** | 캡슐화 유지 + 커플링 제거 + 호출 코드 변경 없음 |
| 2순위 | A (Getter) | 가장 단순. DX11 타입이 변경될 가능성이 사실상 없으므로 실용적 |
| 비추천 | B, D | 커플링을 다른 곳으로 옮기거나 해결 안 됨 |

### Prototype_Manager.h의 include 문제 (별도 개선)

현재 `Prototype_Manager.h`가 `Shader.h`, `Texture.h`, `VIBuffer_Rect.h`를 직접 include하는 것은 바인딩 패턴과 무관하게 개선 가능:

```cpp
// 현재 Prototype_Manager.h
#include "Shader.h"        // ← 왜 필요한가?
#include "Texture.h"
#include "VIBuffer_Rect.h"
```

Prototype_Manager는 `CBase*`로 프로토타입을 관리하고, Clone 시 `dynamic_cast<CGameObject*>` 또는 `dynamic_cast<CComponent*>`만 수행. **Shader/Texture/VIBuffer의 구체 타입은 불필요**. 이 include를 제거하면:
- `GameInstance.h`를 include해도 Shader.h가 따라오지 않음
- 컴포넌트 간 의존이 include 경로에서도 분리됨
- 각 cpp에서 필요한 헤더만 명시적으로 include하게 됨

단, 이 include가 클라이언트 편의를 위한 의도적인 것일 수 있음 (GameInstance.h 하나만 include하면 모든 엔진 타입을 쓸 수 있도록). 이 경우 Prototype_Manager.h 대신 별도 `Engine_Components.h` 등으로 모아두는 것이 더 적절.

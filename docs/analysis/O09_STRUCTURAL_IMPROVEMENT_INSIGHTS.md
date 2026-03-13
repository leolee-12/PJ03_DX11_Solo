# O09: 구조적 개선 인사이트 종합

## 개요

현재 프레임워크에서 당장 문제는 아니지만 구조적으로 더 나은 방안이 있거나, 향후 확장 시 문제가 될 수 있는 부분을 정리. 우선순위(상/중/하)로 분류.

---

## [상] 1. Clear_Resources() 미구현

### 현상
`Level_Manager::Change_Level()`에서 `Clear_Resources(m_iCurrentLevelIndex)`를 호출하지만, 실제 구현은 비어 있음.

```cpp
// GameInstance.cpp:80-86
void CGameInstance::Clear_Resources(_int iLevelIndex)
{
    if (-1 == iLevelIndex)
        return;
    /* iLevelIndex의 자원을 정리 */ // <- 주석만 존재
}
```

### 문제점
레벨 전환 시 이전 레벨의 프로토타입과 오브젝트가 메모리에 남음. Logo→GamePlay 전환 시 Logo 전용 리소스(텍스처, 프로토타입)가 해제되지 않음.

### 참고 프로젝트 구현
```cpp
void CGameInstance::Clear_Resources(_uint iLevelID) {
    m_pPrototype_Manager->Clear(iLevelID);
    m_pObject_Manager->Clear(iLevelID);
}
```
레벨별 배열 슬롯의 프로토타입과 오브젝트를 개별 정리. LEVEL::STATIC 슬롯은 정리 대상에서 제외하여 공용 리소스 보존.

### 인사이트
LEVEL::STATIC이 enum 첫 번째(0)에 위치한 설계 의도가 여기서 드러남. Static 슬롯에 등록된 리소스는 레벨 전환에도 유지되고, 레벨별 슬롯의 리소스만 정리하는 구조.

---

## [상] 2. Loader의 m_isFinished 동기화 부재

### 현상
`m_isFinished`를 로딩 스레드에서 쓰고, 메인 스레드에서 읽지만 동기화가 없음.

```cpp
// Loading 스레드 (Loader.cpp:92)
m_isFinished = true;

// 메인 스레드 (Level_Loading.cpp:29)
if (true == m_pLoader->isFinished())  // 동기화 없이 읽기
```

### 문제점
C++ 메모리 모델에서 서로 다른 스레드의 비-atomic 읽기/쓰기는 **정의되지 않은 동작(UB)**. x86에서는 `_bool`(1바이트) 쓰기가 사실상 atomic이라 실무에서 문제가 드물지만, 표준 위반.

### 개선 방안

**A. std::atomic<bool> 사용 (권장)**
```cpp
std::atomic<_bool> m_isFinished = { false };
```
가장 깔끔하고 표준 준수. 성능 차이 무시할 수준.

**B. Critical Section으로 보호**
```cpp
_bool CLoader::isFinished() {
    EnterCriticalSection(&m_CriticalSection);
    _bool result = m_isFinished;
    LeaveCriticalSection(&m_CriticalSection);
    return result;
}
```
이미 CS를 사용 중이므로 일관성은 좋지만, 단순 bool 읽기에 과도한 오버헤드.

---

## [상] 3. CBase::Release() 반환값과 Safe_Release 상호작용

### 현상

```cpp
// Base.cpp
_uint CBase::Release()
{
    if (0 == m_iRefCnt)   // refcnt가 0이면 파괴
    {
        Free();
        delete this;
        return 0;
    }
    else
        return m_iRefCnt--;  // 후위 감소: 이전 값 반환
}
```

### 동작 흐름 (정상 작동하지만 이해 필요)

이 레퍼런스 카운팅은 COM과 다른 체계:
- **COM**: refcnt 1에서 시작, 0 도달 시 파괴
- **이 프레임워크**: refcnt 0에서 시작, 0인 상태에서 Release 호출 시 파괴

| AddRef 횟수 | m_iRefCnt | 파괴까지 필요한 Release |
|------------|-----------|----------------------|
| 0 (생성만) | 0 | 1회 |
| 1 | 1 | 2회 |
| 2 | 2 | 3회 |

후위 감소(`m_iRefCnt--`)는 의도적. Safe_Release는 반환값이 0일 때만 포인터를 nullptr로 설정하는데, 이 0은 오직 Free()+delete가 실행된 경우에만 발생. 따라서 **"포인터가 nullptr이면 객체가 파괴됨"이 보장**됨.

### 인사이트
만약 전위 감소(`--m_iRefCnt`)를 사용하면, refcnt가 1→0으로 감소할 때 반환값이 0이 되어 Safe_Release가 포인터를 nullptr로 설정하지만, 실제 객체는 아직 살아있음(다음 Release에서 파괴). **댕글링 nullptr 문제** 발생. 현재의 후위 감소가 이 체계에서는 정확히 맞는 설계.

---

## [중] 4. `#define new DBG_NEW` 전역 재정의

### 현상
```cpp
// Engine_Defines.h:39-40
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
```

### 문제점
- **placement new 차단**: `new (ptr) Type()`이 `DBG_NEW (ptr) Type()`으로 치환되어 컴파일 에러
- **operator new 오버로드 차단**: 클래스별 커스텀 allocator 사용 불가
- **STL 내부 충돌**: 일부 STL 구현체가 placement new를 사용하여 내부에서 에러 발생 가능

### 개선 방안
현재는 프레임워크가 placement new를 사용하지 않으므로 실질적 문제 없음. 향후 컴포넌트 풀링이나 메모리 풀을 도입할 때 이 매크로가 걸림돌이 될 수 있으므로 인지 필요.

사용 시 해당 파일에서 일시적 해제:
```cpp
#pragma push_macro("new")
#undef new
// placement new 사용 코드
#pragma pop_macro("new")
```

---

## [중] 5. `using namespace std` 헤더 전역 오염

### 현상
```cpp
// Engine_Defines.h:18
using namespace std;
```
이 헤더가 모든 엔진/클라이언트 파일에 포함되므로, 전체 프로젝트에서 std 네임스페이스가 전역으로 노출.

### 실질적 영향
- `count`, `find`, `swap`, `move` 등 std 함수명과 사용자 정의 함수명 충돌 가능
- 현재 규모에서는 문제없으나, 서드파티 라이브러리 추가 시 이름 충돌 위험 증가

### 인사이트
학원 프레임워크의 보편적 관행이며, 수업 중 `std::` 접두사 생략으로 코드 가독성을 높이려는 의도. 실무 프로젝트에서는 cpp 파일 내에서만 `using namespace std`를 사용하고, 헤더에서는 `std::` 명시가 원칙. 다만 현재 프로젝트 범위에서는 수정 우선순위 낮음.

---

## [중] 6. 프레임 제한 타이밍 드리프트

### 현상
```cpp
// Client.cpp:86,93
if (fTimeAcc >= fFrameRate)
{
    // Update + Render
    fTimeAcc = 0.f;  // 잔여값 버림
}
```

### 문제점
`fTimeAcc`이 `fFrameRate`를 초과한 잔여분을 버림. 예: fTimeAcc=0.018, fFrameRate=0.0167이면 0.0013초의 잔여값 소실. 장시간 누적되면 프레임 타이밍이 점진적으로 어긋남.

### 개선 방안
```cpp
fTimeAcc -= fFrameRate;  // 잔여값 보존
```

### 인사이트
참고 프로젝트에서는 프레임 제한 조건 자체를 주석 처리하고 매 루프 실행(`if (/*...*/ 1)`). 이는 V-Sync나 별도 프레임 리미터에 의존하는 방식. 현재 프로젝트의 소프트웨어 프레임 제한이 더 명시적이므로, 잔여값 보존만 추가하면 완성도 높아짐.

---

## [중] 7. PROTOTYPE enum 기반 Clone 분기

### 현상
```cpp
// Prototype_Manager.cpp:40-43
if (PROTOTYPE::GAMEOBJECT == eType)
    pInstance = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
else
    /*pInstance = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg)*/;
```

### 문제점
- CBase*로 저장한 뒤 런타임에 enum으로 타입 분기 + dynamic_cast → RTTI 비용 + 타입 안전성 약화
- 새로운 프로토타입 타입 추가 시 enum 확장 + if/switch 분기 추가 필요

### 개선 방안
CBase에 순수 가상 Clone을 두면 enum 분기와 dynamic_cast가 불필요:
```cpp
class CBase { virtual CBase* Clone(void* pArg) PURE; };
```

### 인사이트
참고 프로젝트도 동일 패턴 사용. 학원 커리큘럼에서 프로토타입과 컴포넌트를 하나의 맵에서 관리하되 타입으로 구분하는 교육적 설계. 실무에서는 CBase에 Clone 가상 함수를 두거나, 프로토타입 맵을 타입별로 분리하는 것이 일반적.

---

## [하] 8. Raw Array → vector 전환 가능성

### 현상
```cpp
// Object_Manager.h:29
LAYERS* m_pLayers = { nullptr };  // new LAYERS[iNumLevels]로 할당

// Prototype_Manager.h:25
PROTOTYPES* m_pPrototypes = { nullptr };  // new PROTOTYPES[iNumLevels]로 할당
```

### 인사이트
`vector<LAYERS>`를 사용하면 `Safe_Delete_Array` 수동 호출이 불필요하고, RAII로 예외 안전성 확보. 다만 현재 구조에서는 Initialize/Free가 명확하게 짝을 이루고 있어 실질적 위험은 낮음. 참고 프로젝트도 동일하게 raw array 사용.

---

## [하] 9. map 키의 불필요한 const

### 현상
```cpp
// Timer_Manager.h:18
map<const _wstring, class CTimer*> m_Timers;
```

### 인사이트
`std::map`의 key_type은 이미 내부적으로 const로 취급됨. `const _wstring`과 `_wstring`은 map에서 동일하게 동작하지만, `const`가 불필요한 복사 생성자 호출을 유발할 수 있음(일부 컴파일러). 제거해도 동작 변화 없음.

---

## [하] 10. Singleton GetInstance()의 스레드 안전성

### 현상
```cpp
// Engine_Macro.h:64-68
static CLASSNAME* GetInstance(void) {
    if(nullptr == m_pInstance)
        m_pInstance = new CLASSNAME;
    return m_pInstance;
}
```

### 인사이트
CLoader가 별도 스레드에서 `CGameInstance::GetInstance()`를 호출하는데, 이론적으로 race condition 가능. 그러나 실제로는 MainApp 생성 시 이미 싱글톤이 초기화되어 있으므로, Loader 스레드 시작 시점에는 항상 m_pInstance가 유효. **현실적 위험도 극히 낮음.**

C++11의 Meyers' Singleton(`static local`)이 스레드 안전하지만, CBase 레퍼런스 카운팅과 DestroyInstance 패턴을 유지하려면 현재 매크로 방식이 더 자연스러움.

---

## 우선순위 요약

| 순위 | 항목 | 영향도 | 수정 난이도 |
|------|------|--------|-----------|
| 상 | Clear_Resources 구현 | 레벨 전환 시 메모리 누수 | 낮음 |
| 상 | m_isFinished atomic 전환 | UB (표준 위반) | 매우 낮음 |
| 상 | Release() 후위 감소 이해 | 설계 이해 (수정 불필요) | - |
| 중 | #define new 인지 | 향후 확장 시 걸림돌 | 인지만 |
| 중 | 프레임 타이밍 드리프트 | 장시간 실행 시 오차 | 매우 낮음 |
| 중 | Clone 분기 구조 | 확장성 제한 | 중간 |
| 하 | Raw array → vector | 코드 안전성 향상 | 낮음 |
| 하 | const 키 제거 | 불필요한 수식어 | 매우 낮음 |

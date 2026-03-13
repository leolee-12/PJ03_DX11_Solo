# O08-B: LEVEL Enum 사용 패턴 분석

## 결론: `static constexpr` 방식 권장 (현재 프로젝트 방식 유지)

---

## 1. 세 가지 방안 비교

| 기준 | A. 매번 인라인 | B. static constexpr | C. 부모 _uint 멤버 |
|------|--------------|---------------------|-------------------|
| **enum class 의미** | 호출마다 명시적 | 파일 상단 1회 변환 | 완전 상실 |
| **가독성** | 반복 많으면 지저분 | 깔끔 | 깔끔 |
| **타입 안전성** | 변환 시점마다 명시 | 변환 1회 | 상실 |
| **유지보수** | 레벨 변경시 N곳 수정 | 1곳 수정 | 1곳 수정 |
| **런타임 비용** | 0 (컴파일타임) | 0 (constexpr) | 멤버 접근 비용 |
| **엔진-클라이언트 분리** | 유지 | 유지 | **침해** |

---

## 2. 현재 프로젝트 사용 현황

### Level_Logo.cpp
```cpp
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::LOGO);  // 상단 선언
// Ready_Layer_BackGround에서 2회 사용 (Add_GameObject 인자)
// Change_Level에서는 ETOI(LEVEL::LOADING) 인라인 사용
```

### Level_Loading.cpp
```cpp
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::LOADING);
// switch문에서 LEVEL::LOGO, LEVEL::GAMEPLAY 직접 사용 (변환 불필요)
// Change_Level에서 ETOI(m_eNextLevelID) 사용
```

### Level_GamePlay.cpp
```cpp
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::GAMEPLAY);
// 현재 스켈레톤 코드 (사용처 아직 없음)
```

---

## 3. 참고 프로젝트(3D최종버전) 사용 현황

참고 프로젝트는 `static constexpr`를 사용하지 않고 **매번 인라인 변환**:

### Level_GamePlay.cpp (참고)
```cpp
// Add_GameObject 호출마다 ENUM_CLASS(LEVEL::GAMEPLAY) 반복
// 총 18회 이상의 인라인 ENUM_CLASS() 호출
```

이는 GamePlay 레벨에서 다수의 오브젝트를 추가하면서 동일 값을 반복 기입하는 패턴. 가독성이 떨어지고 레벨 변경 시 수정 범위가 넓어짐.

### Level_Logo.cpp (참고)
```cpp
// Add_GameObject에서 ENUM_CLASS(LEVEL::LOGO) 2회 사용
// 소규모라 인라인도 무방
```

---

## 4. 분석: `static constexpr`가 enum class 의미를 퇴색시키는가?

### 퇴색되지 않는 이유

1. **변환이 명시적**: `ETOUI(LEVEL::LOGO)`로 enum class 이름이 선언부에 그대로 노출됨
2. **의미 전달 충분**: `CURRENT_LEVEL`이라는 변수명 자체가 "이 파일의 레벨"을 명확히 전달
3. **타입 안전성은 어차피 소실**: `Add_GameObject`가 `_uint`를 받으므로 어떤 방식이든 변환은 필수. 차이는 변환 횟수뿐
4. **선언부에서 원본 enum 확인 가능**: 파일 상단을 보면 어떤 레벨인지 즉시 파악

### enum class 의미가 퇴색되는 경우 (방안 C)

부모 클래스에 `_uint m_iLevelIndex` 멤버를 두면:
- 생성자에서 `(_uint)LEVEL::LOGO`를 전달 → 이후 모든 사용처에서 `m_iLevelIndex`만 사용
- **어떤 레벨인지 코드만 봐서는 알 수 없음**
- 엔진 레벨에서 클라이언트의 LEVEL enum 의존성 발생 (엔진-클라이언트 경계 침해)

---

## 5. "현재 레벨 값"이 Add_Object 외에도 많이 사용되는가?

### 참고 프로젝트 기준 사용 범위

| 사용처 | 빈도 | 설명 |
|--------|------|------|
| `Add_GameObject` | **높음** | 프로토타입/레이어 레벨 인덱스 (2개 인자) |
| `Add_Prototype` | 중간 | Loader에서 리소스 등록 시 |
| `Change_Level` | 낮음 | 레벨 전환 시 1회 (다른 레벨 값 사용) |
| `Clear_Resources` | 낮음 | 리소스 정리 시 |
| switch문 | 낮음 | Level_Loading에서 분기 (enum 직접 사용) |

`Add_GameObject`에서의 사용 빈도가 압도적이며, 참고 프로젝트의 GamePlay 레벨에서는 18회 이상 반복됨. **`static constexpr`의 실용적 가치가 높은 구간**.

---

## 6. 권장 패턴

```cpp
// Level_GamePlay.cpp 상단
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::GAMEPLAY);

// Add_GameObject 호출 시 - 현재 레벨은 CURRENT_LEVEL 사용
m_pGameInstance->Add_GameObject(CURRENT_LEVEL, TEXT("Proto_Player"),
                                CURRENT_LEVEL, TEXT("Layer_Player"));

// 다른 레벨 참조 시 - 인라인 변환 (명시적)
m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLevel);
```

### 핵심 원칙
- **현재 레벨 값**: `static constexpr CURRENT_LEVEL` 사용 (반복 제거, 수정 1곳)
- **다른 레벨 값**: 인라인 `ETOUI(LEVEL::XXX)` 사용 (의도 명시)
- **switch/비교문**: enum class 직접 사용 (변환 불필요)

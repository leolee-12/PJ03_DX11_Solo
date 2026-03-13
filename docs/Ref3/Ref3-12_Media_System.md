# O13. 참고프로젝트3 미디어 시스템 심화 분석

> 분석 대상: `참고프로젝트3/DDProject/Engine/` (DX9, ProjectMud)
> 분석 범위: DirectShow 비디오 재생 + FMOD 사운드

---

## 1. 시스템 개요

### 1-1. 비디오 서브시스템 (DirectShow)

| 클래스 | 역할 | 위치 |
|--------|------|------|
| `CMediaMgr` | 싱글톤 매니저. 비디오 재생 오케스트레이션, 렌더링 | `Utility/Codes/MediaMgr.h/.cpp` |
| `CMediaObj` | DirectShow 필터 그래프 래퍼. COM 인터페이스 소유 | `Utility/Codes/MediaObj.h/.cpp` |
| `CTextureRenderer` | `CBaseVideoRenderer` 상속. 비디오 프레임→D3D 텍스처 | `Utility/Codes/TextureRenderer.h/.cpp` |
| `CRect_Texture` | 풀스크린 쿼드 버텍스 버퍼 | `Utility/Codes/Rect_Texture.h/.cpp` |

### 1-2. 사운드 서브시스템 (FMOD)

| 클래스 | 역할 | 위치 |
|--------|------|------|
| `CSoundMgr` | 싱글톤 매니저. FMOD 기반 사운드 재생/관리 | `System/Codes/SoundMgr.h/.cpp` |

---

## 2. 소유권 및 참조 관계

```
CMediaMgr (Singleton)
├── m_pGraphicDev : LPDIRECT3DDEVICE9  [AddRef 소유]
├── m_pMediaObj   : CMediaObj*          [new/Safe_Release 소유]
├── m_pRcTexBuffer: CRect_Texture*      [Create/Safe_Release 소유]
├── m_pVtxPos     : VTXTEX[4]           [new[]/Safe_Delete_Array]
└── m_pVtxConPos  : VTXTEX[4]           [new[]/Safe_Delete_Array]

CMediaObj (CBase 상속)
├── m_pGB       : CComPtr<IGraphBuilder>   [COM 스마트 포인터]
├── m_pMC       : CComPtr<IMediaControl>
├── m_pMP       : CComPtr<IMediaPosition>
├── m_pME       : CComPtr<IMediaEvent>
├── m_pRenderer : CComPtr<IBaseFilter>     [CTextureRenderer를 IBaseFilter로 소유]
└── pCTR        : CTextureRenderer*        [raw 포인터, m_pRenderer와 동일 객체]

CTextureRenderer (CBaseVideoRenderer 상속, COM 객체)
├── m_pGraphicDev : LPDIRECT3DDEVICE9  [AddRef 소유]
├── m_pTexture    : LPDIRECT3DTEXTURE9 [CreateTexture/Safe_Release 소유]
└── m_lVidWidth/Height/Pitch : LONG    [비디오 메타데이터]
```

### 주요 소유권 특징

1. **이중 참조 패턴**: `CMediaObj`가 `CTextureRenderer`를 COM(`m_pRenderer`)과 raw(`pCTR`) 두 가지로 참조. COM 참조가 수명을 관리하고, raw 포인터는 캐스팅 없이 전용 메서드(`SetTexture`) 호출 용도.

2. **COM + 레퍼런스 카운팅 혼합**: `CMediaObj`는 `CBase` 상속(엔진 레퍼런스 카운팅)이면서 내부에 COM 객체를 `CComPtr`로 관리. 두 시스템이 공존.

3. **D3D 디바이스 AddRef 체인**: `CMediaMgr` → `CMediaObj::Initialize` → `CTextureRenderer` 생성자 각각에서 `AddRef`. 해제는 역순으로 `Safe_Release`.

---

## 3. 프레임별 호출 흐름

### 3-1. 비디오 재생 시작 (`VideoPlay`)

```
Client → CMediaMgr::VideoPlay(wFileName)
  ├─ 기존 m_pMediaObj 있으면 Safe_Release
  ├─ new CMediaObj
  └─ CMediaObj::Initialize(pGraphicDev, wFileName)
       ├─ CoCreateInstance(CLSID_FilterGraph) → m_pGB
       ├─ new CTextureRenderer(pGraphicDev) → pCTR
       ├─ m_pGB->AddFilter(pCTR, "TextureRenderer")
       ├─ m_pGB->AddSourceFilter(wFileName) → pFSrc
       ├─ pFSrc->FindPin("Output") → pFSrcPinOut
       ├─ m_pGB->Render(pFSrcPinOut)
       │    └─ DirectShow 내부에서 디코더 자동 삽입
       │       Source → [Decoder] → TextureRenderer
       ├─ QueryInterface → m_pMC, m_pMP, m_pME
       └─ m_pMC->Run()  ← 재생 시작
```

> **DirectShow 자동 렌더링**: `m_pGB->Render(pFSrcPinOut)`는 소스 필터의 출력 핀에서 `CTextureRenderer` 입력 핀까지의 경로를 자동 구성한다. 중간 디코더 필터(AVI Decompressor 등)가 자동 삽입됨.

### 3-2. 매 프레임 갱신

```
메인 루프
  ├─ CMediaMgr::Progress()
  │    └─ VertexRenewal()
  │         ├─ 4개 꼭짓점에 ProjVector() 적용 (원근 투영)
  │         └─ ConvertVector()로 NDC 변환 (-1~1)
  │
  └─ CMediaMgr::Render()
       ├─ 샘플러 스테이트 설정 (LINEAR 필터링)
       ├─ View/Proj 행렬 백업
       ├─ View = Proj = Identity 설정
       ├─ m_pMediaObj->SetTexMovie(0)
       │    └─ CTextureRenderer::SetTexture(0)
       │         └─ m_pGraphicDev->SetTexture(0, m_pTexture)
       ├─ m_pRcTexBuffer->SetVtxInfo(m_pVtxConPos)
       ├─ m_pRcTexBuffer->Render_Buffer()
       │    └─ DrawIndexedPrimitive (풀스크린 쿼드)
       └─ View/Proj 행렬 복원
```

### 3-3. 비디오 프레임 디코딩 (DirectShow 스레드)

```
[DirectShow 워커 스레드 — 메인 스레드와 비동기]
  Source Filter → Decoder → CTextureRenderer::DoRenderSample(pMediaSample)
    ├─ pMediaSample->GetPointer(&pBmpBuffer)  // RGB24 비트맵
    ├─ m_pTexture->LockRect(0, &d3dlr)
    ├─ 픽셀 복사 루프:
    │    ├─ 시작점: 텍스처 마지막 행 (수직 반전)
    │    ├─ BGR24(3바이트) → BGRX32(4바이트) 변환
    │    │    pTxt[0]=B, pTxt[1]=G, pTxt[2]=R, pTxt[3]=0xFF
    │    └─ 행 단위: 비트맵은 +pitch, 텍스처는 -pitch (상하 반전)
    └─ m_pTexture->UnlockRect(0)
```

> **핵심**: DirectShow는 별도 스레드에서 비디오를 디코딩하며, `DoRenderSample`이 호출될 때마다 D3D 텍스처를 직접 갱신한다. 메인 스레드의 `Render()`는 이 텍스처를 그대로 바인딩하여 표시.

---

## 4. 디자인 패턴 분석

### 4-1. Singleton (CMediaMgr, CSoundMgr)

```cpp
DECLARE_SINGLETON(CMediaMgr)   // 헤더
IMPLEMENT_SINGLETON(CMediaMgr) // cpp
CMediaMgr::GetInstance()->VideoPlay(L"intro.avi");
```

엔진의 표준 싱글톤 매크로 사용. `CBase` 상속으로 레퍼런스 카운팅과 통합.

### 4-2. Adapter / Bridge (CTextureRenderer)

`CTextureRenderer`는 **DirectShow의 렌더러 인터페이스**(`CBaseVideoRenderer`)와 **D3D 텍스처**를 연결하는 어댑터:

- DirectShow 측: `CheckMediaType`, `SetMediaType`, `DoRenderSample` (가상 함수 오버라이드)
- D3D 측: `CreateTexture`, `LockRect`, `SetTexture`

두 API의 프로토콜 차이를 한 클래스에서 번역한다.

### 4-3. Facade (CMediaMgr)

클라이언트가 DirectShow COM 인터페이스를 직접 다루지 않도록 단순화:

```
Client → CMediaMgr::VideoPlay("file.avi")  // 한 줄 호출
         내부: COM 초기화, 필터 그래프 구성, 디코더 연결, 재생 시작
```

### 4-4. Factory Method (CMediaObj 생성)

`CMediaMgr::VideoPlay` 안에서 `new CMediaObj` + `Initialize` 패턴. 단, **엔진 표준 Create/Clone 패턴을 따르지 않음** — COM 객체 특성상 프로토타입 복제가 부적합하기 때문.

---

## 5. DirectX API 사용 포인트

### 5-1. D3D9 API 호출 맵

| API 호출 | 위치 | 용도 |
|----------|------|------|
| `CreateTexture` | `TextureRenderer::SetMediaType` | 비디오 크기에 맞는 `D3DFMT_X8R8G8B8` 텍스처 생성 |
| `LockRect` / `UnlockRect` | `TextureRenderer::DoRenderSample` | 비디오 프레임을 텍스처 메모리에 직접 기록 |
| `SetTexture` | `TextureRenderer::SetTexture` | 스테이지 0에 비디오 텍스처 바인딩 |
| `SetSamplerState` | `MediaMgr::Render` | LINEAR 필터링 활성화/비활성화 |
| `SetRenderState` | `MediaMgr::Render` | CULL_NONE, LIGHTING OFF |
| `GetTransform` / `SetTransform` | `MediaMgr::Render` | View/Proj 행렬 백업 및 Identity 교체 |
| `DrawIndexedPrimitive` | `CRect_Texture::Render_Buffer` (간접) | 풀스크린 쿼드 렌더링 |

### 5-2. DirectShow COM 인터페이스 맵

| 인터페이스 | 소유자 | 용도 |
|------------|--------|------|
| `IGraphBuilder` | CMediaObj | 필터 그래프 생성/관리, 소스 추가, 자동 렌더링 |
| `IMediaControl` | CMediaObj | `Run()`, `Stop()` — 재생 제어 |
| `IMediaPosition` | CMediaObj | `get_CurrentPosition`, `put_CurrentPosition`, `get_StopTime` |
| `IMediaEvent` | CMediaObj | 이벤트 알림 (코드상 미사용, 향후 확장용) |
| `IBaseFilter` | CMediaObj | CTextureRenderer를 필터로 등록 |

### 5-3. D3DPOOL_MANAGED 선택

텍스처가 `D3DPOOL_MANAGED`로 생성됨. 매 프레임 `LockRect`로 갱신하므로 `D3DPOOL_DEFAULT` + `D3DUSAGE_DYNAMIC`이 더 적합하나, DX9의 Managed Pool이 자동 복구(device lost 대응)를 제공하므로 안정성을 우선한 선택.

---

## 6. 사운드 서브시스템 (CSoundMgr)

비디오와 독립적인 별도 시스템이나, 미디어 시스템의 일부로 간략 정리.

### 6-1. 구조

```
CSoundMgr (Singleton)
├── m_pSystem  : FMOD_SYSTEM*              [FMOD 시스템 인스턴스]
├── m_pChannel : FMOD_CHANNEL*[CHANNEL_END] [채널별 재생 슬롯]
└── m_MapSound : map<TCHAR*, FMOD_SOUND*>   [사운드 리소스 캐시]
```

### 6-2. 주요 기능

| 메서드 | 동작 |
|--------|------|
| `LoadSoundFile` | `_findfirst`로 `../Resources/Sound/` 디렉토리 스캔, 모든 파일 로드 |
| `Play_Sound` | 채널에 사운드 배정, 볼륨 설정 |
| `PlayBGM` | `FMOD_LOOP_NORMAL`로 반복 재생 |
| `Play_Sound` (오버로드) | 거리 기반 재생 (카메라↔소스 거리 > fRange면 무시) |
| `Play_RandomSound` | `pSoundKey` + 랜덤 번호로 변형 사운드 재생 |

### 6-3. 설계 특징

- **C API 사용**: `FMOD_System_*`, `FMOD_Channel_*` — C++ 래퍼(`FMOD::System`) 미사용
- **메모리 관리**: `TCHAR*` 키를 `new[]`로 할당, `Free()`에서 `delete[]`. `std::wstring`을 쓰면 메모리 관리가 단순해짐
- **거리 감쇠 미완성**: 거리 체크만 하고 볼륨 감쇠 로직은 주석 처리 (`// 거리에 따라 소리의 감소를 추가하고 적용할 수 있음`)

---

## 7. 문제점 및 개선 인사이트

### 7-1. 스레드 안전성 (Critical)

`DoRenderSample`은 **DirectShow 워커 스레드**에서 호출되고, `SetTexture`는 **메인 렌더 스레드**에서 호출된다. 동일한 `m_pTexture`에 대해:

- 워커: `LockRect → 픽셀 복사 → UnlockRect`
- 메인: `SetTexture(stage, m_pTexture)`

**동기화 메커니즘이 없다.** `D3DPOOL_MANAGED`의 내부 동기화에 의존하는 형태이나, 이는 공식적으로 보장되지 않음. 올바른 해법:

```
방안 A: 더블 버퍼링 (텍스처 2장, swap)
방안 B: Critical Section으로 LockRect/SetTexture 상호 배제
```

### 7-2. 픽셀 복사 성능

`DoRenderSample`에서 **픽셀 단위** BGR24→BGRX32 변환:

```cpp
for (x = 0; x < m_lVidWidth; x++) {
    pTxt[0] = pBmp[0]; pTxt[1] = pBmp[1]; pTxt[2] = pBmp[2]; pTxt[3] = 0xff;
    pTxt += 4; pBmp += 3;
}
```

1920×1080 기준 초당 30프레임이면 매초 ~187MB 메모리 복사. 개선 방향:

```
방안 A: MEDIASUBTYPE_RGB32 협상 → 직접 memcpy (포맷 변환 불필요)
방안 B: SSE/AVX intrinsics로 벡터화
방안 C: DX11 전환 시 — GPU 디코딩 (Media Foundation + ID3D11VideoDevice)
```

### 7-3. VideoPlay의 new 직접 사용

```cpp
m_pMediaObj = new CMediaObj;  // ← 엔진 규칙 위반
```

`Create` 정적 팩토리 패턴을 따르지 않음. COM 객체의 특수성으로 이해할 수 있으나, `CMediaObj::Create(pGraphicDev, wFileName)`으로 래핑하면 일관성 향상.

### 7-4. IMediaEvent 미활용

`m_pME`를 `QueryInterface`로 획득하나 실제 사용하지 않음. 활용 시:

```cpp
// 재생 완료 감지
long evCode;
m_pME->WaitForCompletion(0, &evCode);
if (evCode == EC_COMPLETE) { /* 재생 끝 */ }
```

현재는 `GetCurrentPosition() >= GetStopTime()`으로 클라이언트가 직접 비교해야 함.

### 7-5. SoundMgr의 TCHAR* 키 관리

```cpp
TCHAR* pSoundKey = new TCHAR[256];  // 수동 메모리 관리
m_MapSound.insert(make_pair(pSoundKey, pSound));
```

`std::wstring`을 키로 사용하면 `new[]`/`delete[]` 불필요. 또한 `map<TCHAR*, ...>`는 포인터 비교이므로 `CTag_Finder` 커스텀 비교자가 필요한데, 이를 `wstring` 키로 바꾸면 자연스럽게 해결됨.

### 7-6. D3DPOOL 선택

| 현재 | 권장 |
|------|------|
| `D3DPOOL_MANAGED` | `D3DPOOL_DEFAULT` + `D3DUSAGE_DYNAMIC` |

`MANAGED`는 시스템 메모리에 백업 복사본을 유지하므로 비디오 텍스처처럼 매 프레임 갱신되는 리소스에는 메모리 낭비. `DYNAMIC`은 `LockRect` 시 `D3DLOCK_DISCARD` 플래그를 쓸 수 있어 GPU 파이프라인 스톨을 줄임.

---

## 8. DX11 프레임워크 적용 시 고려사항

현재 프레임워크는 DX11 기반이므로, 이 DX9 미디어 시스템을 그대로 이식할 수 없다. 핵심 차이와 대안:

### 8-1. 비디오 재생: DirectShow → Media Foundation

| 항목 | DX9 (현재) | DX11 (권장) |
|------|-----------|-------------|
| 프레임워크 | DirectShow | Media Foundation |
| 텍스처 갱신 | `LockRect` CPU 복사 | `IMFDXGIDeviceManager` GPU 직접 디코딩 |
| 포맷 협상 | RGB24 → X8R8G8B8 | NV12 → GPU 색공간 변환 |
| 스레드 모델 | 수동 (위험) | MF 내장 동기화 |

### 8-2. 사운드: FMOD 유지 가능

FMOD는 그래픽 API 독립적이므로 DX11에서도 동일하게 사용 가능. C++ 래퍼(`FMOD::System`, `FMOD::Channel`)로 전환하면 타입 안전성 향상.

### 8-3. 렌더링 통합

현재 `CMediaMgr::Render()`는 D3D9 고정 파이프라인으로 렌더링:

```cpp
SetTransform(D3DTS_VIEW, &Identity);    // DX11에는 없음
SetTexture(0, m_pTexture);               // DX11: ShaderResourceView
DrawIndexedPrimitive(...)                 // DX11: DrawIndexed
```

DX11에서는:
- 전용 셰이더 (VS/PS) 작성 필요
- 렌더러의 렌더 그룹에 `RENDER_SCREEN` 또는 `RENDER_UI` 우선순위로 등록
- `CGameInstance`를 통해 `CRenderer::Add_RenderGroup`으로 통합

---

## 9. 요약

| 영역 | 평가 | 비고 |
|------|------|------|
| 아키텍처 | 양호 | 3계층 분리 (Manager → Object → Renderer) |
| COM 통합 | 양호 | CComPtr로 안전한 수명 관리 |
| 스레드 안전성 | 미흡 | 텍스처 동시 접근 동기화 없음 |
| 성능 | 개선 여지 | 픽셀 단위 CPU 복사, MANAGED Pool |
| 엔진 규칙 준수 | 부분적 | new 직접 사용, Create 패턴 미적용 |
| DX11 이식성 | 낮음 | DirectShow + 고정 파이프라인 의존 |

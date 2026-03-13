# O06-B: Animation + Channel 키프레임 보간

## 1. 전체 구조

```
CModel
  └── vector<CAnimation*>     ← 모든 애니메이션 클립 (걷기, 달리기, 공격 등)
        └── vector<CChannel*> ← 해당 애니메이션이 제어하는 본별 키프레임
              └── vector<KEYFRAME>  ← 시간별 SRT 데이터
```

```
Play_Animation(fTimeDelta)
  ├── Animation::Update_TransformationMatrix
  │     ├── TrackPosition += TickPerSecond × TimeDelta
  │     └── for (Channel) → Channel::Update_TransformationMatrix
  │           ├── 현재 TrackPosition에 맞는 키프레임 쌍 찾기
  │           ├── Lerp(Scale), Slerp(Rotation), Lerp(Translation)
  │           └── Bone[i].TransformationMatrix = AffineTransformation
  └── for (Bone) → Update_CombinedTransformationMatrix
```

---

## 2. KEYFRAME 구조체

```cpp
typedef struct tagKeyFrame {
    XMFLOAT3 vScale;          // 스케일 (x, y, z)
    XMFLOAT4 vRotation;       // 회전 (쿼터니언 x, y, z, w)
    XMFLOAT3 vTranslation;    // 이동 (x, y, z)
    float    fTrackPosition;  // 이 키프레임의 시간 위치 (tick)
} KEYFRAME;
```

**SRT 분리 저장**: 행렬이 아닌 스케일/회전/이동을 개별로 저장한다.
이유: 회전에 **Slerp(구면 선형 보간)**을 적용하기 위해서.
행렬 자체를 Lerp하면 왜곡이 발생하지만, 쿼터니언 Slerp는 등속 회전을 보장한다.

---

## 3. CAnimation 클래스

### 멤버

```cpp
class CAnimation final : public CBase {
    _float   m_fDuration;              // 총 재생 시간 (tick)
    _float   m_fCurrentTrackPosition;  // 현재 재생 위치 (tick)
    _float   m_fTickPerSecond;         // 초당 틱 수 (재생 속도)
    _uint    m_iNumChannels;           // 채널 수
    vector<CChannel*>  m_Channels;     // 채널 배열
    vector<_uint>      m_CurrentKeyFrameIndex;  // 채널별 현재 키프레임 인덱스
};
```

### 초기화 — aiAnimation에서 생성

```cpp
HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation,
    const vector<CBone*>& Bones)
{
    m_iNumChannels  = pAIAnimation->mNumChannels;
    m_fDuration     = pAIAnimation->mDuration;       // 예: 60.0 tick
    m_fTickPerSecond = pAIAnimation->mTicksPerSecond; // 예: 30.0 tick/sec

    m_CurrentKeyFrameIndex.resize(m_iNumChannels);    // 채널별 0으로 초기화

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(
            pAIAnimation->mChannels[i], Bones);
        m_Channels.push_back(pChannel);
    }
}
```

### Update_TransformationMatrix — 재생 제어

```cpp
void CAnimation::Update_TransformationMatrix(
    _float fTimeDelta, const vector<CBone*>& Bones,
    _bool isLoop, _bool* pFinished)
{
    // ① 재생 위치 전진
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    // ② 끝에 도달
    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
        {
            *pFinished = true;  // 비반복: 종료 플래그
            return;
        }
        m_fCurrentTrackPosition = 0.f;  // 반복: 처음부터
    }

    // ③ 각 채널이 자신의 본을 갱신
    _uint iIndex = 0;
    for (auto& pChannel : m_Channels)
        pChannel->Update_TransformationMatrix(
            m_fCurrentTrackPosition, Bones,
            &m_CurrentKeyFrameIndex[iIndex++]);
}
```

### Reset

```cpp
void CAnimation::Reset() {
    m_fCurrentTrackPosition = 0.f;
    for (auto& KeyFrameIndex : m_CurrentKeyFrameIndex)
        KeyFrameIndex = 0;
}
```

`Set_Animation()`에서 애니메이션 전환 시 호출. 재생 위치와 모든 채널의
키프레임 인덱스를 초기화한다.

---

## 4. CChannel 클래스

### 멤버

```cpp
class CChannel final : public CBase {
    _char    m_szName[MAX_PATH];     // 채널 이름 (= 본 이름)
    _uint    m_iNumKeyFrames;        // 키프레임 수
    vector<KEYFRAME> m_KeyFrames;    // 키프레임 배열 (시간순)
    _uint    m_iBoneIndex;           // 이 채널이 제어하는 본의 배열 인덱스
};
```

### 초기화 — 본 인덱스 검색 + 키프레임 통합

```cpp
HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel,
    const vector<CBone*>& Bones)
{
    strcpy_s(m_szName, pAIChannel->mNodeName.data);

    // ① 이름으로 본 인덱스 검색 (find_if + Compare_Name)
    auto iter = find_if(Bones.begin(), Bones.end(),
        [&](CBone* pBone) -> _bool {
            if (pBone->Compare_Name(m_szName)) return true;
            ++m_iBoneIndex;  // 못 찾으면 인덱스 증가
            return false;
        });

    // ② 키프레임 수 = Scale/Rotation/Position 중 최대
    m_iNumKeyFrames = max(mNumPositionKeys, max(mNumScalingKeys, mNumRotationKeys));

    // ③ SRT 통합 키프레임 구축
    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME KeyFrame{};

        if (i < mNumScalingKeys)
            KeyFrame.vScale = mScalingKeys[i].mValue;

        if (i < mNumRotationKeys)
            KeyFrame.vRotation = mRotationKeys[i].mValue;  // (x,y,z,w)

        if (i < mNumPositionKeys)
            KeyFrame.vTranslation = mPositionKeys[i].mValue;

        m_KeyFrames.push_back(KeyFrame);
    }
}
```

**Assimp은 S/R/T 키를 별도 배열로 저장**하지만, 이 엔진은 같은 인덱스의
S/R/T를 하나의 `KEYFRAME`으로 통합한다. 키 수가 다른 경우 마지막 유효 값이 유지된다.

---

## 5. 키프레임 보간 — 핵심 알고리즘

```cpp
void CChannel::Update_TransformationMatrix(
    _float fCurrentTrackPosition,
    const vector<CBone*>& Bones,
    _uint* pCurrentKeyFrameIndex)
{
    // ① 재생 위치 0이면 키프레임 인덱스 리셋
    if (0.0f == fCurrentTrackPosition)
        (*pCurrentKeyFrameIndex) = 0;

    _vector vScale, vRotation, vTranslation;

    // ② 마지막 키프레임을 넘었으면 → 마지막 값 그대로
    KEYFRAME LastKeyFrame = m_KeyFrames.back();
    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale       = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation    = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    else
    {
        // ③ 현재 위치를 포함하는 키프레임 구간 찾기
        while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
            ++(*pCurrentKeyFrameIndex);

        // ④ 구간 내 보간 비율 계산
        _float fRatio =
            (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
            (m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition -
             m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

        // ⑤ 보간
        vScale       = XMVectorLerp(SourScale, DestScale, fRatio);
        vRotation    = XMQuaternionSlerp(SourRotation, DestRotation, fRatio);
        vTranslation = XMVectorLerp(SourTranslation, DestTranslation, fRatio);
    }

    // ⑥ SRT → 행렬 변환 → 본에 적용
    _matrix TransformationMatrix = XMMatrixAffineTransformation(
        vScale,
        XMVectorSet(0, 0, 0, 1),  // 회전 중심 (원점)
        vRotation,
        vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}
```

### 보간 시각화

```
시간축 (TrackPosition):
   0 ──── 10 ──── 20 ──── 30 ──── 40 ──── 50 ──── 60
   KF[0]  KF[1]  KF[2]  KF[3]  KF[4]  KF[5]  KF[6]

현재: TrackPosition = 25

① pCurrentKeyFrameIndex가 2까지 전진 (KF[3].time=30 > 25)
② fRatio = (25 - 20) / (30 - 20) = 0.5
③ Scale = Lerp(KF[2].Scale, KF[3].Scale, 0.5)
④ Rotation = Slerp(KF[2].Rot, KF[3].Rot, 0.5)
⑤ Translation = Lerp(KF[2].Trans, KF[3].Trans, 0.5)
```

### 키프레임 인덱스 캐싱

```cpp
vector<_uint> m_CurrentKeyFrameIndex;  // 채널별 마지막 검색 위치
```

**매 프레임 처음부터 검색하지 않는다.** 이전 프레임의 인덱스를 기억하고,
거기서부터 `while`로 전진한다. 시간이 단조 증가하므로 항상 앞으로만 이동.

| 상황 | 처리 |
|------|------|
| TrackPosition 정상 전진 | while로 1~2칸 전진 |
| 루프 (0으로 리셋) | `if (0.0f)` → 인덱스 0으로 리셋 |
| 마지막 프레임 초과 | 마지막 키프레임 값 고정 |

---

## 6. Lerp vs Slerp

| 보간 대상 | 함수 | 이유 |
|----------|------|------|
| Scale | `XMVectorLerp` | 선형 스케일 변화 (일반적으로 충분) |
| Rotation | `XMQuaternionSlerp` | **구면 선형 보간** — 등속 회전 보장 |
| Translation | `XMVectorLerp` | 선형 이동 (직선 경로) |

**Slerp가 필요한 이유**: 쿼터니언을 Lerp하면 중간에 속도가 변한다(가속→감속).
Slerp는 구면 위의 최단 경로를 등속으로 이동하여 자연스러운 회전을 보장한다.

### XMMatrixAffineTransformation

```cpp
// S × R × T 순서의 복합 행렬 생성
Matrix = Scale * RotationQuaternion * Translation

// 매개변수:
XMMatrixAffineTransformation(
    vScale,                        // 스케일 벡터
    XMVectorSet(0, 0, 0, 1),      // 회전 중심 (원점)
    vRotation,                     // 쿼터니언 회전
    vTranslation);                 // 이동 벡터
```

---

## 7. Clone 패턴

### CAnimation Clone

```cpp
CAnimation::CAnimation(const CAnimation& Prototype)
    : m_fDuration { Prototype.m_fDuration }
    , m_fCurrentTrackPosition { Prototype.m_fCurrentTrackPosition }
    , m_fTickPerSecond { Prototype.m_fTickPerSecond }
    , m_Channels { Prototype.m_Channels }            // 공유!
    , m_CurrentKeyFrameIndex { Prototype.m_CurrentKeyFrameIndex }
{
    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);   // AddRef로 공유
}
```

| 데이터 | Clone 방식 | 이유 |
|--------|-----------|------|
| Duration/TickPerSecond | 값 복사 | 상수 (변경 안 함) |
| CurrentTrackPosition | 값 복사 | **인스턴스별 재생 위치** |
| Channels | **AddRef 공유** | 키프레임 데이터는 불변 |
| CurrentKeyFrameIndex | **값 복사 (vector)** | 인스턴스별 상태 |

**Channel은 공유, 재생 상태만 독립**: 키프레임 데이터는 모든 인스턴스가 같다.
현재 재생 위치(`m_fCurrentTrackPosition`)와 키프레임 인덱스만 각자 관리한다.

---

## 8. 핵심 정리

| 항목 | 설명 |
|------|------|
| **TrackPosition** | 매 프레임 `+= TickPerSecond × TimeDelta` |
| **루프 처리** | Duration 초과 시 0으로 리셋 또는 종료 플래그 |
| **키프레임 검색** | 캐싱된 인덱스에서 전진 (while 루프) |
| **보간** | Scale/Trans = Lerp, Rotation = **Slerp** |
| **결과 적용** | XMMatrixAffineTransformation → Bone.TransformationMatrix |
| **SRT 통합** | Assimp의 분리된 S/R/T 키를 같은 인덱스로 합침 |
| **Clone** | Channel 공유(AddRef), TrackPosition/KeyFrameIndex 독립 |

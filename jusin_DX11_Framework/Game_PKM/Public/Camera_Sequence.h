#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "Camera_Defines.h"

NS_BEGIN(Game_PKM)

/* M3: 카메라 컷 시퀀스 — 여러 CAMERA_SHOT_DESC 를 순서대로 재생하는 데이터 holder.
   CBase 상속(ref counted), GameObject 가 아님(Layer 안 들어감, Director 가 strong ref 보유).

   사용 흐름:
	 CCamera_Sequence* pSeq = CCamera_Sequence::Create();
	 pSeq->Push_Shot(...);
	 pSeq->Push_Shot(...);
	 CCamera_Director::GetInstance()->Play_Sequence(pSeq);
	 Safe_Release(pSeq);   // Director 가 AddRef 했으므로 caller 는 자기 ref 만 release.

   Director::Tick 이 매 프레임 본 객체의 Tick 을 호출 → cursor 자동 진행. */
class CCamera_Sequence final : public CBase
{
private:
	CCamera_Sequence() = default;
	virtual ~CCamera_Sequence() = default;

public:
	HRESULT Initialize();

	/* 컷을 끝에 추가. 호출 순서 = 재생 순서. */
	void Push_Shot(const CAMERA_SHOT_DESC& shot);

	/* m_fElapsedInShot 누적, fDuration 도달 시 cursor 다음으로. 마지막 shot 종료 시
m_bFinished=true. */
	void Tick(_float fTimeDelta);

	/* 현재 재생 중인 shot. m_bFinished 이거나 cursor 가 범위 밖이면 nullptr. */
	const CAMERA_SHOT_DESC* Get_Current_Shot() const;

	/* 현재 shot 안에서 경과 시간(0..fDuration). FOLLOW_LOOKAT 등 M4+ 분기에서 활용. */
	_float Get_Elapsed_In_Shot() const { return m_fElapsedInShot; }

	_bool Is_Finished() const { return m_bFinished; }

	/* 처음부터 재생 (같은 시퀀스를 다시 사용할 때). */
	void Reset();

	/* 모든 shot 의 fDuration 합. Battle Step 의 Is_Complete 시간 판정(M6) 에 사용. */
	_float Get_Total_Duration() const;

public:
	static CCamera_Sequence* Create();

private:
	vector<CAMERA_SHOT_DESC> m_Shots;
	size_t                   m_iCursor = 0;
	_float                   m_fElapsedInShot = 0.f;
	_bool                    m_bFinished = false;

private:
	virtual void Free() override;
};

NS_END
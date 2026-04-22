#include "Base.h"

CBase::CBase()
{
}

CBase::CBase(const CBase& Prototype)
	: m_iRefCnt{ 0 }
{
}

_uint CBase::AddRef()
{
	return m_iRefCnt.fetch_add(1, memory_order_relaxed) + 1;
}

_uint CBase::Release()
{
	_uint iCur = m_iRefCnt.load(std::memory_order_acquire);
	while (0 != iCur)
	{
		//iCur != 0 이면 -1 시도
		// - 성공 : 감산 전 값 반환
		// - 실패 : iCur에 실제 값이 다시 채워지고 재검사
		if (m_iRefCnt.compare_exchange_weak(
			iCur, iCur - 1,
			std::memory_order_acq_rel,   // 성공 시: 감산 결과를 다른 스레드가 보도록
			std::memory_order_acquire))  // 실패 시: 최신 cur을 읽어오는 용도
		{
			return iCur;
		}
	}

	// iCur == 0이 확정된 상태 : 삭제
	Free();
	delete this;
	return 0;
}

void CBase::Free()
{
}
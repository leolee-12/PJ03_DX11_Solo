#pragma once

#include "Engine_Defines.h"


BEGIN(Engine)

#pragma region 게이지

/// <summary>
/// fCur, fMax값을 저장하고, 관련 기능을 가진 객체
/// </summary>
class FGauge
{
public:
	FGauge() : fMax(_float()), fCur(_float()), fPrevCur(fCur) {}
	FGauge(_float _fMax, _bool bMax = false) 
		: fMax(_fMax), fCur(_float(_float(bMax)* _float(_fMax))) {}
	FGauge(const FGauge& rhs) : fMax(rhs.fMax), fCur(rhs.fCur), fPrevCur(rhs.fPrevCur) {}
	~FGauge() = default;

public:
	_float fMax = { 0.f }, fCur = { 0.f };
private:
	_float fPrevCur = { 0.f };

	// 다음 Increase 때 Cur값을 초기화
	_bool	bReserveReset = { false };


public:
	// Readjust 연산자 오버로딩
	FGauge operator =(_float fValue)
	{
		Readjust(fValue);
		return (*this);
	}
	 
public:
	// 값 업데이트 및 맥스값 도달시 반환
	_bool Increase(const _float&& increase) { return Increase(increase); }
	_bool Increase(const _float& increase)
	{
		// 예약적용
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= fMax)
		{
			fCur = fMax;
			return true;
		}
		else if (fCur < 0.f)
			fCur = 0.f;

		return false;
	}

	// 값 업데이트 및 맥스값 도달시 반환
	_bool Increase_MaxOnce(const _float&& increase) { return Increase_MaxOnce(increase); }
	_bool Increase_MaxOnce(const _float& increase)
	{
		// 예약적용
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= fMax)
		{
			fCur = fMax;
			return IsMax_Once();
		}
		else if (fCur < 0.f)
			fCur = 0.f;

		return false;
	}

	// 값 감소 업데이트
	_bool Decrease(const _float&& increase) { return Decrease(increase); }
	_bool Decrease(const _float& increase)
	{
		// 예약적용
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur -= increase;
		if (fCur <= 0.f)
		{
			fCur = 0.f;
			return true;
		}
		else if (fCur >= fMax)
			fCur = fMax;

		return false;
	}

	// 특정 포인트(지점)를 넘어가면 반환
	_bool Increase_UpToPoint(const _float&& increase, const _float&& point) 
	{ return Increase_UpToPoint(increase, point); }
	_bool Increase_UpToPoint(const _float& increase, const _float& point)
	{
		// 예약적용
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= point)
		{
			fCur = point;
			return true;
		}
		else if (fCur <= 0.f)
			fCur = 0.f;

		return false;
	}

	// 특정 포인트(지점)를 넘어가면 반환, 한번 도달해도 끝까지 증가한다.
	_bool Increase_UpToPointOnce(_float increase, _float point)
	{
		// 예약적용
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f))
		{
			if (fCur >= fMax)
			{
				fCur = fMax;
			}
			return true;
		}
		else if (fCur <= 0.f)
			fCur = 0.f;

		return false;
	}

	// 현재값 초기화
	void Reset()
	{
		fCur = _float();
		fPrevCur = fCur;
	}

	void Reset(_float vResetValue)
	{
		fCur = vResetValue;
		fPrevCur = fCur;
	}

	void ReserveReset()
	{
		bReserveReset = true;
	}

	void Set_Max()
	{
		Reset(fMax);
	}

	// 현재값이 반전됨
	void Reverse()
	{
		fCur = (fMax - fCur);
		fPrevCur = fCur;
	}

	// fMax 값 재설정 및 현재값 초기화
	void Readjust(_float _fMax)
	{
		fMax = _fMax;
		fCur = _float();
	}

	// 특정 포인트(지점)를 넘어가면 계속 트루
	_bool IsUpTo(_float point) const
	{
		return (fCur >= point);
	}

	// 증가값과 체크하고자 하는 값으로 한번만 지나갈 때를 체크합니다.
	_bool IsUpTo_Once(_float point, _float increase) const
	{
		return ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f));
	}

	// 맥스일 경우 계속 트루
	_bool IsMax() const
	{
		return (fCur >= fMax);
	}

	// 맥스일 경우 한번만 트루
	_bool IsMax_Once() const
	{
		return (fCur >= fMax && fPrevCur != fCur);
	}

	_bool IsZero_Once() const
	{
		return (fCur <= fMax && fPrevCur != fCur);
	}

	// 퍼센트 값 반환
	_float Get_Percent() const
	{
		return (Cast<_float>(fCur) / Cast<_float>(fMax));
	}

private:
	_bool AssignReserveReset()
	{
		if (bReserveReset)
		{
			bReserveReset = !bReserveReset;
			return true;
		}

		return false;
	}
};

using FDelay = FGauge;

#pragma endregion


#pragma region 왕복 게이지

/// <summary>
/// fCur, fMin, fMax값을 저장하고
/// 저 사이를 fCur이 왕복하는 형태의 클래스이다.
/// </summary>
class FRoundTripGauge
{
public:
	explicit FRoundTripGauge() {}
	explicit FRoundTripGauge(_float _fMin, _float _fMax) 
		: fMax(_fMax), fMin(_fMin) {}
	FRoundTripGauge(const FRoundTripGauge& rhs) 
		: fMax(rhs.fMax), fCur(rhs.fCur), fPrevCur(rhs.fPrevCur) 
		, fMin(rhs.fMin), bIsLoop(rhs.bIsLoop)
	{}
	~FRoundTripGauge() = default;

public:
	_float fMin = { 0.f }, fMax = { 0.f }, fCur = { 0.f };

private:
	_float	fPrevCur = { 0.f };			// 이전값
	_byte	bSign = { 1 };				// 방향성
	_bool	bIsLoop = { false };		// 반복 설정
	_bool	bIsEnd = { false };			// 종료

public:
	// 값 업데이트, Max지점, Min 지점에서 한번씩 True를 반환함
	_bool Update(_float value)
	{
		fPrevCur = fCur;
		if (!bIsEnd)
		{
			// 방향성에 따라 값을 업데이트
			fCur += value * _float(bSign);
			if (fCur >= fMax && bSign == 1)
			{
				fCur = fMax;
				bSign = -1;

				return true;
			}
			else if (fCur <= fMin && bSign == -1)
			{
				fCur = fMin;
				bSign = 1;

				// 루프 설정이 꺼져있으면 끝낸다.
				if (!bIsLoop)
					bIsEnd = true;

				return true;
			}
		}

		return false;
	}

	void Loop(_bool bLoop)
	{
		bIsEnd = false;
		bIsLoop = bLoop;
	}

	// 현재값 초기화
	void Reset()
	{
		bIsEnd = false;
		fCur = _float();
		fPrevCur = fCur;
	}

	// fMax 값 재설정 및 현재값 초기화
	void Readjust(_float _fMin, _float _fMax)
	{
		fMin = _fMin;
		fMax = _fMax;
		fCur = _float();
	}

	// 특정 포인트(지점)를 넘어가면 계속 트루
	_bool IsUpTo(_float point)
	{
		return (fCur >= point);
	}

	// 증가값과 체크하고자 하는 값으로 한번만 지나갈 때를 체크합니다.
	_bool IsUpTo_Once(_float point, _float increase)
	{
		return ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f));
	}

	_bool IsFinished()
	{
		return bIsEnd;
	}

	// 맥스일 경우 계속 트루
	_bool IsMax()
	{
		return (fCur >= fMax);
	}

	// 맥스일 경우 한번만 트루
	_bool IsMax_Once()
	{
		return (fCur >= fMax && fPrevCur != fCur);
	}

	// 퍼센트 값 반환
	_float Percent() const
	{
		return (Cast<_float>(fCur) / Cast<_float>(fMax));
	}

	// 1을 기준으로 퍼센트를 구함, Max값 무시
	_float SaturatedPercent() const
	{
		return max(0.f, min(Cast<_float>(fCur), 1.f));
	}

	// Value값 기준으로 퍼센트를 구함, Max값 무시
	_float ClampPercent(const _float fValue) const
	{
		return max(0.f, min(Cast<_float>(fCur) / fValue, 1.f));
	}
};

#pragma endregion


#pragma region 시간 체커

/// <summary>
/// 시간이 지났는지 체킹하는 소자
/// </summary>
class FTimeChecker
{
public:
	FTimeChecker() {}
	FTimeChecker(_float fInterval) 
		: fMax(fInterval) {}
	FTimeChecker(const FTimeChecker& rhs) 
		: fTime(rhs.fTime), fMax(rhs.fMax) {}
	~FTimeChecker() {}

private:
	_float	fTime = { 0.f };
	_float	fMax = { 0.f };
	_bool	bTicked = { false };

public:
	FTimeChecker operator =(_float fValue)
	{
		fMax = fValue;
		fTime = 0.f;
		bTicked = false;

		return (*this);
	}

public:
	_bool Update(_cref_time fTimeDelta)
	{
		if (bTicked)
			bTicked = false;

		_bool bIsChecked = false;
		fTime += fTimeDelta;
		while (fTime >= fMax)
		{
			fTime -= fMax;
			bIsChecked = true;
		}

		return bTicked = bIsChecked;
	}

	_bool IsTicked()
	{
		return bTicked;
	}

};

#pragma endregion


END
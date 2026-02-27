#pragma once

#include "Engine_Defines.h"

//////////////////////////////////////////////////
// https://easings.net/ko						//
// Ctrl + 좌클릭으로 링크들어가면 그래프 모양 나옴.//
//////////////////////////////////////////////////


BEGIN(Engine)
class ENGINE_DLL CEase
{
public:
	enum EASE {
		IN_QUAD, OUT_QUAD, INOUT_QUAD_,
		IN_SINE, OUT_SINE, INOUT_SINE,
		IN_QUINT, OUT_QUINT, INOUT_QUINT,
		IN_QUART, OUT_QUART, INOUT_QUART,
		NONE_LINEAR, IN_LINEAR, OUT_LINEAR, INOUT_LINEAR,
		IN_EXPO, OUT_EXPO, INOUT_EXPO,
		IN_ELASTIC, OUT_ELASTIC, INOUT_ELASTIC,
		IN_CUBIC, OUT_CUBIC, INOUT_CUBIC,
		IN_CIRC, OUT_CIRC, INOUT_CIRC,
		IN_BOUNCE, OUT_BOUNCE, INOUT_BOUNCE,
		IN_BACK, OUT_BACK, INOUT_BACK,
		EASE_END
	};
	static const char* pEases[EASE_END];
	static const _tchar* pTEases[EASE_END];
	// elapsedTime : 애니메이션 시작부터 현재까지의 경과 시간을 나타냅니다.
	// startValue : 애니메이션의 시작 값입니다.
	// changeAmount : 시작 값에서 최종 값까지의 변동량입니다.
	// totalDuration : 애니메이션의 총 지속 시간입니다.

	// 사용 예시 :
	// 객체가 50 위치에서 시작하여 150 위치로 2초 동안 부드럽게 움직이는 애니메이션을 만들고자 할 때 :
	// m_fTimeAcc += _fTimeDelta;
	// float fResult = CEase::OutQuad(m_fTimeAcc, 50, 100, 2);
	// 세 번째 인자(변화량) = 마지막 위치 - 처음 위치 = 150 - 50 = 100이다.

		// Quad
	static float InQuad(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * (elapsedTime /= totalDuration) * elapsedTime + startValue;
	}
	static float OutQuad(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return -changeAmount * (elapsedTime /= totalDuration) * (elapsedTime - 2) + startValue;
	}
	static float InOutQuad(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if ((elapsedTime /= totalDuration / 2) < 1) return ((changeAmount / 2) * (elapsedTime * elapsedTime)) + startValue;
		return -changeAmount / 2 * (((elapsedTime - 2) * (--elapsedTime)) - 1) + startValue;
	}

	// Sine
	static float InSine(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return -changeAmount * cos(elapsedTime / totalDuration * (XM_PI / 2)) + changeAmount + startValue;
	}
	static float OutSine(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * sin(elapsedTime / totalDuration * (XM_PI / 2)) + startValue;
	}
	static float InOutSine(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return -changeAmount / 2 * (cos(XM_PI * elapsedTime / totalDuration) - 1) + startValue;
	}

	// Quint
	static float InQuint(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * (elapsedTime /= totalDuration) * elapsedTime * elapsedTime * elapsedTime * elapsedTime + startValue;
	}
	static float OutQuint(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * ((elapsedTime = elapsedTime / totalDuration - 1) * elapsedTime * elapsedTime * elapsedTime * elapsedTime + 1) + startValue;
	}
	static float InOutQuint(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if ((elapsedTime /= totalDuration / 2) < 1) return changeAmount / 2 * elapsedTime * elapsedTime * elapsedTime * elapsedTime * elapsedTime + startValue;
		return changeAmount / 2 * ((elapsedTime -= 2) * elapsedTime * elapsedTime * elapsedTime * elapsedTime + 2) + startValue;
	}

	// Quart
	static float InQuart(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * (elapsedTime /= totalDuration) * elapsedTime * elapsedTime * elapsedTime + startValue;
	}
	static float OutQuart(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return -changeAmount * ((elapsedTime = elapsedTime / totalDuration - 1) * elapsedTime * elapsedTime * elapsedTime - 1) + startValue;
	}
	static float InOutQuart(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if ((elapsedTime /= totalDuration / 2) < 1) return changeAmount / 2 * elapsedTime * elapsedTime * elapsedTime * elapsedTime + startValue;
		return -changeAmount / 2 * ((elapsedTime -= 2) * elapsedTime * elapsedTime * elapsedTime - 2) + startValue;
	}

	// 사이트에 없는 모양인데 그냥 알아서 생각하셈
	// Linear
	static float NoneLinear(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * elapsedTime / totalDuration + startValue;
	}
	static float InLinear(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * elapsedTime / totalDuration + startValue;
	}
	static float OutLinear(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * elapsedTime / totalDuration + startValue;
	}
	static float InOutLinear(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * elapsedTime / totalDuration + startValue;
	}

	// Expo
	static float InExpo(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return (elapsedTime == 0) ? startValue : changeAmount * powf(2, 10 * (elapsedTime / totalDuration - 1)) + startValue;
	}
	static float OutExpo(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return (elapsedTime == totalDuration) ? startValue + changeAmount : changeAmount * (-powf(2, -10 * elapsedTime / totalDuration) + 1) + startValue;
	}
	static float InOutExpo(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if (elapsedTime == 0) return startValue;
		if (elapsedTime == totalDuration) return startValue + changeAmount;
		if ((elapsedTime /= totalDuration / 2) < 1) return changeAmount / 2 * powf(2, 10 * (elapsedTime - 1)) + startValue;
		return changeAmount / 2 * (-powf(2, -10 * --elapsedTime) + 2) + startValue;
	}

	// Elastic
	static float InElastic(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if (elapsedTime == 0) return startValue;  if ((elapsedTime /= totalDuration) == 1) return startValue + changeAmount;
		float p = totalDuration * .3f;
		float a = changeAmount;
		float s = p / 4;
		float postFix = a * powf(2, 10 * (elapsedTime -= 1)); // this is a fix, again, with post-increment operators
		return -(postFix * sinf((elapsedTime * totalDuration - s) * (2 * XM_PI) / p)) + startValue;
	}
	static float OutElastic(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if (elapsedTime == 0) return startValue;  if ((elapsedTime /= totalDuration) == 1) return startValue + changeAmount;
		float p = totalDuration * .3f;
		float a = changeAmount;
		float s = p / 4;
		return (a * powf(2, -10 * elapsedTime) * sinf((elapsedTime * totalDuration - s) * (2 * XM_PI) / p) + changeAmount + startValue);
	}
	static float InOutElastic(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if (elapsedTime == 0) return startValue;  if ((elapsedTime /= totalDuration / 2) == 2) return startValue + changeAmount;
		float p = totalDuration * (.3f * 1.5f);
		float a = changeAmount;
		float s = p / 4;

		if (elapsedTime < 1) {
			float postFix = a * powf(2, 10 * (elapsedTime -= 1)); // postIncrement is evil
			return -.5f * (postFix * sinf((elapsedTime * totalDuration - s) * (2 * XM_PI) / p)) + startValue;
		}
		float postFix = a * powf(2, -10 * (elapsedTime -= 1)); // postIncrement is evil
		return postFix * sinf((elapsedTime * totalDuration - s) * (2 * XM_PI) / p) * .5f + changeAmount + startValue;
	}

	// Cubic
	static float InCubic(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * (elapsedTime /= totalDuration) * elapsedTime * elapsedTime + startValue;
	}
	static float OutCubic(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * ((elapsedTime = elapsedTime / totalDuration - 1) * elapsedTime * elapsedTime + 1) + startValue;
	}
	static float InOutCubic(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if ((elapsedTime /= totalDuration / 2) < 1) return changeAmount / 2 * elapsedTime * elapsedTime * elapsedTime + startValue;
		return changeAmount / 2 * ((elapsedTime -= 2) * elapsedTime * elapsedTime + 2) + startValue;
	}

	// Circ
	static float InCirc(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return -changeAmount * (sqrtf(1 - (elapsedTime /= totalDuration) * elapsedTime) - 1) + startValue;
	}
	static float OutCirc(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount * sqrtf(1 - (elapsedTime = elapsedTime / totalDuration - 1) * elapsedTime) + startValue;
	}
	static float InOutCirc(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if ((elapsedTime /= totalDuration / 2) < 1) return -changeAmount / 2 * (sqrtf(1 - elapsedTime * elapsedTime) - 1) + startValue;
		return changeAmount / 2 * (sqrtf(1 - elapsedTime * (elapsedTime -= 2)) + 1) + startValue;
	}

	// Bounce
	static float InBounce(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		return changeAmount - OutBounce(totalDuration - elapsedTime, 0, changeAmount, totalDuration) + startValue;
	}
	static float OutBounce(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if ((elapsedTime /= totalDuration) < (1 / 2.75f)) {
			return changeAmount * (7.5625f * elapsedTime * elapsedTime) + startValue;
		}
		else if (elapsedTime < (2 / 2.75f)) {
			float postFix = elapsedTime -= (1.5f / 2.75f);
			return changeAmount * (7.5625f * (postFix)*elapsedTime + .75f) + startValue;
		}
		else if (elapsedTime < (2.5 / 2.75)) {
			float postFix = elapsedTime -= (2.25f / 2.75f);
			return changeAmount * (7.5625f * (postFix)*elapsedTime + .9375f) + startValue;
		}
		else {
			float postFix = elapsedTime -= (2.625f / 2.75f);
			return changeAmount * (7.5625f * (postFix)*elapsedTime + .984375f) + startValue;
		}
	}
	static float InOutBounce(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		if (elapsedTime < totalDuration / 2) return InBounce(elapsedTime * 2, 0, changeAmount, totalDuration) * .5f + startValue;
		else return OutBounce(elapsedTime * 2 - totalDuration, 0, changeAmount, totalDuration) * .5f + changeAmount * .5f + startValue;
	}

	// Back
	static float InBack(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		float s = 1.70158f;
		float postFix = elapsedTime /= totalDuration;
		return changeAmount * (postFix)*elapsedTime * ((s + 1) * elapsedTime - s) + startValue;
	}
	static float OutBack(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		float s = 1.70158f;
		return changeAmount * ((elapsedTime = elapsedTime / totalDuration - 1) * elapsedTime * ((s + 1) * elapsedTime + s) + 1) + startValue;
	}
	static float InOutBack(float elapsedTime, float startValue, float changeAmount, float totalDuration) {
		float s = 1.70158f;
		if ((elapsedTime /= totalDuration / 2) < 1) return changeAmount / 2 * (elapsedTime * elapsedTime * (((s *= (1.525f)) + 1) * elapsedTime - s)) + startValue;
		float postFix = elapsedTime -= 2;
		return changeAmount / 2 * ((postFix)*elapsedTime * (((s *= (1.525f)) + 1) * elapsedTime + s) + 2) + startValue;
	}

	static const char* pEase[EASE_END];

	// 러프 용 주석 (4번째 매개변수를 파워로 사용할 수 있음)
	// CEase::타입, Acc, 0, 1, 러프 시간
	static float Ease(EASE eEase, float elapsedTime, float startValue, float changeAmount, float totalDuration)
	{
		switch (eEase)
		{
		case Engine::CEase::IN_QUAD:
			return InQuad(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_QUAD:
			return OutQuad(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_QUAD_:
			return InOutQuad(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_SINE:
			return InSine(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_SINE:
			return OutSine(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_SINE:
			return InOutSine(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_QUINT:
			return InQuint(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_QUINT:
			return OutQuint(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_QUINT:
			return InOutQuint(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_QUART:
			return InQuart(elapsedTime, startValue, changeAmount, totalDuration); break;
		case Engine::CEase::OUT_QUART:
			return OutQuart(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_QUART:
			return InOutQuart(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::NONE_LINEAR:
			return NoneLinear(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_LINEAR:
			return InLinear(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_LINEAR:
			return OutLinear(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_LINEAR:
			return InOutLinear(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_EXPO:
			return InExpo(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_EXPO:
			return OutExpo(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_EXPO:
			return InOutExpo(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_ELASTIC:
			return InElastic(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_ELASTIC:
			return OutElastic(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_ELASTIC:
			return InOutElastic(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_CUBIC:
			return InCubic(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_CUBIC:
			return OutCubic(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_CUBIC:
			return InOutCubic(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_CIRC:
			return InCirc(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_CIRC:
			return OutCirc(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_CIRC:
			return InOutCirc(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_BOUNCE:
			return InBounce(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_BOUNCE:
			return OutBounce(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_BOUNCE:
			return InOutBounce(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::IN_BACK:
			return InBack(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::OUT_BACK:
			return OutBack(elapsedTime, startValue, changeAmount, totalDuration);
		case Engine::CEase::INOUT_BACK:
			return InOutBack(elapsedTime, startValue, changeAmount, totalDuration);
		}

		return 0.f;
	}
};
END

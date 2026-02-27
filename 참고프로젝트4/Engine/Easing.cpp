#include "Easing.h"

namespace Engine {

	const char* CEasing::pEases[EASE_END] = { 
		"InQuad", "OutQuad", "InOutQuad"
		, "InSine", "OutSine", "InOutSine"
		, "InQuint", "OutQuint", "InOutQuint"
		, "InQuart", "OutQuart", "InOutQuart"
		, "NoneLinear", "InLinear", "OutLinear", "InOut_Linear"
		, "InExpo", "OutExpo", "InOutExpo"
		, "InElastic", "OutElastic", "InOutElastic"
		, "InCubic", "OutCubic", "InOutCubic"  
		, "InCirc", "OutCirc", "InOutCirc"
		, "InBounce", "OutBounce", "InOutBounce"
		, "InBack", "OutBack", "InOutBack"
	};

	const _tchar* CEasing::pTEases[EASE_END] = {
	TEXT("InQuad"), TEXT("OutQuad"), TEXT("InOutQuad")
	, TEXT("InSine"),	TEXT("OutSine"),	TEXT("InOutSine")
	, TEXT("InQuint"),	TEXT("OutQuint"),	TEXT("InOutQuint")
	, TEXT("InQuart"),	TEXT("OutQuart"),	TEXT("InOutQuart")
	, TEXT("NoneLinear"), TEXT("InLinear"),	TEXT("OutLinear"), TEXT("InOut_Linear")
	, TEXT("InExpo"),	TEXT("OutExpo"),	TEXT("InOutExpo")
	, TEXT("InElastic"),TEXT("OutElastic"),	TEXT("InOutElastic")
	, TEXT("InCubic"),	TEXT("OutCubic"),	TEXT("InOutCubic")
	, TEXT("InCirc"),	TEXT("OutCirc"),	TEXT("InOutCirc")
	, TEXT("InBounce"),	TEXT("OutBounce"),	TEXT("InOutBounce")
	, TEXT("InBack"),	TEXT("OutBack"),	TEXT("InOutBack")
	};
}

_float CEasing::Ease_Float(EASE eEase, _float elapsedTime, _float startValue, _float changeAmount, _float totalDuration)
{
	return Ease(eEase, elapsedTime, startValue, changeAmount, totalDuration);
}

_float2 CEasing::Ease_Float2(EASE eEase, _float elapsedTime, _float2 startValue, _float2 changeAmount, _float totalDuration)
{
	_float2 vResult;

	vResult.x = Ease(eEase, elapsedTime, startValue.x, changeAmount.x, totalDuration);
	vResult.y = Ease(eEase, elapsedTime, startValue.y, changeAmount.y, totalDuration);

	return vResult;
}

_float3 CEasing::Ease_Float3(EASE eEase, _float elapsedTime, _float3 startValue, _float3 changeAmount, _float totalDuration)
{
	_float3 vResult;

	vResult.x = Ease(eEase, elapsedTime, startValue.x, changeAmount.x, totalDuration);
	vResult.y = Ease(eEase, elapsedTime, startValue.y, changeAmount.y, totalDuration);
	vResult.z = Ease(eEase, elapsedTime, startValue.z, changeAmount.z, totalDuration);

	return vResult;
}

_float4 CEasing::Ease_Float4(EASE eEase, _float elapsedTime, _float4 startValue, _float4 changeAmount, _float totalDuration)
{
	_float4 vResult;

	vResult.x = Ease(eEase, elapsedTime, startValue.x, changeAmount.x, totalDuration);
	vResult.y = Ease(eEase, elapsedTime, startValue.y, changeAmount.y, totalDuration);
	vResult.z = Ease(eEase, elapsedTime, startValue.z, changeAmount.z, totalDuration);
	vResult.w = Ease(eEase, elapsedTime, startValue.w, changeAmount.w, totalDuration);

	return vResult;
}

#pragma once
#include "Game_PKM_Defines.h"
#include <vector>
#include <algorithm>

NS_BEGIN(Game_PKM)

/* lerp helpers — XMFLOAT3/4는 산술 연산자가 없으므로 컴포넌트별로 명시적 처리 */
inline _float  CurveLerp(_float a, _float b, _float w) { return a + (b - a) * w; }

inline _float3 CurveLerp(const _float3& a, const _float3& b, _float w)
{
	return _float3(a.x + (b.x - a.x) * w,
		a.y + (b.y - a.y) * w,
		a.z + (b.z - a.z) * w);
}

inline _float4 CurveLerp(const _float4& a, const _float4& b, _float w)
{
	return _float4(a.x + (b.x - a.x) * w,
		a.y + (b.y - a.y) * w,
		a.z + (b.z - a.z) * w,
		a.w + (b.w - a.w) * w);
}

template <typename T>
struct CurveKey
{
	_float t = 0.f;
	T      v = {};
};

template <typename T>
class CCurve final
{
public:
	void Add_Key(_float t, const T& v)
	{
		m_Keys.push_back(CurveKey<T>{ t, v });
		std::sort(m_Keys.begin(), m_Keys.end(),
			[](const CurveKey<T>& a, const CurveKey<T>& b) { return a.t < b.t; });
	}

	void  Clear() { m_Keys.clear(); }
	_bool IsEmpty() const { return m_Keys.empty(); }
	_uint Get_KeyCount() const { return static_cast<_uint>(m_Keys.size()); }
	const std::vector<CurveKey<T>>& Get_Keys() const { return m_Keys; }

	T Sample(_float t01) const
	{
		if (m_Keys.empty())              return T{};
		if (m_Keys.size() == 1)          return m_Keys.front().v;
		if (t01 <= m_Keys.front().t)     return m_Keys.front().v;
		if (t01 >= m_Keys.back().t)      return m_Keys.back().v;

		for (size_t i = 1; i < m_Keys.size(); ++i)
		{
			if (t01 <= m_Keys[i].t)
			{
				const _float fSpan = m_Keys[i].t - m_Keys[i - 1].t;
				const _float fW = (fSpan > 0.0001f)
					? (t01 - m_Keys[i - 1].t) / fSpan
					: 0.f;
				return CurveLerp(m_Keys[i - 1].v, m_Keys[i].v, fW);
			}
		}
		return m_Keys.back().v;
	}

private:
	std::vector<CurveKey<T>> m_Keys;  // t 오름차순 유지
};

using CCurveFloat = CCurve<_float>;
using CCurveVec3 = CCurve<_float3>;
using CCurveColor = CCurve<_float4>;

NS_END
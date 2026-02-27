#include "Engine_Typedef.h"

//

namespace Engine
{
#pragma region _float2

	__forceinline _float
		float2::length() const noexcept
	{
		return XMVectorGetX(XMVector2Length(*this));
	}

	__forceinline float2
		float2::normalize() const noexcept
	{
		return XMVector2Normalize(*this);
	}

#pragma region Assignment Operators

	__forceinline float2&
		float2::operator = (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, _v);

		return *this;
	}

	__forceinline float2&
		float2::operator += (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorAdd(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator -= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorSubtract(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator *= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorMultiply(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator /= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorDivide(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator += (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorAdd(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator -= (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorSubtract(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator *= (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorMultiply(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator /= (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorDivide(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator *= (float _f) noexcept
	{
		XMStoreFloat2(this, XMVectorScale(XMLoadFloat2(this), _f));

		return *this;
	}

	__forceinline float2&
		float2::operator /= (float _f) noexcept
	{
		XMStoreFloat2(this, XMVectorDivide(XMLoadFloat2(this), XMVectorReplicate(_f)));

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float2
		float2::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float2
		float2::operator - () const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorScale(XMLoadFloat2(this), -1.f));

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float2
		float2::operator + (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorAdd(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator - (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorSubtract(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator * (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorMultiply(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator / (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorDivide(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator + (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorAdd(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator - (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorSubtract(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator * (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorMultiply(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator / (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorDivide(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator * (float _f) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorScale(XMLoadFloat2(this), _f));

		return out;
	}

	__forceinline float2
		float2::operator / (float _f) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorDivide(XMLoadFloat2(this), XMVectorReplicate(_f)));

		return out;
	}

	__forceinline _bool
		float2::operator == (const XMVECTOR& _v) const noexcept
	{
		return XMVector2Equal(XMLoadFloat2(this), _v);
	}

	__forceinline _bool
		float2::operator != (const XMVECTOR& _v) const noexcept
	{
		return !XMVector2Equal(XMLoadFloat2(this), _v);
	}

	__forceinline _bool
		float2::operator == (const float2& _v) const noexcept
	{
		return XMVector2Equal(XMLoadFloat2(this), XMLoadFloat2(&_v));
	}

	__forceinline _bool
		float2::operator != (const float2& _v) const noexcept
	{
		return !XMVector2Equal(XMLoadFloat2(this), XMLoadFloat2(&_v));
	}

#pragma endregion

	__forceinline
		float2::operator XMVECTOR() const noexcept
	{
		return XMLoadFloat2(this);
	}

#pragma endregion

#pragma region _float3

	__forceinline _float
		float3::length() const noexcept
	{
		return XMVectorGetX(XMVector3Length(*this));
	}

	__forceinline float3
		float3::normalize() const noexcept
	{
		return XMVector3Normalize(*this);
	}

#pragma region Assignment Operators

	__forceinline float3&
		float3::operator = (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, _v);

		return *this;
	}

	__forceinline float3&
		float3::operator += (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorAdd(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator -= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorSubtract(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator *= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorMultiply(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator /= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorDivide(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator += (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorAdd(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator -= (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator *= (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorMultiply(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator /= (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorDivide(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator *= (float _f) noexcept
	{
		XMStoreFloat3(this, XMVectorScale(XMLoadFloat3(this), _f));

		return *this;
	}

	__forceinline float3&
		float3::operator /= (float _f) noexcept
	{
		XMStoreFloat3(this, XMVectorDivide(XMLoadFloat3(this), XMVectorReplicate(_f)));

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float3
		float3::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float3
		float3::operator - () const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorScale(XMLoadFloat3(this), -1.f));

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float3
		float3::operator + (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorAdd(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator - (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorSubtract(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator * (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorMultiply(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator / (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorDivide(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator + (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorAdd(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator - (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator * (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorMultiply(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator / (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorDivide(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator * (float _f) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorScale(XMLoadFloat3(this), _f));

		return out;
	}

	__forceinline float3
		float3::operator / (float _f) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorDivide(XMLoadFloat3(this), XMVectorReplicate(_f)));

		return out;
	}

	__forceinline _bool
		float3::operator == (const XMVECTOR& _v) const noexcept
	{
		return XMVector3Equal(XMLoadFloat3(this), _v);
	}

	__forceinline _bool
		float3::operator != (const XMVECTOR& _v) const noexcept
	{
		return !XMVector3Equal(XMLoadFloat3(this), _v);
	}

	__forceinline _bool
		float3::operator == (const float3& _v) const noexcept
	{
		return XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&_v));
	}

	__forceinline _bool
		float3::operator != (const float3& _v) const noexcept
	{
		return !XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&_v));
	}

#pragma endregion

	__forceinline
		float3::operator XMVECTOR() const noexcept
	{
		return XMLoadFloat3(this);
	}

#pragma endregion

#pragma region _float4

	__forceinline _float
		float4::length() const noexcept
	{
		return XMVectorGetX(XMVector4Length(*this));
	}

	__forceinline float4
		float4::normalize() const noexcept
	{
		return XMVector4Normalize(*this);
	}

#pragma region Assignment Operators

	__forceinline float4&
		float4::operator = (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, _v);

		return *this;
	}

	__forceinline float4&
		float4::operator += (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorAdd(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator -= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorSubtract(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator *= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorMultiply(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator /= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorDivide(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator += (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorAdd(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator -= (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator *= (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorMultiply(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator /= (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorDivide(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator *= (float _f) noexcept
	{
		XMStoreFloat4(this, XMVectorScale(XMLoadFloat4(this), _f));

		return *this;
	}

	__forceinline float4&
		float4::operator /= (float _f) noexcept
	{
		XMStoreFloat4(this, XMVectorDivide(XMLoadFloat4(this), XMVectorReplicate(_f)));

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float4
		float4::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float4
		float4::operator - () const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorScale(XMLoadFloat4(this), -1.f));

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float4
		float4::operator + (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorAdd(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator - (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorSubtract(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator * (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorMultiply(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator / (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorDivide(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator + (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorAdd(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator - (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator * (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorMultiply(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator / (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorDivide(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator * (float _f) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorScale(XMLoadFloat4(this), _f));

		return out;
	}

	__forceinline float4
		float4::operator / (float _f) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorDivide(XMLoadFloat4(this), XMVectorReplicate(_f)));

		return out;
	}

	__forceinline _bool
		float4::operator == (const XMVECTOR& _v) const noexcept
	{
		return XMVector4Equal(XMLoadFloat4(this), _v);
	}

	__forceinline _bool
		float4::operator != (const XMVECTOR& _v) const noexcept
	{
		return !XMVector4Equal(XMLoadFloat4(this), _v);
	}

	__forceinline _bool
		float4::operator == (const float4& _v) const noexcept
	{
		return XMVector4Equal(XMLoadFloat4(this), XMLoadFloat4(&_v));
	}

	__forceinline _bool
		float4::operator != (const float4& _v) const noexcept
	{
		return !XMVector4Equal(XMLoadFloat4(this), XMLoadFloat4(&_v));
	}

#pragma endregion

	__forceinline
		float4::operator XMVECTOR() const noexcept
	{
		return XMLoadFloat4(this);
	}

#pragma endregion

#pragma region _float4x4

	__forceinline float4x4
		float4x4::inverse() const noexcept
	{
		return XMMatrixInverse(nullptr, *this);
	}

	__forceinline float4
		float4x4::row(_uint _index) const noexcept
	{
		float4 out;
		XMStoreFloat4(&out, XMLoadFloat4x4(this).r[_index]);

		return out;
	}

#pragma region Assignment Operators

	__forceinline float4x4&
		float4x4::operator = (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator += (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) + _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator -= (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) - _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator *= (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) * _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator += (const float4x4& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) + XMLoadFloat4x4(&_m));

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator -= (const float4x4& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) - XMLoadFloat4x4(&_m));

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator *= (const float4x4& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) * XMLoadFloat4x4(&_m));

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator *= (float _f) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) * _f);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator /= (float _f) noexcept
	{
		assert(_f != 0.f);

		XMStoreFloat4x4(this, XMLoadFloat4x4(this) / _f);

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float4x4
		float4x4::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float4x4
		float4x4::operator - () const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * -1.f);

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float4x4
		float4x4::operator + (const XMMATRIX& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) + _m);

		return out;
	}

	__forceinline float4x4
		float4x4::operator - (const XMMATRIX& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) - _m);

		return out;
	}

	__forceinline float4x4
		float4x4::operator * (const XMMATRIX& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * _m);

		return out;
	}

	__forceinline float4x4
		float4x4::operator + (const float4x4& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) + XMLoadFloat4x4(&_m));

		return out;
	}

	__forceinline float4x4
		float4x4::operator - (const float4x4& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) - XMLoadFloat4x4(&_m));

		return out;
	}

	__forceinline float4x4
		float4x4::operator * (const float4x4& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * XMLoadFloat4x4(&_m));

		return out;
	}

	__forceinline float4x4
		float4x4::operator * (float _f) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * _f);

		return out;
	}

	__forceinline float4x4
		float4x4::operator / (float _f) const noexcept
	{
		assert(_f != 0.f);

		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) / _f);

		return out;
	}

	__forceinline _bool
		float4x4::operator == (const XMMATRIX& _m) const noexcept
	{
		_float4x4 m;

		XMStoreFloat4x4(&m, _m);

		return !memcmp(this, &m, sizeof(float4x4));
	}

	__forceinline _bool
		float4x4::operator != (const XMMATRIX& _m) const noexcept
	{
		_float4x4 m;

		XMStoreFloat4x4(&m, _m);

		return memcmp(this, &m, sizeof(float4x4));
	}

	__forceinline _bool
		float4x4::operator == (const float4x4& _m) const noexcept
	{
		return !memcmp(this, &_m, sizeof(float4x4));
	}

	__forceinline _bool
		float4x4::operator != (const float4x4& _m) const noexcept
	{
		return memcmp(this, &_m, sizeof(float4x4));
	}

#pragma endregion

	__forceinline
		float4x4::operator XMMATRIX() const noexcept
	{
		return XMLoadFloat4x4(this);
	}

#pragma endregion
}

//
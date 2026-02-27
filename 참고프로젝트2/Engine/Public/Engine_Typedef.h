#pragma once
#include <string>

namespace Engine
{
	typedef uint8_t						_ubyte;
	typedef int8_t						_byte;

	typedef unsigned short				_ushort;
	typedef short						_short;

	typedef unsigned int				_uint;
	typedef int							_int;

	typedef unsigned long				_ulong;
	typedef long						_long;

	typedef char						_char;
	typedef wchar_t						_tchar;
	typedef wstring						_wstring;

	typedef float						_float;
	typedef double						_double;
	typedef bool						_bool;

	/* 저장용 벡터와 행렬타입이다. */
	typedef XMFLOAT2					_float2;
	typedef XMFLOAT3					_float3;
	typedef XMFLOAT4					_float4;

	typedef XMFLOAT4X4					_float4x4;


	/* SIMD연산용 벡터와 행렬타입이다. */
	typedef XMVECTOR					_vector;
	typedef FXMVECTOR					_fvector;
	typedef GXMVECTOR					_gvector;
	typedef HXMVECTOR					_hvector;
	typedef CXMVECTOR					_cvector;

	typedef XMMATRIX					_matrix;
	typedef FXMMATRIX					_fmatrix;
	typedef CXMMATRIX					_cmatrix;
}



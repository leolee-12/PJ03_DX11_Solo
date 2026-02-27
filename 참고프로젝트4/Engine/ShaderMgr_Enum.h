#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

enum class EShaderType : _uint
{
	Vertex, Pixel, Geometry, Hull, Domain, Compute, Size
};

template<EShaderType Type>
struct ShaderTypeTrait;

template<>
struct ShaderTypeTrait<EShaderType::Vertex>
{
    using Type = ID3D11VertexShader;
};

template<>
struct ShaderTypeTrait<EShaderType::Pixel>
{
    using Type = ID3D11PixelShader;
};

template<>
struct ShaderTypeTrait<EShaderType::Geometry>
{
    using Type = ID3D11GeometryShader;
};

template<>
struct ShaderTypeTrait<EShaderType::Hull>
{
    using Type = ID3D11HullShader;
};

template<>
struct ShaderTypeTrait<EShaderType::Domain>
{
    using Type = ID3D11DomainShader;
};

template<>
struct ShaderTypeTrait<EShaderType::Compute>
{
    using Type = ID3D11ComputeShader;
};

template<EShaderType Type>
using ShaderType = typename ShaderTypeTrait<Type>::Type;

END
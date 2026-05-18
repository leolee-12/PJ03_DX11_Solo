sampler ClampSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = clamp;
    AddressV = clamp;
};

sampler LinearSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearSamplerBias = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
    MipLODBias = 1.f;
};

sampler PointSampler = sampler_state
{
    filter = min_mag_mip_point;
    AddressU = wrap;
    AddressV = wrap;
};

sampler MirrorSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = mirror;
    AddressV = mirror;
};

SamplerComparisonState ShadowCompareSampler
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
    ComparisonFunc = LESS_EQUAL; // refDepth <= storedDepth ¡æ "±×¸²ÀÚ ¾Æ´Ô(=ºû ¹ÞÀ½)"
};

RasterizerState RS_Wireframe
{
    FillMode = WIREFRAME;
    
};

RasterizerState RS_Default
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = SOLID;
    CullMode = FRONT;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = SOLID;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = LESS;
};

DepthStencilState DSS_Z_Disable
{
    DepthEnable = false;
    DepthWriteMask = ZERO;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};
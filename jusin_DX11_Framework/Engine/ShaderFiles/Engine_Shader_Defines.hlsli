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
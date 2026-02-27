#pragma once

#ifdef RENDERER_EXPORTS
#define RENDERER_DLL _declspec(dllexport)

#else
#define RENDERER_DLL _declspec(dllimport)

#endif


namespace Renderer
{
    enum GLOW_TYPE { GLOW_PRI, GLOW_STAR, GLOW_MAIN, GLOW_UI, GLOW_UI_MULTY, GLOW_END };
}

using namespace Renderer;

struct RENDERER_DLL DISTORTION_DESC
{
    _float4    vPosition;
    _float2    vOffSet;
    
    _float2    vScale;

    _float  fMaxTime;
    _float  fLifeTime;
    _float  fFactor;
    _float3 vDir;
    _bool isLoop;

    DISTORTION_DESC()
        : vPosition{ 0.0f, 0.0f, 0.0f, 1.0f },
        vOffSet{ 0.0f, 0.0f },
        vScale{ 1.0f, 1.0f },
        fLifeTime(0.0f),
        fFactor(0.0f),
        vDir{ 1.0f, 0.0f,0.f },
        fMaxTime(0.f),
        isLoop(false){}

    DISTORTION_DESC(const _float4& position, const _float2& offset, const _float2& scale,
        float lifeTime, float factor, const _float3& dir)
        : vPosition(position),
        vOffSet(offset),
        vScale(scale),
        fLifeTime(lifeTime),
        fMaxTime(lifeTime),
        fFactor(factor),
        vDir(dir),
        isLoop(false) {}
};
struct RENDERER_DLL GLOW_DESC
{
    string          strTagName; 
    XMFLOAT4        vGlowColor;
    float           fGlowFactor;
    unsigned int    iPassIndex;

    GLOW_DESC()
        : strTagName(""), vGlowColor{ 0.0f, 0.0f, 0.0f, 1.0f }, fGlowFactor(1.0f), iPassIndex(0) {}

    GLOW_DESC(const string& tagName, const XMFLOAT4& glowColor, float glowFactor, unsigned int passIndex)
        : strTagName(tagName), vGlowColor(glowColor), fGlowFactor(glowFactor), iPassIndex(passIndex) {}
};

struct RENDERER_DLL RENDER_OBJECT
{
public: /* For.PlayerName */
    string strName;

public: /* For.GlowFilter_Color */
    GLOW_DESC tGlowDesc;

public: /* For.Constructor */

    RENDER_OBJECT()
        : strName(""), tGlowDesc() {}

    RENDER_OBJECT(const string& name)
        : strName(name), tGlowDesc() {}

    RENDER_OBJECT(const XMFLOAT4& glowColor)
        : strName(""), tGlowDesc("", glowColor, 1.0f, 0) {} 

    RENDER_OBJECT(const string& name, const XMFLOAT4& glowColor)
        : strName(name), tGlowDesc("", glowColor, 1.0f, 0) {}

    RENDER_OBJECT(const string& name, const XMFLOAT4& glowColor, float glowFactor)
        : strName(name), tGlowDesc("", glowColor, glowFactor, 0) {}

    RENDER_OBJECT(const string& name, const XMFLOAT4& glowColor, float glowFactor, unsigned int passIndex)
        : strName(name), tGlowDesc("", glowColor, glowFactor, passIndex) {}

    RENDER_OBJECT(const string& name, const XMFLOAT4& glowColor, float glowFactor, unsigned int passIndex, const string& tagName)
        : strName(name), tGlowDesc(tagName, glowColor, glowFactor, passIndex) {}
};



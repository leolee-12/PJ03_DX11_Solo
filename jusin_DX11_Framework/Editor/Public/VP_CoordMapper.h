#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)

class CVP_CoordMapper : public CBase
{
private:
    CVP_CoordMapper() = default;
    virtual ~CVP_CoordMapper() = default;

public:
    HRESULT Initialize();

    // 매 프레임 갱신 (panel이 letterbox 결정 직후 호출)
    void Update(const ImVec2& vDisplayPos,
        const ImVec2& vDisplaySize,
        const ImVec2& vRTSize,
        const ImVec2& vDocSize,
        UI_SCALE_POLICY ePolicy);

    // 좌표 변환
    ImVec2 ScreenToDoc(const ImVec2& vScreen) const;
    void   DocRectToScreen(const _float4& rcDoc, ImVec2* pOutMin, ImVec2* pOutMax) const;

    const ImVec2& Get_DisplayPos()  const { return m_vDisplayPos; }
    const ImVec2& Get_DisplaySize() const { return m_vDisplaySize; }

private:
    ImVec2 m_vDisplayPos = {};
    ImVec2 m_vDisplaySize = {};
    ImVec2 m_vRTSize = { 1.f, 1.f };

    UICANVAS_TRANSFORM m_tTransform = {};
    UI_SCALE_POLICY    m_ePolicy = { UI_SCALE_POLICY::UNIFORM_FIT };

public:
    static CVP_CoordMapper* Create();

private:
    virtual void Free() override;
};

NS_END
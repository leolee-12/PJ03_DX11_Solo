#include "Capture_Menu.h"
#include "UISequence.h"
#include "UIButton.h"
#include "UIButton_Group.h"

CCapture_Menu::CCapture_Menu()
{
}

CCapture_Menu::MENU CCapture_Menu::Get_FocusedMENU() const
{
    if (nullptr == m_pGroup)
        return MENU::END;

    const _int iIdx = m_pGroup->Get_FocusedIndex();
    if (iIdx < 0 || iIdx >= static_cast<_int>(MENU::END))
        return MENU::END;

    return static_cast<MENU>(iIdx);
}

HRESULT CCapture_Menu::Resolve_Buttons()
{
    if (nullptr == m_pSequence)
        return E_FAIL;

    /* BTN 인덱스 ↔ uiseq widget id 매핑.
       uiseq 변경 시 이 배열만 수정. */
    static constexpr const _char* s_WidgetIds[ETOUI(MENU::END)] =
    {
                    "widget_006",  // BTN_0
                    "widget_007",  // BTN_1
                    "widget_008",  // BTN_2
                    "widget_009",  // BTN_3
    };

    for (_uint i = 0; i < ETOUI(MENU::END); ++i)
    {
        CUIObject* pObj = m_pSequence->Find_Widget(s_WidgetIds[i]);
        if (nullptr == pObj)
            return E_FAIL;

        CUIButton* pBtn = dynamic_cast<CUIButton*>(pObj);
        if (nullptr == pBtn)
            return E_FAIL;

        m_Buttons[i] = pBtn;  // weak — sequence(children) 가 소유
    }

    return S_OK;
}

HRESULT CCapture_Menu::Build_Group()
{
    m_pGroup = CUIButton_Group::Create();
    if (nullptr == m_pGroup)
        return E_FAIL;

    if (FAILED(m_pGroup->Initialize_Linear(true)))   // wrap-around
    {
        Safe_Release(m_pGroup);
        return E_FAIL;
    }

    for (CUIButton* pBtn : m_Buttons)
    {
        if (nullptr == pBtn)
        {
            Safe_Release(m_pGroup);
            return E_FAIL;
        }
        m_pGroup->Add_Button(pBtn);  // weak
    }

    return S_OK;
}

CCapture_Menu* CCapture_Menu::Create()
{
    return new CCapture_Menu();
}

void CCapture_Menu::Free()
{
    /* m_Buttons 는 weak — 별도 해제 불필요 */
    __super::Free();
}
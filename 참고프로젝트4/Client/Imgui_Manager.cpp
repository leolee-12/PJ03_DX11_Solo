#include "stdafx.h"
#ifdef _DEBUG

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"
#include "Imgui_Manager.h"

#include "ImGuizmo.h"
#include "ImGuiFileDialog.h"

#include "commdlg.h"
#include "shlwapi.h"

#include "GameInstance.h"
#include "Imgui_Window.h"

#include "Imgui_Window_MapEdit.h"
#include "Imgui_Window_EffectEdit.h"
#include "Imgui_Window_UI_Edit.h"
#include "Imgui_Window_AnimMainEdit.h"
#include "Imgui_Window_AnimSequenceEdit.h"
#include "Imgui_Window_ShaderEdit.h"
#include "Imgui_Window_Camera_Edit.h"

#include "Utility_File.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pGameInstance = GI());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("wanju.ttf", 15.f, NULL, io.Fonts->GetGlyphRangesKorean());
    // RenderDoc 할 때는 주석처리 부탁. 아니면 터집니다.

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());

    /* 여기서 툴 윈도우 추가*/

    /* For.Windows*/
    m_pWindows[TOOL_MAP].push_back(CImgui_Window_MapEdit::Create(m_GameObjectList, m_pDevice, m_pContext));
    m_pWindows[TOOL_EFFECT].push_back(CImgui_Window_EffectEdit::Create(m_GameObjectList, m_pDevice, m_pContext));
    m_pWindows[TOOL_ANIM].push_back(CImgui_Window_AnimEdit::Create(m_GameObjectList, m_pDevice, m_pContext));
    m_pWindows[TOOL_ANIM].push_back(CImgui_Window_AnimSequenceEdit::Create(m_GameObjectList, m_pDevice, m_pContext));
    m_pWindows[TOOL_UI].push_back(CImgui_Window_UI_Edit::Create(m_GameObjectList, m_pDevice, m_pContext));
    m_pWindows[TOOL_SHADER].push_back(CImgui_Window_ShaderEdit::Create(m_GameObjectList, m_pDevice, m_pContext));
    m_pWindows[TOOL_CAMERA].push_back(CImgui_Window_Camera_Edit::Create(m_GameObjectList, m_pDevice, m_pContext));

    /* For.Imgui_Matrix Proj&View*/
    m_arrProj = new _float[16];
    m_arrView = new _float[16];

    return S_OK;
}

void CImgui_Manager::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Manager::Render()
{

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    Update_ViewProj();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowSize(ImVec2(g_iWinSizeX, g_iWinSizeY), ImGuiCond_Always);
    ImGui::Begin("Dock", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    ImGuiID dockspaceID = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0, 0), dockspaceFlags);
    ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save File", ".json", "../Bin/Data/");
            }
            if (ImGui::MenuItem("Load"))
            {
                ImGuiFileDialog::Instance()->OpenDialog("LoadFileDlgKey", "Load File", ".json", "../Bin/Data/");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editer"))
        {
            if (ImGui::MenuItem("Map Editer"))
            {
                m_eCurrentTool = TOOL_MAP;
            }

            if (ImGui::MenuItem("Effect Editer"))
            {
                m_eCurrentTool = TOOL_EFFECT;
            }

            if (ImGui::MenuItem("Animation Editer"))
            {
                m_eCurrentTool = TOOL_ANIM;
            }

            if (ImGui::MenuItem("UI Editer"))
            {
                m_eCurrentTool = TOOL_UI;
            }

            if (ImGui::MenuItem("Shader Editer"))
            {
                m_eCurrentTool = TOOL_SHADER;
            }

            if (ImGui::MenuItem("Camera Editer"))
            {
                m_eCurrentTool = TOOL_CAMERA;
            }

            ImGui::EndMenu();
        }

        // 단축키
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_LeftAlt))
        {
            if (ImGui::IsKeyPressed(ImGuiKey_1))
            {
                m_eCurrentTool = TOOL_MAP;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_2))
            {
                m_eCurrentTool = TOOL_EFFECT;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_3))
            {
                m_eCurrentTool = TOOL_ANIM;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_4))
            {
                m_eCurrentTool = TOOL_UI;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_5))
            {
                m_eCurrentTool = TOOL_SHADER;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_6))
            {
                m_eCurrentTool = TOOL_CAMERA;
            }
        }

        ImGui::EndMainMenuBar();
    }

    Dialog_Active();

    /*ImVec4 vBackGroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, vBackGroundColor);*/
    //Setup_ImguiStyle();

    for (auto iter : m_pWindows[m_eCurrentTool])
        iter->Render();

    //ImGui::PopStyleColor();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return S_OK;
}

void CImgui_Manager::Setup_ImguiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.98f);
    //style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    //style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    //style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    //style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    //style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    //style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    //style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    //style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);


    for (int i = 0; i <= ImGuiCol_COUNT; i++)
    {
        ImVec4& col = style.Colors[i];
        float H, S, V;
        ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

        if (S < 0.1f)
        {
            V = 1.0f - V;
        }
        ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
        if (col.w < 1.00f)
        {
            col.w *= 0.5f;
        }
    }

}

_bool CImgui_Manager::Picking_Object(SPACETYPE eSpacetype, LAYERTYPE eLayerType, _float3* pMousePos, _int* pPickIndex, _float* pLength, shared_ptr<CGameObject>* pPickObject)
{
    _float3	vMousePos, vPickMousePos;
    _float  fLength = 0.f, fPickLength = 0.f;
    _float fMinLength = INFINITY;
    _int iIndex = 0, iPickIndex = 0;
    shared_ptr<CGameObject> PickObject = nullptr;

    for (auto iter : m_GameObjectList[eLayerType])
    {
        if ((iter)->Intersect_MousePos(eSpacetype, &vMousePos, &fLength))
        {
            if (fLength < fMinLength)
            {
                PickObject = iter;
                iPickIndex = iIndex;
                vPickMousePos = vMousePos;
                fPickLength = fLength;
#ifdef _DEBUG
                cout << "Picking 되고 있음" << endl;
#endif
            }
        }
        else {
#ifdef _DEBUG
            cout << "No Picking" << endl;
#endif
    }
        ++iIndex;
}

    if (PickObject != nullptr)
    {
        *pMousePos = vPickMousePos;
        if (pPickObject != nullptr)
            *pPickObject = PickObject;
        if (pPickIndex != nullptr)
            *pPickIndex = iPickIndex;
        if (pLength != nullptr)
            *pLength = fPickLength;

        return true;
    }
    else return false;
}


void CImgui_Manager::Update_ViewProj()
{
    _float4x4 matView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW);
    _float arrView[] = { matView._11,matView._12,matView._13,matView._14,
                         matView._21,matView._22,matView._23,matView._24,
                         matView._31,matView._32,matView._33,matView._34,
                         matView._41,matView._42,matView._43,matView._44 };
    memcpy(m_arrView, arrView, sizeof(arrView));

    _float4x4 matProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ);
    _float arrProj[] = { matProj._11,matProj._12,matProj._13,matProj._14,
                         matProj._21,matProj._22,matProj._23,matProj._24,
                         matProj._31,matProj._32,matProj._33,matProj._34,
                         matProj._41,matProj._42,matProj._43,matProj._44 };
    memcpy(m_arrProj, arrProj, sizeof(arrProj));
}

void CImgui_Manager::DrawGrid()
{
    ImGuizmo::DrawGrid(m_arrView, m_arrProj, identityMatrix, 100.f);
}

_bool CImgui_Manager::Find_ObjectTag(LAYERTYPE eLayerType, wstring strTag, shared_ptr<CGameObject> pGameObject)
{
    for (auto iter : m_GameObjectList[eLayerType])
    {
        if (iter->Get_ObjectTag() == strTag)
        {
            if (nullptr != pGameObject.get())
                pGameObject = iter;
            return true;
        }
    }
    return false;
}

void CImgui_Manager::Dialog_Active()
{
    // display
    if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            if (m_eCurrentTool == TOOL_MAP)
            {
                //CData_Manager::GetInstance()->Save_ObjectData(filePathName, m_GameObjectList);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // display
    if (ImGuiFileDialog::Instance()->Display("LoadFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            /*  string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
              CData_Manager::GetInstance()->Load_ObjectData(filePathName, LEVEL_TOOL);
              for (_uint i = 0; i < (_uint)LAYERTYPE_END; i++)
                  Update_List(LAYERTYPE(i));

              static_cast<CImgui_Window_MapEdit*>(m_pWindows[TOOL_MAP].front())->m_pObjectTab->Update_TabList();*/
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void CImgui_Manager::Update_List(LAYERTYPE eLayerType)
{
    list<shared_ptr<CGameObject>>* pGameObjectList = m_pGameInstance->Get_ObjectList(LEVEL_TOOL, eLayerType);
    m_GameObjectList[eLayerType].clear();
    if (pGameObjectList != nullptr)
    {
        for (auto iter : *pGameObjectList)
        {
            m_GameObjectList[eLayerType].push_back(iter);
        }
    }
}

void CImgui_Manager::Arrow_Button(const string& strTag, _float fInterval, float* fValue)
{
    ImGui::SetNextItemWidth(100.0f);

    ImGui::DragFloat(strTag.c_str(), fValue, fInterval);
    ImGui::SameLine();
    string str = "##Up" + strTag;
    if (ImGui::ArrowButton(str.c_str(), ImGuiDir_Up))
        *fValue += fInterval;
    ImGui::SameLine();
    str = "##Down" + strTag;
    if (ImGui::ArrowButton(str.c_str(), ImGuiDir_Down))
        *fValue -= fInterval;
}

void CImgui_Manager::Arrow_Button(const string& strTag, _int iInterval, _int* iValue)
{
    ImGui::DragInt(strTag.c_str(), iValue);
    ImGui::SameLine();
    string str = "##Up" + strTag;
    if (ImGui::ArrowButton(str.c_str(), ImGuiDir_Up))
        *iValue += iInterval;
    ImGui::SameLine();
    str = "##Down" + strTag;
    if (ImGui::ArrowButton(str.c_str(), ImGuiDir_Down))
        *iValue -= iInterval;
}

_bool CImgui_Manager::InputFloat(const string& strTag, _float* vValue)
{
    _float fArr[1] = { *vValue };
    _bool bOut = ImGui::DragFloat(strTag.c_str(), fArr, 0.01f);
    *vValue = fArr[0];
    return bOut;
}

_bool CImgui_Manager::InputFloat2(const string& strTag, _float2* vValue)
{
    _float fArr[2] = { vValue->x,vValue->y };
    _bool bOut = ImGui::DragFloat2(strTag.c_str(), fArr, 0.01f);
    vValue->x = fArr[0];
    vValue->y = fArr[1];
    return bOut;
}

_bool CImgui_Manager::InputFloat3(const string& strTag, _float3* vValue)
{
    _float fArr[3] = { vValue->x,vValue->y,vValue->z };
    _bool bOut = ImGui::DragFloat3(strTag.c_str(), fArr, 0.01f);
    vValue->x = fArr[0];
    vValue->y = fArr[1];
    vValue->z = fArr[2];
    return bOut;
}

_bool CImgui_Manager::InputFloat4(const string& strTag, _float4* vValue)
{
    _float fArr[4] = { vValue->x,vValue->y,vValue->z ,vValue->w };
    _bool bOut = ImGui::DragFloat4(strTag.c_str(), fArr, 0.01f);
    vValue->x = fArr[0];
    vValue->y = fArr[1];
    vValue->z = fArr[2];
    vValue->w = fArr[3];
    return bOut;
}

void CImgui_Manager::Load_Image(const wstring& strTextureKey, _float2 vSize)
{
    ImTextureID texID = m_pGameInstance->Find_Front_SRV(strTextureKey);

    ImGui::Image(texID, ImVec2(vSize.x, vSize.y));
}

_bool CImgui_Manager::Check_ImGui_Rect()
{
    POINT ptMouse;

    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    ImVec2 windowPos = ImGui::GetWindowPos(); //왼쪽상단모서리점
    ImVec2 windowSize = ImGui::GetWindowSize();

    if (ptMouse.x >= windowPos.x && ptMouse.x <= windowPos.x + windowSize.x &&
        ptMouse.y >= windowPos.y && ptMouse.y <= windowPos.y + windowSize.y)
    {
        return true; //마우스 포인터가 ImGui 영역 내에 있음.
    }
    return false; //ImGui 영역이랑 안 겹침!
}

_bool CImgui_Manager::Check_Window_Rect()
{
    POINT ptMouse;

    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    if (ptMouse.x >= 0 && ptMouse.x <= g_iWinSizeX &&
        ptMouse.y >= 0 && ptMouse.y <= g_iWinSizeX)
    {
        return true; //마우스 포인터가 Window 영역 내에 있음.
    }
    return false; //Window 영역 바깥임
}

void CImgui_Manager::Load_EaseImage(_uint iEaseIndex, _float2(vSize))
{
    if (iEaseIndex >= 34)
        return;
    wstring wstrEasingTextureTag = StrToWstr(CEasing::pEases[iEaseIndex]);

    ImTextureID texID = m_pGameInstance->Find_Front_SRV(wstrEasingTextureTag);

    ImGui::Image(texID, ImVec2(vSize.x, vSize.y));
}

void CImgui_Manager::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    __super::Free();
    for (_int i = 0; i < (_int)TOOL_END; i++)
    {
        for (auto iter : m_pWindows[i])
            Safe_Release(iter);
    }

    Safe_Delete_Array(m_arrProj);
    Safe_Delete_Array(m_arrView);



    Safe_Release(m_pGameInstance);
}

#endif // DEBUG

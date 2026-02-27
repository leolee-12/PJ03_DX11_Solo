#include "stdafx.h"

#include <algorithm> // std::remove
#include <sstream>
#include <algorithm>
#include <cmath>
#include <locale>
#include <codecvt>
#include <string>
#include <iostream>
#include "IMGUI_Shader_Tab.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "RenderInstance.h"

#include <cstdio>

CIMGUI_Shader_Tab::CIMGUI_Shader_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, CEffect* pEffect)
	:CIMGUI_Tab{ pDevice,pContext },
    m_TestEffectModel_Texture{ pTexture },
    m_pEffect{ pEffect }
{
    
}

CIMGUI_Shader_Tab::CIMGUI_Shader_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture)
    :CIMGUI_Tab{ pDevice,pContext },
    m_TestEffectModel_Texture{ pTexture }
{

}


HRESULT CIMGUI_Shader_Tab::Initialize()
{
    DragAcceptFiles(g_hWnd, TRUE);
    isStart = true;
    m_PrototypeKeys.clear();
	return S_OK;
}

HRESULT CIMGUI_Shader_Tab::Load_Initialize(string strFilename)
{
    DragAcceptFiles(g_hWnd, TRUE);
    isStart = true;
    m_PrototypeKeys.clear();

    return S_OK;
}
//
//HRESULT CIMGUI_Shader_Tab::Copy_Initialize(void* pArg)
//{
//    return S_OK;
//}

void CIMGUI_Shader_Tab::Update(_float fTimeDelta)
{

    for (auto& iter : m_NodeTextures)
    {
        iter->Camera_Update(fTimeDelta);
        iter->Update(fTimeDelta);
        iter->Late_Update(fTimeDelta);
    }

}

void CIMGUI_Shader_Tab::Render(_float fTimeDelta)
{
    for (auto& iter : m_NodeTextures)
    {
        iter->Camera_Update(fTimeDelta);
        iter->Update(fTimeDelta);
    }
       
    //if (ImGui::Button("Add Rect") && !isStart)
    //{
    //    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Effect_Rect"), TEXT("Layer_Effect_Rect"))))
    //        return;

    //    isStart = true;
    //    DragAcceptFiles(g_hWnd, TRUE);
    //}

    ImGui::Separator();

    if (ImGui::Button("Add MoveTex") && isStart)
    {
        MoveTex_Node nodeDesc{};
        nodeDesc.MoveTex_node_id = m_MoveTex_node_id++;
        m_MoveTex_Node_ids.push_back(nodeDesc);
      //  node_ids.push_back(nodeDesc.MoveTex_node_id - 1500);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Sprite") && isStart)
    {
        Sprite_Node nodeDesc{};
        nodeDesc.Sprite_node_id = m_Sprite_node_id++;
        m_Sprite_Node_ids.push_back(nodeDesc);
      //  node_ids.push_back(nodeDesc.Sprite_node_id - 3000);
    }

    ImGui::Text("Glow_Pri : Input == -2       Glow : Input > -1       NotGlow : Input == -1");
    ImGui::SetNextItemWidth(100.0f);
    _int GameObjectData = m_pEffect->Get_GameObjectData();
    _int GlowFactor = m_pEffect->Get_ObjectRenderData(); // - (GameObjectData == -2 ? 5 : 0);
    if (ImGui::InputInt("Glow", &GameObjectData) && isStart)
    {
        m_pEffect->Set_GameObjectData(GameObjectData);
        if (GlowFactor < 1 || GlowFactor > 4)
            GlowFactor = 1;

        

    }
    if (GlowFactor > 4)
        GlowFactor -= 5;
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::InputInt("GlowFactor", &GlowFactor) && isStart)
    {
        if (GlowFactor > -1 && GlowFactor < 5)
        {
            if (GameObjectData == -2)
                GlowFactor += 5;
            m_pEffect->Set_ObjectRenderData(GlowFactor);
        }
    }

    ImNodes::BeginNodeEditor();  /* 노드 생성시 무조건 호출해야함 */

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    

    Render_MainNode();

    Render_TextureNode();

    Render_MoveTexNode();

    Render_SpriteNode();

    Render_Link();

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);
    ImGui::PopStyleColor();
    ImNodes::EndNodeEditor(); /* 노드 끝 */

    Check_Create_Link(); // 생성할때는 노드에디터가 끝날때 체크
    Check_Delete_Link(); // 삭제하려할땐 전에

    //_int link_id;
    //if (ImNodes::IsLinkDestroyed(&link_id))
    //{
    //    links.erase(std::remove_if(links.begin(), links.end(),
    //        [link_id](const pair<int, int>& link) {
    //            return link.first == link_id || link.second == link_id;
    //        }), links.end());
    //}
    for (auto& iter : m_NodeTextures)
    {
        iter->Late_Update(fTimeDelta);
    }
}

void CIMGUI_Shader_Tab::TabPos_Init()
{
    for (const auto& node : node_positions)
    {
        int node_id = node.first;      // 노드 ID
        ImVec2 node_pos = node.second; // 노드 위치

        // 각 노드의 위치 설정
        ImNodes::SetNodeEditorSpacePos(node_id, node_pos);
    }
}

void CIMGUI_Shader_Tab::Create_NodeTexture(string szPath)
{
    size_t testModelsPos = szPath.find("ModelData");

    if (testModelsPos != string::npos)
    {
        string relativePath = szPath.substr(testModelsPos);

        replace(relativePath.begin(), relativePath.end(), '\\', '/');

        string fullPath = string("../Bin/") + relativePath;
        wstring_convert<codecvt_utf8_utf16<_tchar>> converter;
        // 마지막 슬래시 이후 문자열 찾기
        size_t lastSlashPos = relativePath.find_last_of('/');
        size_t lastDotPos = relativePath.find_last_of('.');

        std::string fileName = relativePath.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1); // "cmm_fire"
        
        wstring prototypeKey = TEXT("Prototype_Component_Texture_Effect_") + converter.from_bytes(fileName);
        //wstring prototypeKeyWithCount = prototypeKey + to_wstring(m_iNodeTextureCount) + TEXT("_") + to_wstring(m_iNumberId);
        wstring prototypeKeyWithAlpha = prototypeKey + TEXT("_Alpha");
        CShader_Texture::SHADER_TEXTURE_DESC tDesc{};
        tDesc.prototypeKey = prototypeKey.c_str();
        tDesc.iID = unique_node_id;
        tDesc.iShaderTab_ID = m_iNumberId;
        //Prototype_Component_Texture_Effect_cmn_aura00
        //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, prototypeKeyWithCount.c_str(),
        //    CTexture::Create(m_pDevice, m_pContext, converter.from_bytes(fullPath).c_str(), 1))))
        //    return;

        CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Shader_Texture"));
        static_cast<CShader_Texture*>(pPrototype->Clone((void*)&tDesc));
        _float2 fTextureSize = m_NodeTextures.back()->m_pTextureCom->Get_TextureSize();

        _float fAspectRatio = fTextureSize.x / fTextureSize.y;
        _float fWinAspectRatio = (_float)g_iWinSizeX / (_float)g_iWinSizeY;

        if (fTextureSize.x > g_iWinSizeX || fTextureSize.y > g_iWinSizeY)
        {
            if (fTextureSize.x / g_iWinSizeX > fTextureSize.y / g_iWinSizeY)
            {
                fTextureSize.x = g_iWinSizeX;
                fTextureSize.y = g_iWinSizeX / fAspectRatio;
            }
            else
            {
                fTextureSize.y = g_iWinSizeY;
                fTextureSize.x = g_iWinSizeY * fAspectRatio;
            }
        }

        _int iRenderTargetCount = m_pRenderInstance->Add_ClientRenderTarget(prototypeKey.c_str(), prototypeKey.c_str(), fTextureSize.x, fTextureSize.y, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f));
        /* 이미 렌더타겟이 중복이다 */
        if (iRenderTargetCount > 0)
        {
            SRV_Texture SRVDesc{};
            SRVDesc.iID = unique_node_id;
            SRVDesc.Texture = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(prototypeKey.c_str() + to_wstring(iRenderTargetCount));
            SRVDesc.Alpha = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(prototypeKey.c_str() + to_wstring(iRenderTargetCount) + L"_Alpha");

            if (SRVDesc.Texture == nullptr || SRVDesc.Alpha == nullptr)
            {
                int a = 10;
            }
            m_NodeTextureSRVs.push_back(SRVDesc);

            m_NodeTextures.back()->Set_PlusKey(prototypeKey.c_str() + to_wstring(iRenderTargetCount));

            Save_Key tSave_KeyDesc{};
            tSave_KeyDesc.iD = unique_node_id;
            tSave_KeyDesc.key = prototypeKey.c_str();
            m_PrototypeKeys.push_back(tSave_KeyDesc);

            m_NodeTextures.back()->m_iID = unique_node_id;
            node_ids.push_back(unique_node_id++);

            DragAcceptFiles(g_hWnd, TRUE);
            m_iNodeTextureCount++;

            return;
        }

        SRV_Texture SRVDesc{};
        SRVDesc.iID = unique_node_id;
        SRVDesc.Texture = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(prototypeKey.c_str());
        SRVDesc.Alpha = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(prototypeKeyWithAlpha.c_str());
        if (SRVDesc.Texture == nullptr || SRVDesc.Alpha == nullptr)
        {
            int a = 10;
        }
        m_NodeTextureSRVs.push_back(SRVDesc);

        Save_Key tSave_KeyDesc{};
        tSave_KeyDesc.iD = unique_node_id;
        tSave_KeyDesc.key = prototypeKey.c_str();
        m_PrototypeKeys.push_back(tSave_KeyDesc);

        m_NodeTextures.back()->m_iID = unique_node_id;
        node_ids.push_back(unique_node_id++);

        DragAcceptFiles(g_hWnd, TRUE);
        m_iNodeTextureCount++;
    }
}

void CIMGUI_Shader_Tab::Render_MainNode()
{
    if (isStart == true)
    {
        //ImNodes::SetNodeEditorSpacePos(m_iMain_node_id, node_positions[m_iMain_node_id]);

        ImNodes::BeginNode(m_iMain_node_id);
        ImGui::Text("Main_Node");

        // Input attribute
        ImNodes::BeginInputAttribute( m_iMain_node_id * 2);
        ImGui::Text("Diffuse");
        ImNodes::EndInputAttribute();

        ImNodes::BeginInputAttribute(  m_iMain_node_id * 2 - 1);
        ImGui::Text("Alpha");
        ImNodes::EndInputAttribute();

        ImNodes::EndNode();


        ImVec2 current_pos = ImNodes::GetNodeEditorSpacePos(m_iMain_node_id);

        if (node_positions[m_iMain_node_id].x != current_pos.x ||
            node_positions[m_iMain_node_id].y != current_pos.y)
        {
            node_positions[m_iMain_node_id] = current_pos;
        }

        //
    }
}

void CIMGUI_Shader_Tab::Render_TextureNode()
{
    for (size_t i = 0; i < node_ids.size(); ++i)
    {
        int node_id = node_ids[i];

        ImNodes::BeginNode(  node_id);
        ImGui::Text("Node %d", node_id);

        ImNodes::BeginOutputAttribute(node_id * m_iAttributeCount);
        ImGui::Text("OutColor");
        ImNodes::EndOutputAttribute();

        ImNodes::BeginInputAttribute(node_id * m_iAttributeCount + 1);
        ImGui::Text("Diffuse");
        ImNodes::EndInputAttribute();

        ImNodes::BeginInputAttribute(node_id * m_iAttributeCount + 2);
        ImGui::Text("Alpha");
        ImNodes::EndInputAttribute();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.8f, 0.9f, 1.0f));
        ShowTextWithoutPrefix(m_PrototypeKeys[i].key);
        ImGui::PopStyleColor();

        if (i < m_NodeTextureSRVs.size())
        {
            ImGui::Image(m_NodeTextureSRVs[i].Texture, ImVec2(150, 150));
        }

        ImNodes::BeginInputAttribute(  node_id * m_iAttributeCount + 3);
        ImGui::Text("ShadeFunction");
        ImNodes::EndInputAttribute();

        //m_NodeTextures[i]->m_vMultiple_Texcoord
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("TexCoord");
        ImGui::SetNextItemWidth(140);
        _float TexCoordValue[2] = { m_NodeTextures[i]->m_vMultiple_Texcoord.x,m_NodeTextures[i]->m_vMultiple_Texcoord.y };
        ImGui::InputFloat2("##TexCoordInput", TexCoordValue, "%.1f");

        ImGui::PopStyleColor();
        if (TexCoordValue[0] == 0.0f) TexCoordValue[0] = 0.01f;
        if (TexCoordValue[1] == 0.0f) TexCoordValue[1] = 0.01f;
        m_NodeTextures[i]->m_vMultiple_Texcoord.x = TexCoordValue[0];
        m_NodeTextures[i]->m_vMultiple_Texcoord.y = TexCoordValue[1];

        ImNodes::EndNode();

        ImVec2 current_pos = ImNodes::GetNodeEditorSpacePos(node_id);

        if (node_positions[node_id].x != current_pos.x ||
            node_positions[node_id].y != current_pos.y)
        {
            node_positions[node_id] = current_pos;
        }
    }
}

void CIMGUI_Shader_Tab::Render_MoveTexNode()
{
    for (auto& iter : m_MoveTex_Node_ids)
    {
        int node_id = iter.MoveTex_node_id;
        
        ImNodes::BeginNode(node_id);

        ImGui::Dummy(ImVec2(1, 1));

        ImGui::Text("MoveTex_Node %d", node_id - 1500);

        ImGui::SetNextItemWidth(60);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::InputFloat2("Direction", &iter.fDirection.x, "%.1f");

        ImGui::SetNextItemWidth(50);
        ImGui::DragFloat("Speed", &iter.fSpeed, 0.1f, 0.0f, 30.0f, "%.2f");

        ImGui::PopStyleColor();
        ImNodes::BeginOutputAttribute(node_id + 1, 2);
        ImGui::Text("Out");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();

        ImVec2 current_pos = ImNodes::GetNodeEditorSpacePos(node_id);

        if (node_positions[node_id].x != current_pos.x ||
            node_positions[node_id].y != current_pos.y)
        {
            node_positions[node_id] = current_pos;
        }
    }
}

void CIMGUI_Shader_Tab::Render_SpriteNode()
{
    for (auto& iter : m_Sprite_Node_ids)
    {
        int node_id = iter.Sprite_node_id;

        ImNodes::BeginNode(node_id);

        ImGui::Dummy(ImVec2(1, 1));

        ImGui::Text("Sprite_Node %d", node_id - 3000);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::SetNextItemWidth(50);
        _int a[2] = { iter.fSpriteSizeNumber.x ,iter.fSpriteSizeNumber.y };
        ImGui::InputInt2("Width & Height", a, ImGuiInputTextFlags_DisplayEmptyRefVal);
        iter.fSpriteSizeNumber = { (_float)a[0],(_float)a[1] };
        ImGui::SetNextItemWidth(50);

        ImGui::DragFloat("Speed", &iter.fSpeed, 0.1f, 0.0f, 30.0f, "%.2f");

        ImGui::PopStyleColor();
        ImNodes::BeginOutputAttribute(node_id + 1, 2);
        ImGui::Text("Out");
        ImNodes::EndOutputAttribute();

        auto it = find_if(links.begin(), links.end(),
            [node_id](const pair<_int, _int>& link) {
                return link.first == node_id + 1;
            });

        if (it != links.end())
        {
            auto Texture_nodeit = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                [&](CShader_Texture* texture) {
                    return texture->m_iID == (*it).second / m_iAttributeCount;
                });

            if (Texture_nodeit != m_NodeTextures.end())
            {

                if (ImGui::Checkbox("Loop", &(iter.isLoop)))
                    (*Texture_nodeit)->Sprite_Loop(iter.isLoop);

                Draw_MusicButton((*Texture_nodeit));
            }
        }



        //else
        //{
        //    Draw_MusicButton(nullptr);
        //}

        ImNodes::EndNode();

        ImVec2 current_pos = ImNodes::GetNodeEditorSpacePos(node_id);

        if (node_positions[node_id].x != current_pos.x ||
            node_positions[node_id].y != current_pos.y)
        {
            node_positions[node_id] = current_pos;
        }
    }
}

void CIMGUI_Shader_Tab::Render_Link()
{
    // 노드들끼리 연결된 모든 애들을 시각화 하기 위한 함수
    for (size_t i = 0; i < links.size(); i++)
    {
        ImNodes::Link(i, links[i].first, links[i].second);
    }
}

void CIMGUI_Shader_Tab::Check_Delete_Link()
{
    static NODE_TYPE eNodeType = NODE_END;
    static int iID = -999;

    if (m_pGameInstance->Get_DIMouseState(DIMK_LBUTTON))
    {
        /*vector<pair<_int, _int>> links*/
        for (_int i = 0; i < links.size(); i++)
        {
            if (ImNodes::IsLinkHovered(&i))
            {
                iID = i;
                eNodeType = NODE_LINK;
                return;
            }
        }
        /*vector<_int> node_ids*/
        for (auto iter : node_ids)
        {
            if (iter == -1)
                return;

            if (ImNodes::IsNodeHovered(&iter))
            {
                if (iter > 1500)
                {
                    iID = iter;
                    eNodeType = NODE_FUNCTION;
                    return;
                }

                iID = iter;
                eNodeType = NODE_BASIC;
                return;
            }
        }
        /*list<MoveTex_Node> m_MoveTex_Node_ids*/
        for (auto iter : m_MoveTex_Node_ids)
        {
            if (ImNodes::IsNodeHovered(&iter.MoveTex_node_id))
            {
                if (iter.MoveTex_node_id == -1)
                    return;

                iID = iter.MoveTex_node_id;
                eNodeType = NODE_FUNCTION;
                return;
            }
        }

        for (auto iter : m_Sprite_Node_ids)
        {
            if (ImNodes::IsNodeHovered(&iter.Sprite_node_id))
            {
                if (iter.Sprite_node_id == -1)
                    return;

                iID = iter.Sprite_node_id;
                eNodeType = NODE_FUNCTION;
                return;
            }
        }
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_DELETE) && eNodeType != NODE_END)
    {
        //id 일단 저장
        _int node_id = iID;

        switch (eNodeType)
        {
        case NODE_BASIC:
        {
            
            m_PrototypeKeys.erase(
                remove_if(m_PrototypeKeys.begin(), m_PrototypeKeys.end(),
                    [](const Save_Key& key) {
                        return key.iD == iID; 
                    }),
                m_PrototypeKeys.end());

            auto Posit = node_positions.find(iID);
            if (Posit != node_positions.end()) {
                node_positions.erase(Posit);
            }

            //Node 부터 지움
            node_ids.erase(remove(node_ids.begin(), node_ids.end(), iID), node_ids.end());

            // m_NodeTextures에 있는거 찾아서 삭제
            auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                [&](CShader_Texture* texture) {
                    return texture->m_iID == iID;
                });
            if (it != m_NodeTextures.end())
            {
                (*it)->Free();
                m_NodeTextures.erase(it);
            }

            // m_NodeTextureSRVs는 SRV는 내비두고 삭제
            auto iter = std::find_if(m_NodeTextureSRVs.begin(), m_NodeTextureSRVs.end(),
                [&](const SRV_Texture& srv) {
                    return srv.iID == iID;
                });
            if (iter != m_NodeTextureSRVs.end())
            {
                m_NodeTextureSRVs.erase(iter);
            }

            // 이 노드가 다른 노드에게 영향을 줬을수도 있음 그 타겟노드아이디
            _int target_node_id = node_id * m_iAttributeCount;
            _int target_second_value = -1;
            // 찾아서 영향받은 노드 ID 저장
            auto LinkIter = std::find_if(links.begin(), links.end(),
                [target_node_id](const pair<_int, _int>& link) {
                    return link.first == target_node_id;
                });
            if (LinkIter != links.end()) {
                target_second_value = LinkIter->second;
            }

            if (target_second_value == -2)
            {
                if (m_pDefaultTexture == nullptr)
                {
                    m_pDefaultTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_Default_Diffuse")));
                }

                m_TestEffectModel_Texture->Set_SRV(m_pDefaultTexture->Get_SRV(0));

            }
            // 그 노드 ID가 알파에 영향을 줬었냐?
            else if (target_second_value % 4 == 2)
            {
                // m_NodeTextures에서 찾아서 삭제
                auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == target_second_value / 4;
                    });
                if (it != m_NodeTextures.end())
                {
                    (*it)->Remove_InputTextures(END_ALPHA);
                }
            }
            // 그 노드 ID가 디퓨즈에 영향을 줬었냐?
            else if (target_second_value % 4 == 1)
            {
                // m_NodeTextures에서 찾아서 삭제
                auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == target_second_value / 4;
                    });
                if (it != m_NodeTextures.end())
                {
                    (*it)->Remove_InputTextures(END_DIFFUSE);
                }
            }

            // 링크 삭제
            links.erase(remove_if(links.begin(), links.end(),
                [node_id](const pair<_int, _int>& link) {
                    return link.first == node_id * 4 || link.second == node_id * 4 ||
                        link.first == node_id * 4 + 1 || link.second == node_id * 4 + 1 ||
                        link.first == node_id * 4 + 2 || link.second == node_id * 4 + 2 ||
                        link.first == node_id * 4 + 3 || link.second == node_id * 4 + 3;
                }), links.end());

            iID = -999;
            eNodeType = NODE_END;
            break;
        }
        case NODE_LINK:
        {
            auto linkPos = links.begin() + iID;

            // 혹시 함수?
            if (linkPos->first > 1500)
            {
                // m_NodeTextures에서 찾아서 삭제
                auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == linkPos->second / 4;
                    });
                if (it != m_NodeTextures.end())
                {
                    if (linkPos->first > 1500 && linkPos->first < 3000)
                        (*it)->Remove_InputFunction(FUNCTION_TEXMOVE);
                    else if (linkPos->first > 3000)
                        (*it)->Remove_InputFunction(FUNCTION_SPRITE);
                }
            }
            // 혹시 메인노드?
            else if (linkPos->second == -2)
            {
                if (m_pDefaultTexture == nullptr)
                {
                    m_pDefaultTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_Default_Diffuse")));
                }

                m_TestEffectModel_Texture->Set_SRV(m_pDefaultTexture->Get_SRV(0));

            }
            // 그 노드 ID가 알파에 영향을 줬었냐?
            else if (linkPos->second % 4 == 2)
            {
                // m_NodeTextures에서 찾아서 삭제
                auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == linkPos->second / 4;
                    });
                if (it != m_NodeTextures.end())
                {
                    (*it)->Remove_InputTextures(END_ALPHA);
                }
            }
            // 그 노드 ID가 디퓨즈에 영향을 줬었냐?
            else if (linkPos->second % 4 == 1)
            {
                // m_NodeTextures에서 찾아서 삭제
                auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == linkPos->second / 4;
                    });
                if (it != m_NodeTextures.end())
                {
                    (*it)->Remove_InputTextures(END_DIFFUSE);
                }
            }

            links.erase(linkPos);

            iID = -999;
            eNodeType = NODE_END;
            break;
        }
        case NODE_FUNCTION:
        {
            // 이 노드가 다른 노드에게 영향을 줬을수도 있음 그 타겟노드아이디
            _int target_node_id = node_id + 1;
            _int target_second_value = -1;
            // 찾아서 영향받은 노드 ID 저장
            auto LinkIter = std::find_if(links.begin(), links.end(),
                [target_node_id](const pair<_int, _int>& link) {
                    return link.first == target_node_id;
                });
            if (LinkIter != links.end()) {
                target_second_value = LinkIter->second;
            }

            // 혹시 메인노드?
            if (target_second_value == -2)
            {

            }
            // 함수 아웃쪽
            else if (target_second_value % 4 == 3)
            {
                // m_NodeTextures에서 찾아서 삭제
                auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == target_second_value / 4;
                    });
                if (it != m_NodeTextures.end())
                {
                    if (target_node_id > 1500 && target_node_id < 3000)
                    {
                        (*it)->Remove_InputFunction(FUNCTION_TEXMOVE);

                    }
                    else if (target_node_id > 3000)
                    {
                        (*it)->Remove_InputFunction(FUNCTION_SPRITE);

                    }
                }

            }
            if (target_node_id > 1500 && target_node_id < 3000)
            {
                m_MoveTex_Node_ids.remove_if([&](const MoveTex_Node& node) {
                    return node.MoveTex_node_id == iID;
                    });
            }
            else if (target_node_id > 3000)
            {
                m_Sprite_Node_ids.remove_if([&](const Sprite_Node& node) {
                    return node.Sprite_node_id == iID;
                    });
            }
            links.erase(remove_if(links.begin(), links.end(),
                [node_id](const pair<_int, _int>& link) {
                    return link.first == node_id + 1; //||
                }), links.end());
            //Node 부터 지움

          /*  if(iID < 3000)
                node_ids.erase(remove(node_ids.begin(), node_ids.end(), iID - 1500), node_ids.end());
            else
                node_ids.erase(remove(node_ids.begin(), node_ids.end(), iID - 3000), node_ids.end());*/

                //// 그 노드 ID가 디퓨즈에 영향을 줬었냐?
                //else if (target_second_value % 4 == 1)
                //{
                //    // m_NodeTextures에서 찾아서 삭제
                //    auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                //        [&](CShader_Texture* texture) {
                //            return texture->m_iID == target_second_value / 4;
                //        });
                //    if (it != m_NodeTextures.end())
                //    {
                //        (*it)->Remove_InputFunction(FUNCTION_TEXMOVE);
                //    }
                //}



            iID = -999;
            eNodeType = NODE_END;
            break;
        }
        }
    }
}

void CIMGUI_Shader_Tab::Draw_MusicButton(CShader_Texture* pShaderTexture)
{
    //재생버튼   
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 35);
    ImVec2 button_size = ImVec2(20, 20);
    if (ImGui::InvisibleButton("play_button", button_size)) 
    {
        pShaderTexture->Sprite_ButtonClick(CShader_Texture::SB_PLAY);
    }

    ImVec2 p = ImGui::GetItemRectMin();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float size = 20.0f;
    ImVec2 triangle_pos[3] = {
        ImVec2(p.x + (button_size.x - size) * 0.5f, p.y),
        ImVec2(p.x + (button_size.x + size) * 0.5f, p.y + size / 2),
        ImVec2(p.x + (button_size.x - size) * 0.5f, p.y + size)
    };

    ImU32 triangle_color;

    if (ImGui::IsItemHovered()) 
        triangle_color = IM_COL32(155, 255, 155, 255);
    else
        triangle_color = IM_COL32(85, 255, 85, 255);

    draw_list->AddTriangleFilled(triangle_pos[0], triangle_pos[1], triangle_pos[2], triangle_color);

    // 일시정지 버튼
    ImGui::SameLine();
    if (ImGui::InvisibleButton("pause_button", button_size)) 
    {
        pShaderTexture->Sprite_ButtonClick(CShader_Texture::SB_PAUSE);
    }

    p = ImGui::GetItemRectMin();
    ImVec2 pause_rect1_pos[2] = {
        ImVec2(p.x + 4, p.y),
        ImVec2(p.x + 8, p.y + size)
    };
    ImVec2 pause_rect2_pos[2] = {
        ImVec2(p.x + 12, p.y),
        ImVec2(p.x + 16, p.y + size)
    };
    ImU32 pause_color = IM_COL32(255, 255, 65, 255);
    if (ImGui::IsItemHovered()) 
        pause_color = IM_COL32(255, 255, 185, 255);

    draw_list->AddRectFilled(pause_rect1_pos[0], pause_rect1_pos[1], pause_color);
    draw_list->AddRectFilled(pause_rect2_pos[0], pause_rect2_pos[1], pause_color);

    // 초기화 버튼
    ImGui::SameLine();
    if (ImGui::InvisibleButton("reset_button", button_size)) 
    {
        pShaderTexture->Sprite_ButtonClick(CShader_Texture::SB_RESET);
    }

    p = ImGui::GetItemRectMin();
    ImVec2 reset_rect_pos[2] = {
        ImVec2(p.x, p.y),
        ImVec2(p.x + size, p.y + size)
    };
    ImU32 reset_color = IM_COL32(255, 85, 85, 255);
    if (ImGui::IsItemHovered())
        reset_color = IM_COL32(255, 135, 135, 255);

    draw_list->AddRectFilled(reset_rect_pos[0], reset_rect_pos[1], reset_color);
}

void CIMGUI_Shader_Tab::Click_Save_Shader_Tab(string fileName)
{
    Shader_Tab_Save tDesc{};
    /*------------ 값 채우기------------ */

    _int iCount = 0;
    // 1. Save_Key 채우기 (m_PrototypeKeys에서 값을 가져옴)
    for (const auto& prototypeKey : m_PrototypeKeys)
    {
        Save_Key saveKey;
        saveKey.key = prototypeKey.key;
        saveKey.iD = prototypeKey.iD;
        saveKey.vTexCoord = m_NodeTextures[iCount]->m_vMultiple_Texcoord;
        tDesc.keys.push_back(saveKey);
    }

    // 2. Node_Position 채우기 (node_positions에서 값을 가져옴)
    for (const auto& nodePosPair : node_positions)
    {
        Node_Position nodePos;
        nodePos.nodeID = nodePosPair.first;
        nodePos.nodePosition = nodePosPair.second;
        tDesc.nodePositions.push_back(nodePos);
    }

    // 3. MoveTex_Node_Save 채우기 (m_MoveTex_Node_ids에서 값을 가져옴)
    for (const auto& moveTexNode : m_MoveTex_Node_ids)
    {
        MoveTex_Node_Save moveTexNodeSave;
        moveTexNodeSave.MoveTex_node_id = moveTexNode.MoveTex_node_id;
        moveTexNodeSave.fDirection = moveTexNode.fDirection;
        moveTexNodeSave.fSpeed = moveTexNode.fSpeed;
        tDesc.moveTexNodes.push_back(moveTexNodeSave);
    }

    // 4. Sprite_Node_Save 채우기 (m_Sprite_Node_ids에서 값을 가져옴)
    for (const auto& spriteNode : m_Sprite_Node_ids)
    {
        Sprite_Node_Save spriteNodeSave;
        spriteNodeSave.Sprite_node_id = spriteNode.Sprite_node_id;
        spriteNodeSave.isLoop = spriteNode.isLoop;
        spriteNodeSave.fSpriteSizeNumber = spriteNode.fSpriteSizeNumber;
        spriteNodeSave.fSpeed = spriteNode.fSpeed;
        tDesc.spriteNodes.push_back(spriteNodeSave);
    }

    // 5. Link_Save 채우기 (links에서 값을 가져옴)
    for (const auto& link : links)
    {
        Link_Save linkSave;
        linkSave.srcNodeID = link.first;
        linkSave.destNodeID = link.second;
        tDesc.links.push_back(linkSave);
    }


    /*------------------------------------*/
    Save_Shader_Tab(fileName, tDesc);
}

void CIMGUI_Shader_Tab::Click_Load_Shader_Tab(string fileName)
{
    // 불러온 데이터를 저장할 구조체
    Shader_Tab_Save tDesc{};

    // 파일에서 데이터를 불러옴
    Load_Shader_Tab(fileName, tDesc);

    // 1. 텍스처 키 정보 로드 및 처리
    m_PrototypeKeys.clear();  // 기존 데이터를 초기화
    for (const auto& key : tDesc.keys)
    {
        m_PrototypeKeys.push_back(key);
    }
    Load_NodeTextures(m_PrototypeKeys);  // Prototype 키를 통해 노드 텍스처를 로드

    // 2. 각 노드의 위치 설정
    //node_positions.clear();  // 기존 위치 정보를 초기화
    for (const auto& nodePos : tDesc.nodePositions)
    {

        node_positions[nodePos.nodeID] = nodePos.nodePosition;

    }

    // 3. 무브 텍스처 노드 정보 로드 및 추가
    m_MoveTex_Node_ids.clear();  // 기존 무브 텍스 노드 초기화
    for (const auto& moveTexNode : tDesc.moveTexNodes)
    {
        MoveTex_Node newMoveTexNode;
        newMoveTexNode.MoveTex_node_id = moveTexNode.MoveTex_node_id;
        newMoveTexNode.fDirection = moveTexNode.fDirection;
        newMoveTexNode.fSpeed = moveTexNode.fSpeed;
        m_MoveTex_Node_ids.push_back(newMoveTexNode);  // 무브 텍스 노드 리스트에 추가
    }

    // 4. 스프라이트 노드 정보 로드 및 추가
    m_Sprite_Node_ids.clear();  // 기존 스프라이트 노드 초기화
    for (const auto& spriteNode : tDesc.spriteNodes)
    {
        Sprite_Node newSpriteNode;
        newSpriteNode.Sprite_node_id = spriteNode.Sprite_node_id;
        newSpriteNode.isLoop = spriteNode.isLoop;
        newSpriteNode.fSpriteSizeNumber = spriteNode.fSpriteSizeNumber;
        newSpriteNode.fSpeed = spriteNode.fSpeed;
        m_Sprite_Node_ids.push_back(newSpriteNode);  // 스프라이트 노드 리스트에 추가
    }

    // 5. 노드 간 연결 정보 로드
    links.clear();  // 기존 연결 정보 초기화
    for (const auto& link : tDesc.links)
    {
        Create_Link(link.srcNodeID, link.destNodeID);
        //links.push_back({ link.srcNodeID, link.destNodeID });  // 노드 연결 리스트에 추가
    }

    // 6. 추가적인 초기화가 필요한 내용이 있으면 추가
    // 예를 들어, 노드들의 ID 또는 속성 재설정 등이 필요할 수 있습니다.
}

void CIMGUI_Shader_Tab::Create_Link(_int start_attr, _int end_attr)
{

    auto it = find_if(links.begin(), links.end(),
        [end_attr](const pair<_int, _int>& link) {
            return link.second == end_attr;
        });

    if (it != links.end())
    {

        links.erase(it);
    }


    // 함수 노드들
    if (start_attr > 1500)
    {
        if (start_attr < 3000)
        {
            auto it = m_MoveTex_Node_ids.begin();
            advance(it, (start_attr - 1 - 1501));

            links.push_back(make_pair(start_attr, end_attr));
            auto Texture_nodeit = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                [&](CShader_Texture* texture) {
                    return texture->m_iID == end_attr / m_iAttributeCount;
                });

            if (Texture_nodeit != m_NodeTextures.end())
            {
                (*Texture_nodeit)->Push_Shade_MoveTex(&it->fDirection, &it->fSpeed);
                (*Texture_nodeit)->Remove_InputFunction(FUNCTION_SPRITE);
            }
        }
        else if (start_attr > 3000/* 노드 종류 추가되면 조건 더 달아야됨*/)
        {
            auto it = m_Sprite_Node_ids.begin();
            advance(it, (start_attr - 1 - 3001));

            links.push_back(make_pair(start_attr, end_attr));
            auto Texture_nodeit = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                [&](CShader_Texture* texture) {
                    return texture->m_iID == end_attr / m_iAttributeCount;
                });

            if (Texture_nodeit != m_NodeTextures.end())
            {
                (*Texture_nodeit)->Push_Shade_Sprite(&it->fSpriteSizeNumber, &it->fSpeed);
                (*Texture_nodeit)->Remove_InputFunction(FUNCTION_TEXMOVE);
            }

        }
    }
    // Main_Node Diffuse에 연결된거임
    else if (end_attr == -2)
    {
        m_iMain_Input_Diffuse_id = (start_attr) / 3;


        auto& SRVit = std::find_if(m_NodeTextureSRVs.begin(), m_NodeTextureSRVs.end(),
            [&](SRV_Texture SRV) {
                return SRV.iID == start_attr / m_iAttributeCount;
            });
        //wstring abs = L"a";
        //wstring absb = L"b";
        //m_TestEffectModel_Texture = static_cast<CTexture*>(static_cast<CEffect_Single*>(CEffect_Manager::Get_Instance()->Find_In_Layer_Effect(abs, absb))->Get_Component(TEXT("Com_DiffuseTexture")));
        if (SRVit != m_NodeTextureSRVs.end())
        {
            links.push_back(make_pair(start_attr, end_attr));
            m_TestEffectModel_Texture->Set_SRV((ID3D11ShaderResourceView*)SRVit->Texture);
            m_TestEffectModel_Texture->Set_SRV((ID3D11ShaderResourceView*)SRVit->Alpha, 1);
        }
        //m_Effect_Rect->Push_Texture_Diffuse((ID3D11ShaderResourceView*)m_NodeTextureSRVs[m_iMain_Input_Diffuse_id - 1], 0);
    }
    else if (end_attr % 4 == 2)
    {
        links.push_back(make_pair(start_attr, end_attr));

        auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
            [&](CShader_Texture* texture) {
                return texture->m_iID == end_attr / m_iAttributeCount;
            });

        if (it != m_NodeTextures.end())
        {
            auto SRVit = std::find_if(m_NodeTextureSRVs.begin(), m_NodeTextureSRVs.end(),
                [&](SRV_Texture SRV) {
                    return SRV.iID == start_attr / m_iAttributeCount;
                });

            (*it)->Push_InputTextures((ID3D11ShaderResourceView*)SRVit->Texture, END_ALPHA);

        }
        //m_NodeTextures[(end_attr - 2) / 4 - 1]->Push_InputTextures((ID3D11ShaderResourceView*)m_NodeTextureSRVs[(start_attr) / 4 - 1].Texture, END_ALPHA);
    }
    else if (end_attr % 4 == 1)
    {
        links.push_back(make_pair(start_attr, end_attr));

        auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
            [&](CShader_Texture* texture) {
                return texture->m_iID == end_attr / m_iAttributeCount;
            });
        if (it != m_NodeTextures.end())
        {
            auto SRVit = std::find_if(m_NodeTextureSRVs.begin(), m_NodeTextureSRVs.end(),
                [&](SRV_Texture SRV) {
                    return SRV.iID == start_attr / 4;
                });

            (*it)->Push_InputTextures((ID3D11ShaderResourceView*)SRVit->Texture, END_DIFFUSE);

        }

        // m_NodeTextures[(end_attr - 1) / 4 - 1]->Push_InputTextures((ID3D11ShaderResourceView*)m_NodeTextureSRVs[(start_attr) / 4 - 1].Texture, END_DIFFUSE);
    }
    
}


void CIMGUI_Shader_Tab::Load_NodeTextures(vector<Save_Key>& PrototypeKeys)
{
    _int iCount = 0;
    for (auto& iter : PrototypeKeys)
    {
        //size_t lastSlashPos = iter.key.find_last_of('/');
        //size_t lastDotPos = iter.key.find_last_of('.');

        //wstring fileName = iter.key.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);

        CShader_Texture::SHADER_TEXTURE_DESC tDesc{};
        tDesc.prototypeKey = iter.key.c_str();
        
        tDesc.iShaderTab_ID = m_iNumberId;
        
        CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Shader_Texture"));
        static_cast<CShader_Texture*>(pPrototype->Clone((void*)&tDesc));

        _float2 fTextureSize = m_NodeTextures.back()->m_pTextureCom->Get_TextureSize();

        _float fAspectRatio = fTextureSize.x / fTextureSize.y;
        _float fWinAspectRatio = (_float)g_iWinSizeX / (_float)g_iWinSizeY;

        if (fTextureSize.x > g_iWinSizeX || fTextureSize.y > g_iWinSizeY)
        {
            if (fTextureSize.x / g_iWinSizeX > fTextureSize.y / g_iWinSizeY)
            {
                fTextureSize.x = g_iWinSizeX;
                fTextureSize.y = g_iWinSizeX / fAspectRatio;
            }
            else
            {
                fTextureSize.y = g_iWinSizeY;
                fTextureSize.x = g_iWinSizeY * fAspectRatio;
            }
        }

        _int iRenderTargetCount = m_pRenderInstance->Add_ClientRenderTarget(iter.key.c_str(), iter.key.c_str(), fTextureSize.x, fTextureSize.y, DXGI_FORMAT_B8G8R8A8_UNORM, XMVectorSet(0.f, 0.f, 0.f, 0.f));
        /* 이미 렌더타겟이 중복이다 */
        if (iRenderTargetCount > 0)
        {
            SRV_Texture SRVDesc{};
            SRVDesc.iID = unique_node_id;
            SRVDesc.Texture = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(iter.key.c_str() + to_wstring(iRenderTargetCount));
            SRVDesc.Alpha = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(iter.key.c_str() + to_wstring(iRenderTargetCount) + L"_Alpha");

            if (SRVDesc.Texture == nullptr || SRVDesc.Alpha == nullptr)
            {
                int a = 10;
            }
            m_NodeTextureSRVs.push_back(SRVDesc);

            m_NodeTextures.back()->Set_PlusKey(iter.key.c_str() + to_wstring(iRenderTargetCount));

            if (m_PrototypeKeys[iCount].vTexCoord.x != 0.f && m_PrototypeKeys[iCount].vTexCoord.y != 0.f)
                m_NodeTextures.back()->m_vMultiple_Texcoord = m_PrototypeKeys[iCount].vTexCoord;

            m_NodeTextures.back()->m_iID = unique_node_id;
            node_ids.push_back(unique_node_id++);

            m_iNodeTextureCount++;

            continue;
        }

        SRV_Texture SRVDesc{};
        SRVDesc.iID = unique_node_id;
        SRVDesc.Texture = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(iter.key.c_str());
        wstring alphaKey = iter.key + L"_Alpha";
        SRVDesc.Alpha = (ImTextureID)m_pRenderInstance->Copy_RenderTarget_SRV(alphaKey.c_str());
        if (SRVDesc.Texture == nullptr || SRVDesc.Alpha == nullptr)
        {
            int a = 10;
        }
        m_NodeTextureSRVs.push_back(SRVDesc);


        if (m_PrototypeKeys[iCount].vTexCoord.x != 0.f && m_PrototypeKeys[iCount].vTexCoord.y != 0.f)
            m_NodeTextures.back()->m_vMultiple_Texcoord = m_PrototypeKeys[iCount].vTexCoord;

        m_NodeTextures.back()->m_iID = unique_node_id;
        node_ids.push_back(unique_node_id++);

        DragAcceptFiles(g_hWnd, TRUE);
        m_iNodeTextureCount++;
    }
}

void CIMGUI_Shader_Tab::Save_ClientBinary()
{

}

void CIMGUI_Shader_Tab::Check_Create_Link()
{
    // start 주는놈 end 받는놈
    _int start_attr = 0, end_attr = 0;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
    {
        auto it = find_if(links.begin(), links.end(),
            [end_attr](const pair<_int, _int>& link) {
                return link.second == end_attr;
            });

        if (it != links.end())
        {

            links.erase(it);
        }


        // 함수 노드들
        if (start_attr > 1500)
        {
            if (start_attr < 3000)
            {
                auto it = m_MoveTex_Node_ids.begin();
                advance(it, (start_attr - 1 - 1501));

                links.push_back(make_pair(start_attr, end_attr));
                auto Texture_nodeit = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == end_attr / m_iAttributeCount;
                    });

                if (Texture_nodeit != m_NodeTextures.end())
                {
                    (*Texture_nodeit)->Push_Shade_MoveTex(&it->fDirection, &it->fSpeed);
                    (*Texture_nodeit)->Remove_InputFunction(FUNCTION_SPRITE);
                }
            }
            else if (start_attr > 3000/* 노드 종류 추가되면 조건 더 달아야됨*/)
            {
                auto it = m_Sprite_Node_ids.begin();
                advance(it, (start_attr - 1 - 3001));

                links.push_back(make_pair(start_attr, end_attr));
                auto Texture_nodeit = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                    [&](CShader_Texture* texture) {
                        return texture->m_iID == end_attr / m_iAttributeCount;
                    });

                if (Texture_nodeit != m_NodeTextures.end())
                {
                    (*Texture_nodeit)->Push_Shade_Sprite(&it->fSpriteSizeNumber, &it->fSpeed);
                    (*Texture_nodeit)->Remove_InputFunction(FUNCTION_TEXMOVE);
                }

            }
        }
        // Main_Node Diffuse에 연결된거임
        else if (end_attr == -2)
        {
            m_iMain_Input_Diffuse_id = (start_attr) / 3;
            links.push_back(make_pair(start_attr, end_attr));

            auto SRVit = std::find_if(m_NodeTextureSRVs.begin(), m_NodeTextureSRVs.end(),
                [&](SRV_Texture SRV) {
                    return SRV.iID == start_attr / m_iAttributeCount;
                });
            //wstring abs = L"a";
            //wstring absb = L"b";
            //m_TestEffectModel_Texture = static_cast<CTexture*>(static_cast<CEffect_Single*>(CEffect_Manager::Get_Instance()->Find_In_Layer_Effect(abs, absb))->Get_Component(TEXT("Com_DiffuseTexture")));

            m_TestEffectModel_Texture->Set_SRV((ID3D11ShaderResourceView*)SRVit->Texture);
            m_TestEffectModel_Texture->Set_SRV((ID3D11ShaderResourceView*)SRVit->Alpha,1);
            //m_Effect_Rect->Push_Texture_Diffuse((ID3D11ShaderResourceView*)m_NodeTextureSRVs[m_iMain_Input_Diffuse_id - 1], 0);
        }
        else if (end_attr % 4 == 2)
        {
            links.push_back(make_pair(start_attr, end_attr));

            auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                [&](CShader_Texture* texture) {
                    return texture->m_iID == end_attr / m_iAttributeCount;
                });

            if (it != m_NodeTextures.end())
            {
                auto SRVit = std::find_if(m_NodeTextureSRVs.begin(), m_NodeTextureSRVs.end(),
                    [&](SRV_Texture SRV) {
                        return SRV.iID == start_attr / m_iAttributeCount;
                    });

                (*it)->Push_InputTextures((ID3D11ShaderResourceView*)SRVit->Texture,END_ALPHA);

            }
            //m_NodeTextures[(end_attr - 2) / 4 - 1]->Push_InputTextures((ID3D11ShaderResourceView*)m_NodeTextureSRVs[(start_attr) / 4 - 1].Texture, END_ALPHA);
        }
        else if (end_attr % 4 == 1)
        {
            links.push_back(make_pair(start_attr, end_attr));

            auto it = std::find_if(m_NodeTextures.begin(), m_NodeTextures.end(),
                [&](CShader_Texture* texture) {
                    return texture->m_iID == end_attr / m_iAttributeCount;
                });
            if (it != m_NodeTextures.end())
            {
                auto SRVit = std::find_if(m_NodeTextureSRVs.begin(), m_NodeTextureSRVs.end(),
                    [&](SRV_Texture SRV) {
                        return SRV.iID == start_attr / 4;
                    });

                (*it)->Push_InputTextures((ID3D11ShaderResourceView*)SRVit->Texture, END_DIFFUSE);

            }

           // m_NodeTextures[(end_attr - 1) / 4 - 1]->Push_InputTextures((ID3D11ShaderResourceView*)m_NodeTextureSRVs[(start_attr) / 4 - 1].Texture, END_DIFFUSE);
        }
    }
}

void CIMGUI_Shader_Tab::Save_Shader_Tab(std::string fileName, const Shader_Tab_Save& shaderTabSave, _int version) 
{
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) return;

    // 버전 정보 기록
    outFile << "Version: " << version << "\n";

    // Save_Key 저장
    outFile << "KeyCount: " << shaderTabSave.keys.size() << "\n";
    for (const auto& key : shaderTabSave.keys) {
        outFile << "KeySize: " << key.key.size() << "\n";
        // wstring을 string으로 변환하여 저장
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string keyStr = converter.to_bytes(key.key);
        outFile << "Key: " << keyStr << "\n";
        outFile << "ID: " << key.iD << "\n";

        // 버전 2부터 vTexCoord 저장
        if (version >= 2) {
            outFile << "vTexCoord: " << key.vTexCoord.x << " " << key.vTexCoord.y << "\n";
        }
    }

    // Node_Position 저장
    outFile << "NodePosCount: " << shaderTabSave.nodePositions.size() << "\n";
    for (const auto& nodePos : shaderTabSave.nodePositions) {
        outFile << "NodeID: " << nodePos.nodeID << " Position: " << nodePos.nodePosition.x << " " << nodePos.nodePosition.y << "\n";
    }

    // MoveTex_Node 저장
    outFile << "MoveTexNodeCount: " << shaderTabSave.moveTexNodes.size() << "\n";
    for (const auto& moveTexNode : shaderTabSave.moveTexNodes) {
        outFile << "MoveTexNodeID: " << moveTexNode.MoveTex_node_id
            << " Direction: " << moveTexNode.fDirection.x << " " << moveTexNode.fDirection.y
            << " Speed: " << moveTexNode.fSpeed << "\n";

    }

    // Sprite_Node 저장
    outFile << "SpriteNodeCount: " << shaderTabSave.spriteNodes.size() << "\n";
    for (const auto& spriteNode : shaderTabSave.spriteNodes) {
        outFile << "SpriteNodeID: " << spriteNode.Sprite_node_id
            << " IsLoop: " << spriteNode.isLoop
            << " SpriteSizeNumber: " << spriteNode.fSpriteSizeNumber.x << " " << spriteNode.fSpriteSizeNumber.y
            << " Speed: " << spriteNode.fSpeed << "\n";
    }

    // Link_Save 저장
    outFile << "LinkCount: " << shaderTabSave.links.size() << "\n";
    for (const auto& link : shaderTabSave.links) {
        outFile << "LinkSrcNodeID: " << link.srcNodeID
            << " LinkDestNodeID: " << link.destNodeID << "\n";
    }

    outFile.close();
}

void CIMGUI_Shader_Tab::Load_Shader_Tab(std::string fileName, Shader_Tab_Save& shaderTabSave) {
    std::ifstream inFile(fileName);
    if (!inFile.is_open())
        return;

    std::string line;
    int version = 1; // 기본 버전
    bool versionFound = false;

    // 첫 번째 줄에서 버전 정보 읽기
    if (std::getline(inFile, line)) {
        if (line.find("Version:") != std::string::npos) {
            sscanf_s(line.c_str(), "Version: %d", &version);
            versionFound = true;
        }
    }

    // Save_Key 로드
    if (std::getline(inFile, line)) 
    {
        size_t keyCount = std::stoul(line.substr(line.find(":") + 1));
        shaderTabSave.keys.resize(keyCount);

        for (auto& key : shaderTabSave.keys) {
            // KeySize 읽기
            std::getline(inFile, line);
            size_t keySize = std::stoul(line.substr(line.find(":") + 1));

            // Key 읽기
            std::getline(inFile, line); // Key 읽기
            std::string keyStr = line.substr(line.find(":") + 1);
            keyStr.erase(0, keyStr.find_first_not_of(" \n\r\t")); // 여백 제거

            // UTF-8 문자열을 wchar_t로 변환
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            key.key = converter.from_bytes(keyStr); // string을 wstring으로 변환

            // ID 읽기
            std::getline(inFile, line); // ID 읽기
            key.iD = std::stoi(line.substr(line.find(":") + 1));

            // vTexCoord 읽기 (버전 2 이상일 경우)
            if (version >= 2) {
                std::getline(inFile, line); // vTexCoord 읽기
                sscanf_s(line.c_str(), "vTexCoord: %f %f", &key.vTexCoord.x, &key.vTexCoord.y);
            }
        }
    }

    // Node_Position 로드
    if (std::getline(inFile, line)) {
        size_t nodePosCount = std::stoul(line.substr(line.find(":") + 1));
        shaderTabSave.nodePositions.resize(nodePosCount);
        for (auto& nodePos : shaderTabSave.nodePositions) {
            std::getline(inFile, line);
            sscanf_s(line.c_str(), "NodeID: %d Position: %f %f", &nodePos.nodeID, &nodePos.nodePosition.x, &nodePos.nodePosition.y);
        }
    }

    // MoveTex_Node 로드
    if (std::getline(inFile, line)) {
        size_t moveTexNodeCount = std::stoul(line.substr(line.find(":") + 1));
        shaderTabSave.moveTexNodes.resize(moveTexNodeCount);
        for (auto& moveTexNode : shaderTabSave.moveTexNodes) {
            std::getline(inFile, line);
            sscanf_s(line.c_str(), "MoveTexNodeID: %d Direction: %f %f Speed: %f",
                &moveTexNode.MoveTex_node_id, &moveTexNode.fDirection.x, &moveTexNode.fDirection.y, &moveTexNode.fSpeed);
        }
    }

    // Sprite_Node 로드
    if (std::getline(inFile, line)) {
        size_t spriteNodeCount = std::stoul(line.substr(line.find(":") + 1));
        shaderTabSave.spriteNodes.resize(spriteNodeCount);
        for (auto& spriteNode : shaderTabSave.spriteNodes) {
            std::getline(inFile, line);
            sscanf_s(line.c_str(), "SpriteNodeID: %d IsLoop: %d SpriteSizeNumber: %f %f Speed: %f",
                &spriteNode.Sprite_node_id, &spriteNode.isLoop, &spriteNode.fSpriteSizeNumber.x, &spriteNode.fSpriteSizeNumber.y, &spriteNode.fSpeed);
        }
    }

    // Link_Save 로드
    if (std::getline(inFile, line)) {
        size_t linkCount = std::stoul(line.substr(line.find(":") + 1));
        shaderTabSave.links.resize(linkCount);
        for (auto& link : shaderTabSave.links) {
            std::getline(inFile, line);
            sscanf_s(line.c_str(), "LinkSrcNodeID: %d LinkDestNodeID: %d",
                &link.srcNodeID, &link.destNodeID);
        }
    }

    inFile.close();
}

void CIMGUI_Shader_Tab::Delete_Clone_EffectToShader_Texture(CEffect* pEffect)
{
    if (m_CopyClones.empty() == true)
        return;

    auto& it = m_CopyClones.find(pEffect);

    if (it == m_CopyClones.end())
        return;
    
    for (auto& Textureiter : (*it->second))
    {
        Textureiter->Delete_CloneValue(pEffect);
    }

    m_CopyClones.erase(it);
}

void CIMGUI_Shader_Tab::Add_Clone_EffectToShader_Texture(CEffect* pEffect)
{
    m_CopyClones.emplace(pEffect, &m_NodeTextures);

    for (auto& iter : m_NodeTextures)
    {
        iter->Add_CloneValue(pEffect);
    }
   
}

_int CIMGUI_Shader_Tab::Update_Clone_EffectToShader_Texture(CEffect* pEffect, _float fTimeDelta)
{
    _int iResult = { 0 };
    auto& it = m_CopyClones.find(pEffect);

    if (it == m_CopyClones.end())
        return -1;

    for (auto& Textureiter : (*it->second))
    {
        if (Textureiter->Update_CloneValue(pEffect, fTimeDelta) == 1)
            iResult = 1;
    }

    return iResult;
}

// 로드 함수
//void CIMGUI_Shader_Tab::Load_Shader_Tab(string fileName, Shader_Tab_Save& shaderTabSave) {
//    std::ifstream inFile(fileName, std::ios::binary);
//    if (!inFile.is_open()) return;
//
//    size_t fileSize = GetFileSize(inFile);
//    int version = 1; // 기본 버전
//
//    //// 버전 정보가 포함된 새 포맷인지 확인
//    //if (fileSize >= sizeof(int)) {
//    //    inFile.read(reinterpret_cast<char*>(&version), sizeof(version));
//
//    //    // 버전 번호가 올바르지 않은 경우 기본값으로 설정하여 버전 정보가 없는 파일로 처리
//    //    if (inFile.fail() || version <= 0 || version > 2) {
//    //        inFile.clear();
//    //        inFile.seekg(0, std::ios::beg); // 파일 시작으로 돌아가서 버전 없는 경우로 읽기
//    //        version = 1;
//    //    }
//    //}
//
//    // Save_Key 로드
//    size_t keyCount;
//    inFile.read(reinterpret_cast<char*>(&keyCount), sizeof(keyCount));
//    shaderTabSave.keys.resize(keyCount);
//
//    for (auto& key : shaderTabSave.keys) {
//        size_t keySize;
//        inFile.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
//        key.key.resize(keySize);
//        inFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(key.key.data())), keySize * sizeof(wchar_t));
//        inFile.read(reinterpret_cast<char*>(&key.iD), sizeof(key.iD));
//
//        //// 버전 2 이상인 경우 vTexCoord 로드, 그렇지 않으면 기본값 설정
//        //if (version >= 2 && fileSize >= (inFile.tellg() + static_cast<std::streamoff>(sizeof(key.vTexCoord)))) {
//        //    inFile.read(reinterpret_cast<char*>(&key.vTexCoord), sizeof(key.vTexCoord));
//        //    if (inFile.fail()) {
//        //        key.vTexCoord = { 0.0f, 0.0f };
//        //        inFile.clear();
//        //    }
//        //}
//        //else {
//            key.vTexCoord = { 1.0f, 1.0f };
//       // }
//    }
//
//    // Node_Position 로드
//    size_t nodePosCount;
//    inFile.read(reinterpret_cast<char*>(&nodePosCount), sizeof(nodePosCount));
//    shaderTabSave.nodePositions.resize(nodePosCount);
//    inFile.read(reinterpret_cast<char*>(shaderTabSave.nodePositions.data()), nodePosCount * sizeof(Node_Position));
//
//    // MoveTex_Node 로드
//    size_t moveTexNodeCount;
//    inFile.read(reinterpret_cast<char*>(&moveTexNodeCount), sizeof(moveTexNodeCount));
//    shaderTabSave.moveTexNodes.resize(moveTexNodeCount);
//    inFile.read(reinterpret_cast<char*>(shaderTabSave.moveTexNodes.data()), moveTexNodeCount * sizeof(MoveTex_Node_Save));
//
//    // Sprite_Node 로드
//    size_t spriteNodeCount;
//    inFile.read(reinterpret_cast<char*>(&spriteNodeCount), sizeof(spriteNodeCount));
//    shaderTabSave.spriteNodes.resize(spriteNodeCount);
//    inFile.read(reinterpret_cast<char*>(shaderTabSave.spriteNodes.data()), spriteNodeCount * sizeof(Sprite_Node_Save));
//
//    // Link_Save 로드
//    size_t linkCount;
//    inFile.read(reinterpret_cast<char*>(&linkCount), sizeof(linkCount));
//    shaderTabSave.links.resize(linkCount);
//    inFile.read(reinterpret_cast<char*>(shaderTabSave.links.data()), linkCount * sizeof(Link_Save));
//
//    inFile.close();
//}

CIMGUI_Shader_Tab* CIMGUI_Shader_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, CEffect* pEffect)
{
    CIMGUI_Shader_Tab* pInstance = new CIMGUI_Shader_Tab(pDevice, pContext, pTexture, pEffect);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Shader_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CIMGUI_Shader_Tab* CIMGUI_Shader_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture)
{
    CIMGUI_Shader_Tab* pInstance = new CIMGUI_Shader_Tab(pDevice, pContext, pTexture);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CIMGUI_Shader_Tab"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIMGUI_Shader_Tab::Free()
{
    for (auto& iter : m_NodeTextures)
    {
        Safe_Release(iter);
    }

    //for (auto& iter : m_NodeTextureSRVs)
    //{
    //    Safe_Release(static_cast<ID3D11ShaderResourceView*>(iter.Texture));
    //}

    //Safe_Release(m_pDefaultTexture);

    //Safe_Release(m_TestEffectModel_Texture);
    //Safe_Release(m_pEffect);

    
    __super::Free();
}

CIMGUI_Shader_Tab* CIMGUI_Shader_Tab::Create_Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, string strFilename,CEffect* pEffect)
{
    CIMGUI_Shader_Tab* pInstance = new CIMGUI_Shader_Tab(pDevice, pContext, pTexture, pEffect);

    if (FAILED(pInstance->Load_Initialize(strFilename)))
    {
        MSG_BOX(TEXT("Failed to Created : CIMGUI_Shader_Tab"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIMGUI_Shader_Tab::RemoveSubstring(std::wstring& wstr, const std::wstring& toRemove)
{
    size_t pos = wstr.find(toRemove);
    if (pos != std::wstring::npos) {
        wstr.erase(pos, toRemove.length());
    }
}

string CIMGUI_Shader_Tab::WStringToString(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}


void CIMGUI_Shader_Tab::ShowTextWithoutPrefix(const std::wstring& wstr) 
{
    std::wstring modifiedStr = wstr;
    RemoveSubstring(modifiedStr, L"Prototype_Component_Texture_Effect_");
    std::string convertedStr = WStringToString(modifiedStr);
    ImGui::Text("%s", convertedStr.c_str());
}

size_t CIMGUI_Shader_Tab::GetRemainingFileSize(ifstream& file)
{
    size_t currentPos = file.tellg();
    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(currentPos, ios::beg);
    return fileSize - currentPos;
}

size_t CIMGUI_Shader_Tab::GetFileSize(ifstream& file)
{
    size_t currentPos = file.tellg();
    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(currentPos, ios::beg);
    return fileSize;
}
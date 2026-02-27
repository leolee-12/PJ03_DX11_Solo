#pragma once

#include "stdafx.h"
#include "IMGUI_Effect_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Effect_Manager.h"
#include "Imgui_Manager.h"
#include <string>
#include <locale>
#include <codecvt>
#include <IMGUI_Shader_Tab.h>
#include <Effect_Layer.h>
#include "Effect_Animation.h"
#include <algorithm>

const char* Effect[] = { "Test", "Layer", "Layer KeyFrame"};
const char* EffectType[] = { "NoneLight", "Blend", "ZNone", "Overlap" };

static int CurrentEffect = 0;
static int CurrentEffectType = 0;

static bool openKeyFrameWindow = false;
static bool openColorWindow = false;
static bool openLayerTransformWindow = false;
_float4 color = { 0.0f, 0.0f, 0.0f, 30.f };
static std::string selectedEffectName;
static int selectedFrame = -1;

static std::wstring selectedLayerName;
static bool initialized = false;

CIMGUI_Effect_Tab::CIMGUI_Effect_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CIMGUI_Tab{ pDevice,pContext }
    , m_pEffect_Manager{ CEffect_Manager::Get_Instance() }
{
    Safe_AddRef(m_pEffect_Manager);
}

HRESULT CIMGUI_Effect_Tab::Initialize()
{
    return S_OK;
}

void CIMGUI_Effect_Tab::Render(_float fTimeDelta)
{
    Push_Initialize();

    _uint i = m_pEffect_Manager->m_TestEffect.size();

    if (ImGui::BeginCombo("Menu", Effect[CurrentEffect]))
    {
        for (int i = 0; i < IM_ARRAYSIZE(Effect); i++) {
            bool isSelected = (CurrentEffect == i);
            if (ImGui::Selectable(Effect[i], isSelected)) {
                CurrentEffect = i;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    ImGui::SetNextItemWidth(200.0f); // 200 픽셀로 너비 설정
    //static char EffectNameBuffer[128] = "";
    //ImGui::InputText("File Name", EffectNameBuffer, IM_ARRAYSIZE(EffectNameBuffer));

    ImGui::SameLine();

    if (ImGui::Button("Select Layer Save"))
    {
        Save_Selected_Effects_File();
    }

    ImGui::SameLine();

    //if (ImGui::Button("All Layer Save"))
    //{
    //    Save_All_Effects_File();
    //}
    ImGui::Separator();
    ImGui::Separator();

    if (CurrentEffect == 0) 
    {
        Render_For_Each_Effect();
    }

    if (CurrentEffect == 1)
    {
        Render_For_Effect_Layer();
    }

    if (CurrentEffect == 2)
    {
        Render_For_Layer_KeyFrame(fTimeDelta);
    }

    if (openLayerTransformWindow)
        Render_For_Layer_Transform();
    if(openKeyFrameWindow)
      Render_For_Effect_KeyFrame();

    if (openColorWindow)
       Render_For_Effect_Color();

}

void CIMGUI_Effect_Tab::Push_Initialize()
{
    if (m_isInitialize == false)
    {
        for (const auto& pair : m_pEffect_Manager->m_EffectModel) {
            ModelName.push_back(WStringToUTF8(pair.first));
        }

        for (const auto& pair : m_pEffect_Manager->m_EffectTexture) {
            TextureName.push_back(WStringToUTF8(pair.first));
        }

        m_isInitialize = true;
    }
}

void CIMGUI_Effect_Tab::Save_To_Effect_Layer(_uint iCurTestEffectIndex, const wstring& strEffectLayerTag, void* pArg)
{
    m_pEffect_Manager->Add_Effect_To_Layer(iCurTestEffectIndex, strEffectLayerTag);
}

HRESULT CIMGUI_Effect_Tab::Save_All_Effects_File()
{
    const wstring folderPath = L"../Bin/Effects/Effect/";

    // 모든 레이어를 순회하면서 각각의 파일에 저장
    for (const auto& layerPair : m_pEffect_Manager->m_FinalEffects)
    {
        const wstring& layerName = layerPair.first;
        CEffect_Layer* pLayer = layerPair.second;

        // 파일 이름을 레이어 이름으로 설정
        wstring filePath = folderPath + layerName + L".txt";

        // EFFECT_LAYER_DATA에 현재 레이어 데이터 수집
        EFFECT_LAYER_DATA layerData;
        layerData.layerName = layerName;
        layerData.duration = pLayer->m_fDuration;
        layerData.tickPerSecond = pLayer->m_fTickPerSecond;
        layerData.keyFramesCount = pLayer->m_iNumKeyFrames;
        layerData.iNumEffect = pLayer->m_MixtureEffects.size();
        layerData.vPosition = pLayer->Get_Layer_Position();
        layerData.vScaled = pLayer->Get_Layer_Scaled();
        layerData.vRotation = pLayer->Get_Layer_Rotation();

        // 각 레이어 안의 이펙트 정보 추가
        for (auto& pEffect : pLayer->Get_Effects())
        {
            EFFECT_DATA effectData;
            effectData.effectName = pEffect->m_EffectName;
            effectData.modelName = pEffect->m_ModelName;
            effectData.diffuseTextureName = pEffect->m_DiffuseTextureName;
            effectData.effectType = pEffect->m_eEffect_Type;
            effectData.renderIndex = pEffect->m_iRenderIndex;
            effectData.passIndex = pEffect->m_iPassIndex;
            effectData.uniqueIndex = pEffect->m_iGameObjectData;
            effectData.isLoop = pEffect->m_bIsLoop;
            effectData.position = pEffect->Get_Effect_Position();
            effectData.scale = pEffect->Get_Effect_Scaled();
            effectData.rotation = pEffect->Get_Effect_Rotation();
            effectData.vColor = pEffect->m_vColor;
            effectData.vGlowColor = pEffect->m_vGlowColor;
            effectData.fGlowFactor = pEffect->m_fGlowFactor;
            effectData.iDerredPassIndex = pEffect->m_iDerredPassIndex;
            effectData.iNumKeyFrame = pEffect->m_pAnimation->m_EffectKeyFrames.size();

            effectData.maskTextureName = L"../Bin/Effects/Shader_Tab/" + layerData.layerName + pEffect->m_EffectName;
            CImgui_Manager::Get_Instance()->Save_Shader_Tab(pEffect->m_iUnique_Index, WStringToUTF8(effectData.maskTextureName));
            // 키프레임 정보 추가
            for (const auto& keyFramePair : pEffect->m_pAnimation->m_EffectKeyFrames)
            {
                EFFECT_KEYFRAME_DATA keyFrameData;
                keyFrameData.keyFrameNumber = keyFramePair.first;
                keyFrameData.position = keyFramePair.second.vPosition;
                keyFrameData.scale = keyFramePair.second.vScale;
                keyFrameData.rotation = keyFramePair.second.vRotation;
                keyFrameData.curTime = keyFramePair.second.fCurTime;
                keyFrameData.duration = keyFramePair.second.fDuration;
                keyFrameData.bIsNotPlaying = keyFramePair.second.bIsNotPlaying;

                effectData.keyframes.push_back(keyFrameData);
            }

            layerData.effects.push_back(effectData);
        }

        // m_vecEffectData 초기화하고 레이어 데이터 추가
        m_vecEffectData.clear();
        m_vecEffectData.push_back(layerData);

        // GameInstance에 있는 Save_Effects 함수로 m_vecEffectData 저장
        if (FAILED(m_pGameInstance->Save_Effects(filePath, &m_vecEffectData)))
        {
            return E_FAIL; // 저장 실패 시 오류 반환
        }
    }

    return S_OK; // 저장 성공 시 S_OK 반환
}

HRESULT CIMGUI_Effect_Tab::Save_Selected_Effects_File()
{
    const wstring folderPath = L"../Bin/Effects/Effect/";
    const wstring& layerName = selectedLayerName;  // 현재 선택된 레이어 이름
    CEffect_Layer* pLayer = m_pEffect_Manager->Find_Effect_Layer(layerName);

    if (!pLayer) return E_FAIL;

    // 파일 이름을 현재 선택된 레이어의 이름으로 설정
    wstring filePath = folderPath + layerName + L".txt";

    // EFFECT_LAYER_DATA에 현재 레이어 데이터 수집
    EFFECT_LAYER_DATA layerData;
    layerData.layerName = layerName;
    layerData.duration = pLayer->m_fDuration;
    layerData.tickPerSecond = pLayer->m_fTickPerSecond;
    layerData.keyFramesCount = pLayer->m_iNumKeyFrames;
    layerData.iNumEffect = pLayer->m_MixtureEffects.size();
    layerData.vPosition = pLayer->Get_Layer_Position();
    layerData.vScaled = pLayer->Get_Layer_Scaled();
    layerData.vRotation = pLayer->Get_Layer_Rotation();
    layerData.bIsFollowing = pLayer->m_bIsFollowing;

    // 각 레이어 안의 이펙트 정보 추가
    for (auto& pEffect : pLayer->Get_Effects())
    {
        EFFECT_DATA effectData;
        effectData.effectName = pEffect->m_EffectName;
        effectData.modelName = pEffect->m_ModelName;
        effectData.diffuseTextureName = pEffect->m_DiffuseTextureName;
        effectData.effectType = pEffect->m_eEffect_Type;
        effectData.renderIndex = pEffect->m_iRenderIndex;
        effectData.passIndex = pEffect->m_iPassIndex;
        effectData.uniqueIndex = pEffect->m_iGameObjectData;
        effectData.isLoop = pEffect->m_bIsBillboarding;
        effectData.position = pEffect->Get_Effect_Position();
        effectData.scale = pEffect->Get_Effect_Scaled();
        effectData.rotation = pEffect->Get_Effect_Rotation();
        effectData.vColor = pEffect->m_vColor;

        if (pEffect->m_bIsBackSideEffect)
        {
            effectData.vGlowColor.x = 1.f;
        }
        else
        {
            effectData.vGlowColor.x = 0.f;
        }

        if (pEffect->m_bIsShaderLoop)
        {
            effectData.vGlowColor.y = 1.f;
        }
        else
        {
            effectData.vGlowColor.y = 0.f;
        }

        if (effectData.uniqueIndex <= -2) //프리
        {
            effectData.fGlowFactor = pEffect->m_iObjectRenderData - 5 + 1;

        }
        else if (effectData.uniqueIndex == -1) //글로우 x
        {
            effectData.fGlowFactor = pEffect->m_iObjectRenderData;
        }
        else if (effectData.uniqueIndex >= 0)
        {
            effectData.fGlowFactor = pEffect->m_iObjectRenderData + 1;
        }

        effectData.iDerredPassIndex = pEffect->m_iDerredPassIndex;
        effectData.iNumKeyFrame = pEffect->m_pAnimation->m_EffectKeyFrames.size();

        effectData.maskTextureName = L"../Bin/Effects/Shader_Tab/" + layerData.layerName + pEffect->m_EffectName;

        CImgui_Manager::Get_Instance()->Save_Shader_Tab(pEffect->m_iUnique_Index, WStringToUTF8(effectData.maskTextureName));
        // 키프레임 정보 추가
        for (const auto& keyFramePair : pEffect->m_pAnimation->m_EffectKeyFrames)
        {
            EFFECT_KEYFRAME_DATA keyFrameData;
            keyFrameData.keyFrameNumber = keyFramePair.first;
            keyFrameData.position = keyFramePair.second.vPosition;
            keyFrameData.scale = keyFramePair.second.vScale;
            keyFrameData.rotation = keyFramePair.second.vRotation;
            keyFrameData.curTime = keyFramePair.second.fCurTime;
            keyFrameData.duration = keyFramePair.second.fDuration;
            keyFrameData.bIsNotPlaying = keyFramePair.second.bIsNotPlaying;

            effectData.keyframes.push_back(keyFrameData);
        }

        layerData.effects.push_back(effectData);
    }

    // m_vecEffectData 초기화하고 레이어 데이터 추가
    m_vecEffectData.clear();
    m_vecEffectData.push_back(layerData);

    // GameInstance에 있는 Save_Effects 함수로 m_vecEffectData 저장
    if (FAILED(m_pGameInstance->Save_Effects(filePath, &m_vecEffectData)))
    {
        return E_FAIL; // 저장 실패 시 오류 반환
    }

    return S_OK; // 저장 성공 시 S_OK 반환
}


string CIMGUI_Effect_Tab::WStringToUTF8(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

wstring CIMGUI_Effect_Tab::UTF8ToWString(const string& utf8Str)
{
    // UTF-8 문자열의 길이를 확인하고, 변환할 wstring의 길이를 계산합니다.
    int wstrLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);

    // 변환할 wstring의 크기를 할당
    std::wstring wstr(wstrLength, 0);

    // 변환 작업 수행
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], wstrLength);

    // 널 문자를 제거하고 반환
    wstr.pop_back(); // null character 제거
    return wstr;
}

void CIMGUI_Effect_Tab::Render_For_Each_Effect()
{
    static int CurrentModel = 0;
    static int CurrentMaskTexture = 0;
    static int CurrentDiffuseTexture = 0;

    if (CurrentEffect == 0)
    {
        if (ImGui::BeginCombo("EffectType", EffectType[CurrentEffectType]))
        {
            for (int i = 0; i < IM_ARRAYSIZE(EffectType); i++) {
                bool isSelected = (CurrentEffectType == i);
                if (ImGui::Selectable(EffectType[i], isSelected)) {
                    CurrentEffectType = i;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (!ModelName.empty() && ImGui::BeginCombo("Model Name", ModelName[CurrentModel].c_str()))
        {
            for (int i = 0; i < ModelName.size(); i++) {
                bool isSelected = (CurrentModel == i);
                if (ImGui::Selectable(ModelName[i].c_str(), isSelected)) {
                    CurrentModel = i;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }


        static char EffectNameBuffer[128] = "";
        ImGui::InputText("Effect Name", EffectNameBuffer, IM_ARRAYSIZE(EffectNameBuffer));

        ImGui::SameLine();
        if (ImGui::Button("Add Lying Rect"))
        {
            std::wstring wEffectName = UTF8ToWString(EffectNameBuffer);
            std::wstring wModelName = UTF8ToWString("Model_Effect_acmn_pivot_plane01");
            EFFECT_TYPE effectType = static_cast<EFFECT_TYPE>(CurrentEffectType);
            m_pEffect_Manager->Add_Test_Effect(effectType, &wEffectName, &wModelName);
        }

        ImGui::SameLine();

        if (ImGui::Button("Add Standing Rect"))
        {
            std::wstring wEffectName = UTF8ToWString(EffectNameBuffer);
            std::wstring wModelName = UTF8ToWString("Model_Effect_acmn_povot_plane00");
            EFFECT_TYPE effectType = static_cast<EFFECT_TYPE>(CurrentEffectType);
            m_pEffect_Manager->Add_Test_Effect(effectType, &wEffectName, &wModelName);
        }

        if (ImGui::Button("Add Effect"))
        {
            std::wstring wEffectName = UTF8ToWString(EffectNameBuffer);
            std::wstring wModelName = UTF8ToWString(ModelName[CurrentModel]);
            EFFECT_TYPE effectType = static_cast<EFFECT_TYPE>(CurrentEffectType);
            m_pEffect_Manager->Add_Test_Effect(effectType, &wEffectName, &wModelName);
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete Selected Effect"))
        {
            _uint EffectIndex = CImgui_Manager::Get_Instance()->Get_CurShaderTab_Index();

            m_pEffect_Manager->Delete_Test_Effect(CImgui_Manager::Get_Instance()->Get_CurShaderTab_Id());
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete All Effects"))
        {
            vector<_int> vecTabIndexs;


            vecTabIndexs = m_pEffect_Manager->Delete_All_Test_Effect();

            for (size_t i = 0; i < vecTabIndexs.size(); i++)
            {
                _int Index = vecTabIndexs[i];

                CImgui_Manager::Get_Instance()->Delete_Shader_Tab(Index);
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Change Color"))
        {

        }
        ImGui::Separator();
        ImGui::Text("Select Effect Information");

        _int EffectId = CImgui_Manager::Get_Instance()->Get_CurShaderTab_Id();
        _float3 CurPosition = m_pEffect_Manager->Get_Effect_Position(EffectId);
        _float3 CurScaled = m_pEffect_Manager->Get_Effect_Scaled(EffectId);
        _float3 CurRotation = m_pEffect_Manager->Get_Effect_Rotation(EffectId);

        ImGui::Text("Position");
        ImGui::Text("X"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Position X Slider", &CurPosition.x, -100.0f, 100.0f))
            m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition);
        ImGui::SameLine();
        if (ImGui::Button("-##Position X Dec")) { CurPosition.x -= 0.1f; m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition); }
        ImGui::SameLine();
        if (ImGui::Button("+##Position X Inc")) { CurPosition.x += 0.1f; m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition); }
        ImGui::SameLine();
        if (ImGui::InputFloat("##Position X", &CurPosition.x, 0.1f))
            m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition);

        ImGui::Text("Y"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Position Y Slider", &CurPosition.y, -100.0f, 100.0f))
            m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition);
        ImGui::SameLine();
        if (ImGui::Button("-##Position Y Dec")) { CurPosition.y -= 0.1f; m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition); }
        ImGui::SameLine();
        if (ImGui::Button("+##Position Y Inc")) { CurPosition.y += 0.1f; m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition); }
        ImGui::SameLine();
        if (ImGui::InputFloat("##Position Y", &CurPosition.y, 0.1f))
            m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition);

        ImGui::Text("Z"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Position Z Slider", &CurPosition.z, -100.0f, 100.0f))
            m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition);
        ImGui::SameLine();
        if (ImGui::Button("-##Position Z Dec")) { CurPosition.z -= 0.1f; m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition); }
        ImGui::SameLine();
        if (ImGui::Button("+##Position Z Inc")) { CurPosition.z += 0.1f; m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition); }
        ImGui::SameLine();
        if (ImGui::InputFloat("##Position Z", &CurPosition.z, 0.1f))
            m_pEffect_Manager->Set_Effect_Position(EffectId, CurPosition);

        ImGui::Separator();

        ImGui::Text("Scale");
        ImGui::Text("X"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Scale X Slider", &CurScaled.x, 0.01f, 100.0f))
            m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled);
        ImGui::SameLine();
        if (ImGui::Button("-##Scale X Dec")) { CurScaled.x = max(0.01f, CurScaled.x - 0.1f); m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled); }
        ImGui::SameLine();
        if (ImGui::Button("+##Scale X Inc")) { CurScaled.x += 0.1f; m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled); }
        ImGui::SameLine();
        if (ImGui::InputFloat("##Scale X", &CurScaled.x, 0.1f)) {
            CurScaled.x = max(0.01f, CurScaled.x);
            m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled);
        }

        ImGui::Text("Y"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Scale Y Slider", &CurScaled.y, 0.01f, 100.0f))
            m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled);
        ImGui::SameLine();
        if (ImGui::Button("-##Scale Y Dec")) { CurScaled.y = max(0.01f, CurScaled.y - 0.1f); m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled); }
        ImGui::SameLine();
        if (ImGui::Button("+##Scale Y Inc")) { CurScaled.y += 0.1f; m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled); }
        ImGui::SameLine();
        if (ImGui::InputFloat("##Scale Y", &CurScaled.y, 0.1f)) {
            CurScaled.y = max(0.01f, CurScaled.y);
            m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled);
        }

        ImGui::Text("Z"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Scale Z Slider", &CurScaled.z, 0.01f, 100.0f))
            m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled);
        ImGui::SameLine();
        if (ImGui::Button("-##Scale Z Dec")) { CurScaled.z = max(0.01f, CurScaled.z - 0.1f); m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled); }
        ImGui::SameLine();
        if (ImGui::Button("+##Scale Z Inc")) { CurScaled.z += 0.1f; m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled); }
        ImGui::SameLine();
        if (ImGui::InputFloat("##Scale Z", &CurScaled.z, 0.1f)) {
            CurScaled.z = max(0.01f, CurScaled.z);
            m_pEffect_Manager->Set_Effect_Scaled(EffectId, CurScaled);
        }

        ImGui::Separator();

        ImGui::Text("Rotation");
        ImGui::Text("X"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Rotation X Slider", &CurRotation.x, 0.0f, 360.0f)) {
            CurRotation.x = std::fmod(CurRotation.x, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        }
        ImGui::SameLine();
        if (ImGui::Button("-##Rotation X Dec")) {
            CurRotation.x = std::fmod(CurRotation.x - 1.0f + 360.0f, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        } ImGui::SameLine();
        if (ImGui::Button("+##Rotation X Inc")) {
            CurRotation.x = std::fmod(CurRotation.x + 1.0f, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        } ImGui::SameLine();
        if (ImGui::InputFloat("##Rotation X", &CurRotation.x, 1.0f)) {
            CurRotation.x = std::fmod(CurRotation.x, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        }

        ImGui::Text("Y"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Rotation Y Slider", &CurRotation.y, 0.0f, 360.0f)) {
            CurRotation.y = std::fmod(CurRotation.y, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        }
        ImGui::SameLine();
        if (ImGui::Button("-##Rotation Y Dec")) {
            CurRotation.y = std::fmod(CurRotation.y - 1.0f + 360.0f, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        } ImGui::SameLine();
        if (ImGui::Button("+##Rotation Y Inc")) {
            CurRotation.y = std::fmod(CurRotation.y + 1.0f, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        } ImGui::SameLine();
        if (ImGui::InputFloat("##Rotation Y", &CurRotation.y, 1.0f)) {
            CurRotation.y = std::fmod(CurRotation.y, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        }

        ImGui::Text("Z"); ImGui::SameLine();
        if (ImGui::SliderFloat("##Rotation Z Slider", &CurRotation.z, 0.0f, 360.0f)) {
            CurRotation.z = std::fmod(CurRotation.z, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        }
        ImGui::SameLine();
        if (ImGui::Button("-##Rotation Z Dec")) {
            CurRotation.z = std::fmod(CurRotation.z - 1.0f + 360.0f, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        } ImGui::SameLine();
        if (ImGui::Button("+##Rotation Z Inc")) {
            CurRotation.z = std::fmod(CurRotation.z + 1.0f, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        } ImGui::SameLine();
        if (ImGui::InputFloat("##Rotation Z", &CurRotation.z, 1.0f)) {
            CurRotation.z = std::fmod(CurRotation.z, 360.0f);
            m_pEffect_Manager->Set_Effect_Rotation(EffectId, CurRotation);
        }

        ImGui::Separator();
    }
}

void CIMGUI_Effect_Tab::Render_For_Effect_Layer()
{
    ImGui::InputText("New Layer", &EffectLayerKey[0], 128);

    if (ImGui::Button("Select Effect Save To New Layer"))
    {
        wstring strEffectLayerTag = UTF8ToWString(EffectLayerKey);

        _uint EffectIndex = CImgui_Manager::Get_Instance()->Get_CurShaderTab_Index();

        m_pEffect_Manager->Add_Effect_To_Layer(EffectIndex, strEffectLayerTag);

        EffectLayerKey.clear();
    }

    if (ImGui::Button("All Effect Save To New Layer"))
    {
        wstring strEffectLayerTag = UTF8ToWString(EffectLayerKey);

        m_pEffect_Manager->Add_All_Effect_To_Layer(strEffectLayerTag);

        EffectLayerKey.clear();
    }

    ImGui::Separator();

    auto LayerList = m_pEffect_Manager->Get_Layer_List();

    if (!LayerList.empty())
    {
        vector<string> layerListUTF8;
        for (const auto& layer : LayerList)
        {
            layerListUTF8.push_back(WStringToUTF8(layer)); // wstring을 UTF-8로 변환하는 함수가 필요
        }

        // 현재 선택된 레이어 인덱스를 유지할 변수
        static int selectedLayerIndex = 0;
        const char* currentLayer = layerListUTF8[selectedLayerIndex].c_str();

        if (ImGui::BeginCombo("Select Layer", currentLayer))
        {
            for (int i = 0; i < layerListUTF8.size(); i++)
            {
                bool isSelected = (selectedLayerIndex == i);
                if (ImGui::Selectable(layerListUTF8[i].c_str(), isSelected))
                {
                    selectedLayerIndex = i;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Add Selected Effect To Select Layer"))
        {
            _uint EffectIndex = CImgui_Manager::Get_Instance()->Get_CurShaderTab_Index();
            const wstring selectedLayerKey = LayerList[selectedLayerIndex];

            m_pEffect_Manager->Add_Effect_To_Layer(EffectIndex, selectedLayerKey);
        }

        // 선택된 레이어에 전체 객체 추가 버튼
        if (ImGui::Button("Add All Effects To Select Layer"))
        {
            const wstring selectedLayerKey = LayerList[selectedLayerIndex];

            m_pEffect_Manager->Add_All_Effect_To_Layer(selectedLayerKey);
        }

        if (ImGui::Button("Delete Layer"))
        {
            const wstring selectedLayerKey = LayerList[selectedLayerIndex];

            m_pEffect_Manager->Delete_Layer(selectedLayerKey);

            selectedLayerIndex = 0;
        }
    
    }
}

void CIMGUI_Effect_Tab::Render_For_Layer_KeyFrame(_float fTimeDelta)
{
    // 레이어 목록 가져오기
    auto LayerList = m_pEffect_Manager->Get_Layer_List();

    if (!LayerList.empty())
    {
        vector<string> layerListUTF8;
        for (const auto& layer : LayerList)
        {
            layerListUTF8.push_back(WStringToUTF8(layer)); // wstring을 UTF-8로 변환
        }

        static int selectedLayerIndex = 0;
        const char* currentLayer = layerListUTF8[selectedLayerIndex].c_str();

        // 레이어 선택 콤보 박스
        if (ImGui::BeginCombo("Select Layer", currentLayer))
        {
            for (int i = 0; i < layerListUTF8.size(); i++)
            {
                bool isSelected = (selectedLayerIndex == i);
                if (ImGui::Selectable(layerListUTF8[i].c_str(), isSelected))
                {
                    selectedLayerIndex = i;
                    selectedLayerName = LayerList[i]; // 선택된 레이어 이름을 저장
                    CurLayer = selectedLayerName;

                    if (CurLayer != PreLayer)
                    {
                        openColorWindow = false;
                        PreLayer = CurLayer;
                    }

                    m_pEffect_Manager->Set_Render_Layer(selectedLayerName);
                    m_pEffect_Manager->m_strSelectLayerName = selectedLayerName;
                    
                    CImgui_Manager::Get_Instance()->Set_CurEffectLayer(m_pEffect_Manager->Find_Effect_Layer(selectedLayerName));
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(70.0f, 0.0f)); // 가로 70px 간격 추가
        ImGui::SameLine();

        static bool isPlaying = false;
        CEffect_Layer* pLayer = m_pEffect_Manager->Find_Effect_Layer(selectedLayerName);

        if (ImGui::Button("Play"))
        {
            isPlaying = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Pause"))
        {
            isPlaying = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Anim Pos Reset"))
        {
            m_pEffect_Manager->Set_Layer_Animation_Position(selectedLayerName, 0.f);
        }

        if (isPlaying)
        {
            m_pEffect_Manager->Play_Layer_Animation(fTimeDelta, selectedLayerName);
        }

        int totalKeyframes = 0;
        const float frameInterval = 1.0f / 60.0f;

        if (pLayer)
        {

            float layerDuration = pLayer->m_fDuration;
            float tickPerSecond = pLayer->m_fTickPerSecond;
            float AnimCurPos = pLayer->m_fCurrentAnimPosition;

            ImGui::Text("Layer Duration:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f);
            if (ImGui::InputFloat("##LayerDuration", &layerDuration, 0.1f, 1.0f, "%.2f"))
            {
                pLayer->m_fDuration = max(0.0f, layerDuration);
            }

            ImGui::SameLine();
            ImGui::Text("Animation Speed:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f);
            if (ImGui::InputFloat("##AnimationSpeed", &tickPerSecond, 0.01f, 0.01f, "%.2f"))
            {
                pLayer->m_fTickPerSecond = max(0.01f, tickPerSecond);
            }

            ImGui::SameLine();
            ImGui::Text("Current Anim Pos:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f);
            if (ImGui::InputFloat("##AnimPos", &AnimCurPos, 0.001f, 0.001f, "%.3f"))
            {
                pLayer->m_fCurrentAnimPosition = max(0.0f, min(AnimCurPos, pLayer->m_fDuration));
                m_pEffect_Manager->Set_Layer_Animation_Position(selectedLayerName, AnimCurPos);
            }

            totalKeyframes = static_cast<int>(layerDuration / frameInterval);
            pLayer->m_iNumKeyFrames = totalKeyframes;

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(100.0f, 0.0f));
            if (ImGui::Button("Copy"))
            {
                CEffect_Layer::COPY_DESC tDesc{};
                XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());
                tDesc.pPlayertMatrix = &m_IdentityMatrix;
                m_pEffect_Manager->Copy_Layer(selectedLayerName,&tDesc);
                // 카피버튼 테스트용
            }

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(30.0f, 0.0f));
            ImGui::SameLine();
            if (ImGui::Button("Layer Transform"))
            {
                openLayerTransformWindow = true;
            }

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(30.0f, 0.0f));
            ImGui::SameLine();

            //팔로잉 체크박스

            bool isChecked = pLayer->m_bIsFollowing;

            if (ImGui::Checkbox("##Following", &isChecked))
            {
                pLayer->m_bIsFollowing = isChecked;
            }
        }

        ImGui::Separator();

      auto effectNames = m_pEffect_Manager->Get_In_Layer_Effect_List(&selectedLayerName);

        if (!effectNames.empty())
        {
            const int buttonSize = 20;
            const float effectNameWidth = 150.0f;

            ImGui::BeginChild("TimelineRegion", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);

            float currentXPosition = pLayer ? (pLayer->m_fCurrentAnimPosition / pLayer->m_fDuration) * (totalKeyframes * (buttonSize + 5.0f)) : 0.0f;

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 start = ImGui::GetCursorScreenPos();
            ImVec2 lineStart(start.x + effectNameWidth + currentXPosition, start.y);
            ImVec2 lineEnd(lineStart.x, lineStart.y + 300);

            drawList->AddLine(lineStart, lineEnd, IM_COL32(255, 0, 0, 255), 2.0f);
            lineStart.x -= 75.f;
            ImGui::SetCursorScreenPos(lineStart);
            ImGui::InvisibleButton("##DragRedLine", ImVec2(300.0f, 30.0f));

            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                float newXPos = ImGui::GetMousePos().x - start.x - effectNameWidth;
                float normalizedPosition = newXPos / (totalKeyframes * (buttonSize + 5.0f));
                pLayer->m_fCurrentAnimPosition = pLayer->m_fDuration * normalizedPosition;

                if (pLayer->m_fCurrentAnimPosition < 0.0f)
                    pLayer->m_fCurrentAnimPosition = 0.0f;

                if (pLayer->m_fCurrentAnimPosition > pLayer->m_fDuration)
                    pLayer->m_fCurrentAnimPosition = pLayer->m_fDuration;

                if (!isPlaying)
                    m_pEffect_Manager->Set_Layer_Animation_Position(selectedLayerName, pLayer->m_fCurrentAnimPosition);
            }

            std::vector<bool> effectChecks(effectNames.size(), false);
            std::vector<bool> effectBacksideChecks(effectNames.size(), false);
            std::vector<bool> effectShaderLoopChecks(effectNames.size(), false);

            for (int item = 0; item < effectNames.size(); item++)
            {
                CEffect* pEffect = m_pEffect_Manager->Find_In_Layer_Effect(selectedLayerName, effectNames[item]);
                if (pEffect)
                {
                    effectChecks[item] = pEffect->m_bIsBillboarding;
                    effectBacksideChecks[item] = pEffect->m_bIsBackSideEffect;
                    effectShaderLoopChecks[item] = pEffect->m_bIsShaderLoop;
                }
            }

            for (int item = 0; item < effectNames.size(); item++)
            {
                ImGui::PushID(item);

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(180, 197, 230, 230));
                ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(0, 0, 0, 230));

                bool isChecked = effectChecks[item];
                bool isBackSide = effectBacksideChecks[item];
                bool isShaderLoop = effectShaderLoopChecks[item];

                if (ImGui::Button("Change Color"))
                {
                    EffectName = effectNames[item];
                    openColorWindow = true;
                    m_pEffect_Manager->Find_In_Layer_Effect(selectedLayerName, EffectName)->m_IsColorEffect = true;
                }
                if (ImGui::Checkbox("##Bilboarding", &isChecked))
                {
                    CEffect* pEffect = m_pEffect_Manager->Find_In_Layer_Effect(selectedLayerName, effectNames[item]);
                    if (pEffect)
                    {
                        pEffect->m_bIsBillboarding = isChecked;
                    }
                    effectChecks[item] = isChecked;
                }

                ImGui::SameLine();

                if (ImGui::Checkbox("##BacksideEffect", &isBackSide))
                {
                    CEffect* pEffect = m_pEffect_Manager->Find_In_Layer_Effect(selectedLayerName, effectNames[item]);
                    if (pEffect)
                    {
                        pEffect->m_bIsBackSideEffect = isBackSide;
                    }
                    effectBacksideChecks[item] = isBackSide;
                }

                ImGui::SameLine();

                if (ImGui::Checkbox("##ShaderLoopEffect", &isShaderLoop))
                {
                    CEffect* pEffect = m_pEffect_Manager->Find_In_Layer_Effect(selectedLayerName, effectNames[item]);
                    if (pEffect)
                    {
                        pEffect->m_bIsShaderLoop = isShaderLoop;
                    }
                    effectShaderLoopChecks[item] = isShaderLoop;
                }

                ImGui::PopStyleColor(2);
                ImGui::PopStyleVar(2);

                string effectNameUTF8 = WStringToUTF8(effectNames[item]);
                ImGui::SameLine();
                ImGui::TextWrapped("%s", effectNameUTF8.c_str());
                ImGui::SameLine(effectNameWidth);
                ImGui::Dummy(ImVec2(30.0f, 0.0f));
                ImGui::SameLine();

                for (int frame = 0; frame < totalKeyframes; frame++)
                {
                    float curtime = frame * frameInterval;
                    ImGui::SetCursorPosX(effectNameWidth + frame * (buttonSize + 5.0f));
                    ImGui::PushID(frame + item * totalKeyframes);

                    bool isKeyFrameExist = m_pEffect_Manager->Find_KeyFrame(selectedLayerName, effectNames[item], frame);
                    if (isKeyFrameExist)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.8f, 0.3f, 0.7f));
                    }

                    if (ImGui::Button("##", ImVec2(buttonSize, buttonSize)))
                    {
                        selectedEffectName = effectNameUTF8;
                        selectedFrame = frame;
                        openKeyFrameWindow = true;
                        initialized = false;
                        SelectCurTime = curtime;
                    }

                    if (isKeyFrameExist)
                    {
                        ImGui::PopStyleColor();
                    }

                    ImGui::PopID();
                    ImGui::SameLine();
                }
                ImGui::NewLine();

                ImGui::PopID();
            }

            ImGui::EndChild();
        }
    }
}

void CIMGUI_Effect_Tab::Render_For_Effect_KeyFrame()
{
    EFFECT_KEYFRAME newKeyFrame;
    _float3 CurPosition = { 0.f, 0.f, 0.f };
    _float3 CurScale = { 0.f, 0.f, 0.f };
    _float3 CurRotation = { 0.f, 0.f, 0.f };
    _bool IsNotPlaying = { false };

    newKeyFrame.fDuration = m_pEffect_Manager->Find_Effect_Layer(selectedLayerName)->m_fDuration;

    // CurTime을 선택된 프레임 시간에 따라 자동 설정
    const float frameInterval = 1.0f / 60.0f;
    newKeyFrame.fCurTime = selectedFrame * frameInterval;

    ImGui::Begin("Keyframe Editor", &openKeyFrameWindow, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Editing Keyframe for Effect: %s", selectedEffectName.c_str());
    ImGui::Text("Frame: %d", selectedFrame);
    ImGui::SameLine();
    ImGui::Text("CurAnimPos: %.3f", newKeyFrame.fCurTime);
    ImGui::SameLine();
    ImGui::Text("Duration: %.2f", newKeyFrame.fDuration);

    // 처음 창을 열 때 저장된 키프레임 불러오기
    if (!initialized)
    {
        bool keyFrameExists = m_pEffect_Manager->Find_KeyFrame(selectedLayerName, UTF8ToWString(selectedEffectName), selectedFrame);
        if (keyFrameExists)
        {
            newKeyFrame = m_pEffect_Manager->Get_KeyFrame(selectedLayerName, UTF8ToWString(selectedEffectName), selectedFrame);
        }
        else
        {
            newKeyFrame = m_pEffect_Manager->Get_Near_Front_KeyFrame(selectedLayerName, UTF8ToWString(selectedEffectName), selectedFrame);
        }

        CurPosition = newKeyFrame.vPosition;
        CurScale = newKeyFrame.vScale;
        CurRotation = newKeyFrame.vRotation;
        IsNotPlaying = newKeyFrame.bIsNotPlaying;

        m_pEffect_Manager->Set_Layer_Effect_Position(selectedLayerName, UTF8ToWString(selectedEffectName), CurPosition);
        m_pEffect_Manager->Set_Layer_Effect_Scaled(selectedLayerName, UTF8ToWString(selectedEffectName), CurScale);
        m_pEffect_Manager->Set_Layer_Effect_Rotation(selectedLayerName, UTF8ToWString(selectedEffectName), CurRotation);

        //m_pEffect_Manager->Find_In_Layer_Effect(selectedLayerName, UTF8ToWString(selectedEffectName))->Set_Layer_Matrix();

        initialized = true;
    }
    else
    {
        CurPosition = m_pEffect_Manager->Get_Layer_Effect_Position(selectedLayerName, UTF8ToWString(selectedEffectName));
        CurScale = m_pEffect_Manager->Get_Layer_Effect_Scaled(selectedLayerName, UTF8ToWString(selectedEffectName));
        CurRotation = m_pEffect_Manager->Get_Layer_Effect_Rotation(selectedLayerName, UTF8ToWString(selectedEffectName));
        IsNotPlaying = m_pEffect_Manager->Get_Layer_Effect_IsPlaying(selectedLayerName, UTF8ToWString(selectedEffectName));

        m_pEffect_Manager->Set_Layer_Effect_Position(selectedLayerName, UTF8ToWString(selectedEffectName), CurPosition);
        m_pEffect_Manager->Set_Layer_Effect_Scaled(selectedLayerName, UTF8ToWString(selectedEffectName), CurScale);
        m_pEffect_Manager->Set_Layer_Effect_Rotation(selectedLayerName, UTF8ToWString(selectedEffectName), CurRotation);
    }



    ImGui::Text("Effect Keyframe Settings");

    // Is Not Playing 체크박스
    ImGui::Checkbox("Is Not Playing", &IsNotPlaying);
    {
        m_pEffect_Manager->Set_Layer_Effect_IsNotPlaying(selectedLayerName, UTF8ToWString(selectedEffectName), IsNotPlaying);
    }

    ImGui::Separator();

    _bool isPosClick = { false };
    // Position 섹션
    ImGui::Text("Position");

    ImGui::Text("X"); 
    ImGui::SameLine(); if (ImGui::SliderFloat("##Position X Slider", &CurPosition.x, -100.0f, 100.0f)) { isPosClick = true; }
    ImGui::SameLine(); if (ImGui::Button("-##Position X Dec")) { CurPosition.x -= 0.1f; isPosClick = true; }
    ImGui::SameLine(); if (ImGui::Button("+##Position X Inc")) { CurPosition.x += 0.1f; isPosClick = true; }
    ImGui::SameLine(); if (ImGui::InputFloat("##Position X Input", &CurPosition.x, 0.1f)) { isPosClick = true; }

    ImGui::Text("Y"); 
    ImGui::SameLine(); if (ImGui::SliderFloat("##Position Y Slider", &CurPosition.y, -100.0f, 100.0f)) { isPosClick = true; } 
    ImGui::SameLine(); if (ImGui::Button("-##Position Y Dec")) { CurPosition.y -= 0.1f; isPosClick = true; }
    ImGui::SameLine(); if (ImGui::Button("+##Position Y Inc")) { CurPosition.y += 0.1f; isPosClick = true; }
    ImGui::SameLine(); if (ImGui::InputFloat("##Position Y", &CurPosition.y, 0.1f))   { isPosClick = true; }

    ImGui::Text("Z"); 
    ImGui::SameLine(); if (ImGui::SliderFloat("##Position Z Slider", &CurPosition.z, -100.0f, 100.0f)) { isPosClick = true; }
    ImGui::SameLine(); if (ImGui::Button("-##Position Z Dec")) { CurPosition.z -= 0.1f; isPosClick = true; }
    ImGui::SameLine(); if (ImGui::Button("+##Position Z Inc")) { CurPosition.z += 0.1f; isPosClick = true; }
    ImGui::SameLine(); if (ImGui::InputFloat("##Position Z", &CurPosition.z, 0.1f)) { isPosClick = true; }

    ImGui::Separator();

    _bool isScaleClick = { false };
    // Scale 섹션
    ImGui::Text("Scale");

    ImGui::Text("X"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Scale X Slider", &CurScale.x, 0.0001f, 100.0f))
    {
        isScaleClick = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("-##Scale X Dec")) 
    { 
        CurScale.x = max(0.0001f, CurScale.x - 0.0001f); 
        isScaleClick = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("+##Scale X Inc")) 
    { 
        CurScale.x += 0.0001f; 
        isScaleClick = true;
    }
    ImGui::SameLine();
    if(ImGui::InputFloat("##Scale X", &CurScale.x, 0.0001f))
    {
        CurScale.x = max(0.0001f, CurScale.x);
        isScaleClick = true;
    }

    ImGui::Text("Y"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Scale Y Slider", &CurScale.y, 0.0001f, 100.0f))
    {
        isScaleClick = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("-##Scale Y Dec")) 
    { 
        CurScale.y = max(0.0001f, CurScale.y - 0.0001f); 
        isScaleClick = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("+##Scale Y Inc")) 
    { 
        CurScale.y += 0.0001f; 
        isScaleClick = true;
    }
    ImGui::SameLine();
    if(ImGui::InputFloat("##Scale Y", &CurScale.y, 0.0001f))
    {
        CurScale.y = max(0.0001f, CurScale.y);
        isScaleClick = true;
    }
    ImGui::Text("Z"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Scale Z Slider", &CurScale.z, 0.0001f, 100.0f))
    {
        isScaleClick = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("-##Scale Z Dec")) 
    { 
        CurScale.z = max(0.0001f, CurScale.z - 0.0001f); 
        isScaleClick = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("+##Scale Z Inc")) 
    { 
        CurScale.z += 0.0001f; 
        isScaleClick = true;
    }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Scale Z", &CurScale.z, 0.0001f))
    {
        CurScale.z = max(0.0001f, CurScale.z);
        isScaleClick = true;
       
    }
    ImGui::Separator();

    _bool isRotationClick = { false };
    // Rotation 섹션
    ImGui::Text("Rotation");

    ImGui::Text("X"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Rotation X Slider", &CurRotation.x, 0.0f, 360.0f))
    {
        isRotationClick = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("-##Rotation X Dec")) 
    { 
        CurRotation.x = fmod(CurRotation.x - 1.0f + 360.0f, 360.0f); 
        isRotationClick = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("+##Rotation X Inc")) 
    {
        CurRotation.x = fmod(CurRotation.x + 1.0f, 360.0f); 
        isRotationClick = true;
    }
    ImGui::SameLine();
    if(ImGui::InputFloat("##Rotation X", &CurRotation.x, 1.0f))
    {
        isRotationClick = true;
    }

    ImGui::Text("Y"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Rotation Y Slider", &CurRotation.y, 0.0f, 360.0f))
    {
        isRotationClick = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("-##Rotation Y Dec")) 
    {
        CurRotation.y = fmod(CurRotation.y - 1.0f + 360.0f, 360.0f); 
        isRotationClick = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("+##Rotation Y Inc")) 
    { 
        CurRotation.y = fmod(CurRotation.y + 1.0f, 360.0f); 
        isRotationClick = true;
    }
    ImGui::SameLine();
    if(ImGui::InputFloat("##Rotation Y", &CurRotation.y, 1.0f))
    {
        isRotationClick = true;
    }

    ImGui::Text("Z"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Rotation Z Slider", &CurRotation.z, 0.0f, 360.0f))
    {
        isRotationClick = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("-##Rotation Z Dec")) 
    { 
        CurRotation.z = fmod(CurRotation.z - 1.0f + 360.0f, 360.0f); 
        isRotationClick = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("+##Rotation Z Inc")) 
    {
        CurRotation.z = fmod(CurRotation.z + 1.0f, 360.0f); 
        isRotationClick = true;
    }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Rotation Z", &CurRotation.z, 1.0f))
    {
        isRotationClick = true;
    }


    if (isPosClick == true)
        m_pEffect_Manager->Set_Layer_Effect_Position(selectedLayerName, UTF8ToWString(selectedEffectName), CurPosition);

    if (isScaleClick == true)
        m_pEffect_Manager->Set_Layer_Effect_Scaled(selectedLayerName, UTF8ToWString(selectedEffectName), CurScale);

    if (isRotationClick == true)
    {

        m_pEffect_Manager->Set_Layer_Effect_Rotation(selectedLayerName, UTF8ToWString(selectedEffectName), CurRotation);
    }
  

    ImGui::Separator();

    // Keyframe 저장 버튼
    if (ImGui::Button("Save Keyframe"))
    {
        newKeyFrame.vPosition = CurPosition;
        newKeyFrame.vScale = CurScale;
        newKeyFrame.vRotation = CurRotation;
        newKeyFrame.bIsNotPlaying = IsNotPlaying;
        newKeyFrame.fCurTime = selectedFrame * frameInterval;

        // 업데이트된 newKeyFrame을 저장
        m_pEffect_Manager->Add_KeyFrame(selectedLayerName, UTF8ToWString(selectedEffectName), selectedFrame, newKeyFrame);

        ImGui::Text("Keyframe saved!");
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete Keyframe"))
    {
        m_pEffect_Manager->Delete_KeyFrame(selectedLayerName, UTF8ToWString(selectedEffectName), selectedFrame);
        ImGui::Text("Keyframe deleted!");
    }

    ImGui::Separator();
    ImGui::End();


    m_pEffect_Manager->Set_ParentMatrixMultiply_LocalMatrix(selectedLayerName, UTF8ToWString(selectedEffectName));
    
}

void CIMGUI_Effect_Tab::Render_For_Effect_Color()
{
    ImGui::Begin("Effect Color Edit", &openColorWindow, ImGuiWindowFlags_AlwaysAutoResize);

    // RGBA 값을 위한 세로바 슬라이더
    bool valueChanged = false;  // 값이 변경되었는지 확인

    color = m_pEffect_Manager->Find_In_Layer_Effect(selectedLayerName, EffectName)->m_vColor;

    ImGui::Dummy(ImVec2(5.0f, 1.0f));
    ImGui::SameLine();
    valueChanged |= ImGui::VSliderFloat("R", ImVec2(20, 160), &color.x, 0.0f, 255.0f, "");
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(1.0f, 1.0f));
    ImGui::SameLine();
    valueChanged |= ImGui::VSliderFloat("G", ImVec2(20, 160), &color.y, 0.0f, 255.0f, "");
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(1.0f, 1.0f));
    ImGui::SameLine();
    valueChanged |= ImGui::VSliderFloat("B", ImVec2(20, 160), &color.z, 0.0f, 255.0f, "");
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(1.0f, 1.0f));
    ImGui::SameLine();
    valueChanged |= ImGui::VSliderFloat("A", ImVec2(20, 160), &color.w, 0.0f, 30.0f, "");

    // 숫자 입력을 위한 필드
    valueChanged |= ImGui::InputFloat4("", reinterpret_cast<float*>(&color));

    // 0.5f씩 증가/감소하는 버튼 (RGB)
    if (ImGui::Button("+0.5 R")) { color.x = min(255.0f, max(0.0f, color.x + 0.5f)); valueChanged = true; }
    ImGui::SameLine();
    if (ImGui::Button("+0.5 G")) { color.y = min(255.0f, max(0.0f, color.y + 0.5f)); valueChanged = true; }
    ImGui::SameLine();
    if (ImGui::Button("+0.5 B")) { color.z = min(255.0f, max(0.0f, color.z + 0.5f)); valueChanged = true; }
    ImGui::SameLine();
    if (ImGui::Button("+0.05 A")) { color.w = min(1.0f, max(0.0f, color.w + 0.05f)); valueChanged = true; }

    if (ImGui::Button("-0.5 R")) { color.x = min(255.0f, max(0.0f, color.x - 0.5f)); valueChanged = true; }
    ImGui::SameLine();
    if (ImGui::Button("-0.5 G")) { color.y = min(255.0f, max(0.0f, color.y - 0.5f)); valueChanged = true; }
    ImGui::SameLine();
    if (ImGui::Button("-0.5 B")) { color.z = min(255.0f, max(0.0f, color.z - 0.5f)); valueChanged = true; }
    ImGui::SameLine();
    if (ImGui::Button("-0.05 A")) { color.w = min(30.0f, max(0.0f, color.w - 0.05f)); valueChanged = true; }


    if (valueChanged) 
    {
            m_pEffect_Manager->Set_Layer_Effect_Color(selectedLayerName, EffectName, color);
    }

    ImGui::End();
}

void CIMGUI_Effect_Tab::Render_For_Layer_Transform()
{

    ImGui::Begin("Edit Layer Transform", &openLayerTransformWindow, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Layer Transform");

    _float3 CurPosition = m_pEffect_Manager->Get_Layer_Position(selectedLayerName);
    _float3 CurScaled = m_pEffect_Manager->Get_Layer_Scaled(selectedLayerName);
    _float3 CurRotation = m_pEffect_Manager->Get_Layer_Rotation(selectedLayerName);

    ImGui::Text("Position");
    ImGui::Text("X"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Position X Slider", &CurPosition.x, -100.0f, 100.0f))
        m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition);
    ImGui::SameLine();
    if (ImGui::Button("-##Position X Dec")) { CurPosition.x -= 0.1f; m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition); }
    ImGui::SameLine();
    if (ImGui::Button("+##Position X Inc")) { CurPosition.x += 0.1f; m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition); }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Position X", &CurPosition.x, 0.1f))
        m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition);

    ImGui::Text("Y"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Position Y Slider", &CurPosition.y, -100.0f, 100.0f))
        m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition);
    ImGui::SameLine();
    if (ImGui::Button("-##Position Y Dec")) { CurPosition.y -= 0.1f; m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition); }
    ImGui::SameLine();
    if (ImGui::Button("+##Position Y Inc")) { CurPosition.y += 0.1f; m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition); }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Position Y", &CurPosition.y, 0.1f))
        m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition);

    ImGui::Text("Z"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Position Z Slider", &CurPosition.z, -100.0f, 100.0f))
        m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition);
    ImGui::SameLine();
    if (ImGui::Button("-##Position Z Dec")) { CurPosition.z -= 0.1f; m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition); }
    ImGui::SameLine();
    if (ImGui::Button("+##Position Z Inc")) { CurPosition.z += 0.1f; m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition); }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Position Z", &CurPosition.z, 0.1f))
        m_pEffect_Manager->Set_Layer_Position(selectedLayerName, CurPosition);

    ImGui::Separator();

    ImGui::Text("Scale");
    ImGui::Text("X"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Scale X Slider", &CurScaled.x, 0.01f, 100.0f))
        m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled);
    ImGui::SameLine();
    if (ImGui::Button("-##Scale X Dec")) { CurScaled.x = max(0.01f, CurScaled.x - 0.1f); m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled); }
    ImGui::SameLine();
    if (ImGui::Button("+##Scale X Inc")) { CurScaled.x += 0.1f; m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled); }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Scale X", &CurScaled.x, 0.1f)) {
        CurScaled.x = max(0.01f, CurScaled.x);
        m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled);
    }

    ImGui::Text("Y"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Scale Y Slider", &CurScaled.y, 0.01f, 100.0f))
        m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled);
    ImGui::SameLine();
    if (ImGui::Button("-##Scale Y Dec")) { CurScaled.y = max(0.01f, CurScaled.y - 0.1f); m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled); }
    ImGui::SameLine();
    if (ImGui::Button("+##Scale Y Inc")) { CurScaled.y += 0.1f; m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled); }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Scale Y", &CurScaled.y, 0.1f)) {
        CurScaled.y = max(0.01f, CurScaled.y);
        m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled);
    }

    ImGui::Text("Z"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Scale Z Slider", &CurScaled.z, 0.01f, 100.0f))
        m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled);
    ImGui::SameLine();
    if (ImGui::Button("-##Scale Z Dec")) { CurScaled.z = max(0.01f, CurScaled.z - 0.1f); m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled); }
    ImGui::SameLine();
    if (ImGui::Button("+##Scale Z Inc")) { CurScaled.z += 0.1f; m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled); }
    ImGui::SameLine();
    if (ImGui::InputFloat("##Scale Z", &CurScaled.z, 0.1f)) {
        CurScaled.z = max(0.01f, CurScaled.z);
        m_pEffect_Manager->Set_Layer_Scaled(selectedLayerName, CurScaled);
    }

    ImGui::Separator();

    ImGui::Text("Rotation");
    ImGui::Text("X"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Rotation X Slider", &CurRotation.x, 0.0f, 360.0f)) {
        CurRotation.x = std::fmod(CurRotation.x, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    }
    ImGui::SameLine();
    if (ImGui::Button("-##Rotation X Dec")) {
        CurRotation.x = std::fmod(CurRotation.x - 1.0f + 360.0f, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    } ImGui::SameLine();
    if (ImGui::Button("+##Rotation X Inc")) {
        CurRotation.x = std::fmod(CurRotation.x + 1.0f, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    } ImGui::SameLine();
    if (ImGui::InputFloat("##Rotation X", &CurRotation.x, 1.0f)) {
        CurRotation.x = std::fmod(CurRotation.x, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    }

    ImGui::Text("Y"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Rotation Y Slider", &CurRotation.y, 0.0f, 360.0f)) {
        CurRotation.y = std::fmod(CurRotation.y, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    }
    ImGui::SameLine();
    if (ImGui::Button("-##Rotation Y Dec")) {
        CurRotation.y = std::fmod(CurRotation.y - 1.0f + 360.0f, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    } ImGui::SameLine();
    if (ImGui::Button("+##Rotation Y Inc")) {
        CurRotation.y = std::fmod(CurRotation.y + 1.0f, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    } ImGui::SameLine();
    if (ImGui::InputFloat("##Rotation Y", &CurRotation.y, 1.0f)) {
        CurRotation.y = std::fmod(CurRotation.y, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    }

    ImGui::Text("Z"); ImGui::SameLine();
    if (ImGui::SliderFloat("##Rotation Z Slider", &CurRotation.z, 0.0f, 360.0f)) {
        CurRotation.z = std::fmod(CurRotation.z, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    }
    ImGui::SameLine();
    if (ImGui::Button("-##Rotation Z Dec")) {
        CurRotation.z = std::fmod(CurRotation.z - 1.0f + 360.0f, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    } ImGui::SameLine();
    if (ImGui::Button("+##Rotation Z Inc")) {
        CurRotation.z = std::fmod(CurRotation.z + 1.0f, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    } ImGui::SameLine();
    if (ImGui::InputFloat("##Rotation Z", &CurRotation.z, 1.0f)) {
        CurRotation.z = std::fmod(CurRotation.z, 360.0f);
        m_pEffect_Manager->Set_Layer_Rotation(selectedLayerName, CurRotation);
    }

    m_pEffect_Manager->Set_In_Layer_Effect_Layer_Transform(selectedLayerName);

    ImGui::End();
}


CIMGUI_Effect_Tab* CIMGUI_Effect_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIMGUI_Effect_Tab* pInstance = new CIMGUI_Effect_Tab(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CIMGUI_Effect_Tab"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIMGUI_Effect_Tab::Free()
{
    Safe_Release(m_pEffect_Manager);

    ModelName.clear();
    TextureName.clear();

    __super::Free();
}

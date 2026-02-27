#include "stdafx.h"
#include "IMGUI_Animation_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"

#include "imgui.h"

#include <fstream>  
#include <iostream> 
#include <vector>   
#include <string>   

//#include "Windows.h"
//#include "shellapi.h"

#include "Animation.h"
#include "Channel.h"




CIMGUI_Animation_Tab::CIMGUI_Animation_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}


//static FrameEventMap FrameEvent;// [CHARACTER_INDEX_END] [100] [2] ;

HRESULT CIMGUI_Animation_Tab::Initialize()
{

    m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();




    m_iTestModelIndex = PLAY_FRN;

  

    //static FrameEventMap FrameEvent[CHARACTER_INDEX_END][100][2];
  //  FrameEvent[SELECT_HIT][0][5].emplace("TEST");
  //  FrameEvent[SELECT_HIT][0][50].push_back("MotionLock,1,10");
  //  FrameEvent[SELECT_HIT][1][0].push_back("ZERO");


    //FrameEvent[PlAYER_GOKU][ATTACK][50].push_back("TEST");

    //애니메이션 재생속도 자체를 0으로 바꾸고  0.1초 뒤에  특정 프레임으로 움직인다던가 하는 함수가 필요할듯
   
    /*
    animation lock and jump(_float fLockTime, _float fNextPosition);   
    {
        m_fAccLockTime= 0.f;
        m_fLockTime = fLockTime;
        _bool AnimationLock = true;
        m_fNextPosition = fNextPosition;
    }

    if(m_fAccLockTime > fLockTime)
    {
        AnimationLock = false;
        m_fCurrentPosition = m_fNextPosition;
       
       m_fAccLockTime=0.f;
       m_fLockTime=0.f;
       m_fNextPosition=0.f;
    }

    //만들었다고 치는데 이 복잡한걸 어떻게 string으로 처리함?
    */


   // CFrameEvent_Manager::Get_Instance()->Add_Event(TEXT("MotionLock,1.02f,100"));



   //첫 실행일때, 루프돌았을때 ZeroFrame을 어떻게 구분하는가
  

   
    //이거 ModelPreview로 이사감
    //CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
   // CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_Frieza.txt");



	return S_OK;
}


static int g_iCurrentAnimationIndex = 999;
static bool g_isLoop = false;

void CIMGUI_Animation_Tab::Render(_float fTimeDelta)
{
    



    ImGui::InputText("Label", buffer, IM_ARRAYSIZE(buffer));

    if (ImGui::Button("Label Load"))
    {

        //../Bin/ModelData\Hit_Select.bin

        if (m_pSelectedModelCom != nullptr)
            Safe_Release(m_pSelectedModelCom);

        _fmatrix preMatrix = XMMatrixIdentity();
       m_pSelectedModelCom = CModel::Create(m_pDevice, m_pContext, buffer, preMatrix);

    }

    ImGui::SameLine();
    if (ImGui::Button("Object Load"))
    {


        if (m_pSelectedModelCom != nullptr)
            Safe_Release(m_pSelectedModelCom);

       
        m_pSelectedObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Preview"), 0);
        m_pSelectedModelCom = static_cast<CModel*>(m_pSelectedObject->Get_Component(TEXT("Com_Model")));

        m_pSelectedObjectTransformCom = static_cast<CTransform*>(m_pSelectedObject->Get_Component(TEXT("Com_Transform")));

       
        m_pSelectedModelCom->m_bNoMoveXZ = true;

    }

    //ImGui::NewLine();
    if (ImGui::Button("Event ReLoad"))
    {
        CFrameEvent_Manager::Get_Instance()->ReLoadFrameEvent(m_EventTXTBuffer);
    }

    ImGui::SameLine();
    if (ImGui::Button("Event Delete"))
    {
        CFrameEvent_Manager::Get_Instance()->ClearFrameEvent();
    }


    ImGui::NewLine();
    if (m_pSelectedModelCom != nullptr)
    {

         if (ImGui::Button("AnimationName Export"))
         {
             ExportAllAnimationToTXT();
         }
         ImGui::SameLine();
         if (ImGui::Button("Select Position Reset"))
         {
             CTransform* pTrasnform = static_cast<CTransform*>(m_pSelectedObject->Get_Component(TEXT("Com_Transform")));
             pTrasnform->Set_State(CTransform::STATE_POSITION, { 0,0,0,1 });

         }
         ImGui::SameLine();
         if (ImGui::Button("Toggle NoMoveXZ"))
         {

             // Layer_Camera
             //CGameObject* pObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));
             //CTransform* pTrasnform = static_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
             //
             ////_vector Debug = pTrasnform->Get_State(CTransform::STATE_POSITION);
             ////_vector Debug2 = pTrasnform->Get_State(CTransform::STATE_LOOK);
             //
             //
             //pTrasnform->Set_State(CTransform::STATE_POSITION, { 0,1,-5,1 });
             //pTrasnform->LookAt({0, 0, 0, 1});



             m_pSelectedModelCom->m_bNoMoveXZ = !m_pSelectedModelCom->m_bNoMoveXZ;

         }
         Info_Anim();

        

         if (m_bMotionPlaying)
         {

             _float fPrePosition = m_pSelectedModelCom->m_fCurrentAnimPosition;

             _int iOneFrameTeest = 0;

             if (fPrePosition == 0)
             {
                 ProcessEventsFramesZero(m_iTestModelIndex, g_iCurrentAnimationIndex);
                 fPrePosition += 0.001;

                 iOneFrameTeest++;
             }

       

             //  if (m_pSelectedModelCom->Play_Animation_Lick(fTimeDelta))
             if (m_pSelectedModelCom->Play_Animation_Lick2(fTimeDelta, m_pSelectedObjectTransformCom))
             {
                 //모션이 끝났으면, 루프면    (아까까진 루프가 아니였는데 이번에 루프면 어쩌지?)
                 if(m_pSelectedModelCom->m_isLoopAnim)
                 {
                     fPrePosition = 0.001;
                     ProcessEventsFramesZero(m_iTestModelIndex, g_iCurrentAnimationIndex);
                     iOneFrameTeest++;
                 }
             }

             iOneFrameTeest;
             
             if (iOneFrameTeest > 1)
             {
                 _bool bDebug = false;
             }

             _float fCurPosition = m_pSelectedModelCom->m_fCurrentAnimPosition;
             
            
             ProcessEventsBetweenFrames2(0, m_pSelectedModelCom->m_iCurrentAnimationIndex, fPrePosition, fCurPosition);

         }

         if (m_bCurrentPositionSlide)
         {
             _float fCurrentPosition =(m_pSelectedModelCom->m_fCurrentAnimPosition);

             ImGui::Text("CurrentAnimPosition :%f ", (m_pSelectedModelCom->m_fCurrentAnimPosition));
             float maxValue = m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex]->m_fDuration;       // 슬라이더 최대값
             if (ImGui::SliderFloat("Current Position", &(m_pSelectedModelCom->m_fCurrentAnimPosition), 0, maxValue))
             {
                 
                 //줄어들면 0으로 줄였다가 재실행?
                 if (fCurrentPosition > m_pSelectedModelCom->m_fCurrentAnimPosition)
                 {
                     _float fSavePosition = m_pSelectedModelCom->m_fCurrentAnimPosition;

                     m_pSelectedModelCom->SetUp_Animation(0, false, 0);
                     m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, g_isLoop);
                     //m_pSelectedModelCom->Play_Animation(fSavePosition / 25.f);
                     m_pSelectedModelCom->Play_Animation(fSavePosition/ m_pSelectedModelCom->Get_pCurrentAnimation()->m_fTickPerSecond);  


                 }
                 //늘어났다면   같은경우 있으니 if 추가해야됨
                 else if (fCurrentPosition < m_pSelectedModelCom->m_fCurrentAnimPosition)
                 {
                     //차이만큼 PlayAnimation
                     _float fdifference = m_pSelectedModelCom->m_fCurrentAnimPosition - fCurrentPosition;
                     m_pSelectedModelCom->Play_Animation(fdifference / m_pSelectedModelCom->Get_pCurrentAnimation()->m_fTickPerSecond);

                 }

             }


         }

    }

    _bool DebugPoint = false;

    
    SHORT currKeyState = GetAsyncKeyState(VK_SPACE);  // 예시로 스페이스바를 사용

    // 키가 눌리는 순간만 감지
    if ((currKeyState & 0x8000) && !(prevKeyState & 0x8000))
    {
        // 키가 눌리는 순간
        m_bMotionPlaying = !m_bMotionPlaying;
    }

    // 현재 상태를 이전 상태로 업데이트
    prevKeyState = currKeyState;
}

void CIMGUI_Animation_Tab::Info_Anim()
{
    //999로 한 이유는 0으로 하면 터져서 
   // static int g_iCurrentAnimationIndex = 999;


    vector<const char*> animNames;

    //태완
   // for (const auto& anim : m_pSelectedModelCom->m_ModelData.animations) {
   //     animNames.push_back(anim.szName);
   // }

    //적용 후
    for (auto AnimData : m_pSelectedModelCom->m_Animations)
    {
        animNames.push_back(AnimData->m_szName);
    }


    ImGui::Text("Selected Animation");
    ImGui::Combo("##", &g_iCurrentAnimationIndex, animNames.data(), animNames.size());

    if (g_iCurrentAnimationIndex < animNames.size())
    {
        // 현재 선택된 애니메이션 데이터를 가져옴
         
        //Ver.태완
        //const auto& selectedAnim = m_pSelectedModelCom->m_ModelData.animations[g_iCurrentAnimationIndex];


        const auto& selectedAnim = m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex];

        // 애니메이션의 각 멤버를 ImGui::Text로 표시
        ImGui::Text("AnimIndex: %d", g_iCurrentAnimationIndex);
        ImGui::Text("Name: %s", selectedAnim->m_szName);
        ImGui::Text("Duration: %f", selectedAnim->m_fDuration);
        ImGui::Text("Ticks Per Second: %f", selectedAnim->m_fTickPerSecond);
        ImGui::Text("Number of Channels: %u", selectedAnim->m_iNumChannels);

        // 각 채널을 하나로 묶어 트리 노드로 표시
        if (ImGui::TreeNode("Channels"))
        {
            // 각 채널의 정보 표시
            for (size_t i = 0; i < selectedAnim->m_Channels.size(); ++i)
            {
                const auto& channel = selectedAnim->m_Channels[i];
                
               //const auto channel = selectedAnim->m_Channels[i];


                //if (ImGui::TreeNode((void*)(intptr_t)i, "Channel %u: %s", static_cast<unsigned>(i), channel->m_szName))
                if (ImGui::TreeNode((void*)(intptr_t)i, "Channel %u: %s", static_cast<unsigned>(i), channel->GetName()))
                {
                   //ImGui::Text("  Name: %s", channel.m_szName);
                   //ImGui::Text("  Bone Index: %u", channel.iBoneIndex);
                   //ImGui::Text("  Number of KeyFrames: %u", channel.iNumKeyFrames);

                    ImGui::Text("  Name: %s", channel->GetName());
                    ImGui::Text("  Bone Index: %u", channel->m_iBoneIndex);
                    ImGui::Text("  Number of KeyFrames: %u", channel->m_iNumKeyFrames);

                    // 각 키프레임의 정보 표시
                    if (ImGui::TreeNode("KeyFrames"))
                    {
                        for (size_t j = 0; j < channel->m_KeyFrames.size(); ++j)
                        {
                            const auto& keyFrame = channel->m_KeyFrames[j];
                            if (ImGui::TreeNode((void*)(intptr_t)j, "KeyFrame %u", static_cast<unsigned>(j)))
                            {
                                ImGui::Text("    Time: %f", keyFrame.fTime);
                                ImGui::Text("    Scale: [%f, %f, %f]", keyFrame.vScale.x, keyFrame.vScale.y, keyFrame.vScale.z);
                                ImGui::Text("    Rotation: [%f, %f, %f, %f]", keyFrame.vRotation.x, keyFrame.vRotation.y, keyFrame.vRotation.z, keyFrame.vRotation.w);
                                ImGui::Text("    Position: [%f, %f, %f]", keyFrame.vPosition.x, keyFrame.vPosition.y, keyFrame.vPosition.z);
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::Checkbox("Loop", &g_isLoop);
        if (ImGui::Button("Play"))
        {

           //if (m_pSelectedModelCom->m_fCurrentAnimPosition == m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex]->m_fDuration)
           //{
           //    m_pSelectedModelCom->m_fCurrentAnimPosition = 0.f;
           //}
            m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, g_isLoop);
            
           // ProcessEventsFramesZero(m_iTestModelIndex, g_iCurrentAnimationIndex);
           

            //ProcessEventsFramesZero((int)SELECT_HIT, g_iCurrentAnimationIndex);
            m_bMotionPlaying = true;
            m_bCurrentPositionSlide = true;

        }

        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            m_bMotionPlaying = false;

        }
        
        if (ImGui::Button("Toggle"))
        {
            m_bMotionPlaying = !m_bMotionPlaying;

        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {

            m_bMotionPlaying = false;
            m_bCurrentPositionSlide = false;
            m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, false, 0);
            m_pSelectedModelCom->m_fCurrentAnimPosition = 0.f;
        }

        if (ImGui::SliderFloat("AnimationUpdateTime", &m_fAnimationUpdateTime, 0.005, 1.f))
        {
            m_pSelectedModelCom->Set_MaxAnimationUpdate_Time(m_fAnimationUpdateTime);
        }
        if (ImGui::SliderFloat("TickPerSecond", &m_fTool_TickPerSecond, 0, 120.f))
        {
            m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex]->m_fTickPerSecond = m_fTool_TickPerSecond;
        }

        if (ImGui::Button("Save Time&Speed  to this Animation&Position"))
        {
            SaveEventTXT(m_EventTXTBuffer, Get_strAnimationEvent());

        }

        ImGui::InputText("Target Position", m_PostionBuffer, IM_ARRAYSIZE(m_PostionBuffer));
        if (ImGui::Button("Position Move"))
        {

            _float fTarget_Position = stof(m_PostionBuffer);


            m_pSelectedModelCom->SetUp_Animation(0, false, 0);
            m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, g_isLoop);
            //m_pSelectedModelCom->Play_Animation(fSavePosition / 25.f);
            m_pSelectedModelCom->Play_Animation(fTarget_Position / m_pSelectedModelCom->Get_pCurrentAnimation()->m_fTickPerSecond);

        }
    }
}


 
void CIMGUI_Animation_Tab::SelectedModelLoad(std::ifstream& inFile, BoneData& bone)
{
    m_pSelectedModelCom->LoadBoneFromBinary(inFile, bone);
}



//void CIMGUI_Animation_Tab::LoadBoneFromBinary_Tool(std::ifstream& inFile, BoneData& bone) {
//    // BoneData의 이름 읽기
//    size_t nameLength;
//    inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
//    bone.name.resize(nameLength);
//    inFile.read(&bone.name[0], nameLength);
//    
//    // BoneData의 변환 행렬 읽기
//    inFile.read(reinterpret_cast<char*>(&bone.transformationMatrix), sizeof(XMFLOAT4X4));
//    
//    // BoneData의 자식 뼈대 읽기
//    uint32_t numChildren;
//    inFile.read(reinterpret_cast<char*>(&numChildren), sizeof(numChildren));
//    bone.children.resize(numChildren);
//    for (auto& child : bone.children) {
//        LoadBoneFromBinary_Tool(inFile, child);
//    }
//}

void CIMGUI_Animation_Tab::LoadFromFile(const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // ModelHeader 읽기
    ModelHeader header;
    inFile.read(reinterpret_cast<char*>(&header), sizeof(header));
    std::cout << "Loaded header: " << header.numMeshes << " meshes, "
        << header.numMaterials << " materials, "
        << header.numAnimations << " animations" << std::endl;

    std::vector<MeshData> meshes(header.numMeshes);
    for (auto& mesh : meshes) {
        // Mesh 이름 읽기
        size_t nameLength;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        mesh.name.resize(nameLength);
        inFile.read(&mesh.name[0], nameLength);

        // Vertex와 Index 개수 읽기
        inFile.read(reinterpret_cast<char*>(&mesh.vertexCount), sizeof(mesh.vertexCount));
        inFile.read(reinterpret_cast<char*>(&mesh.indexCount), sizeof(mesh.indexCount));

        // Anim 여부에 따른 Vertex 데이터 읽기
        if (header.isAnim) {
            mesh.animVertices.resize(mesh.vertexCount);
            inFile.read(reinterpret_cast<char*>(mesh.animVertices.data()), mesh.vertexCount * sizeof(VertexAnim));
        }
        else {
            mesh.vertices.resize(mesh.vertexCount);
            inFile.read(reinterpret_cast<char*>(mesh.vertices.data()), mesh.vertexCount * sizeof(Vertex));
        }

        // Index 데이터 읽기
        mesh.indices.resize(mesh.indexCount);
        inFile.read(reinterpret_cast<char*>(mesh.indices.data()), mesh.indexCount * sizeof(unsigned int));

        // Material index 읽기
        inFile.read(reinterpret_cast<char*>(&mesh.materialIndex), sizeof(mesh.materialIndex));

        if (header.isAnim) {
            // 뼈대 정보 읽기
            uint32_t numBones;
            inFile.read(reinterpret_cast<char*>(&numBones), sizeof(numBones));
            mesh.bones.resize(numBones);
            for (auto& bone : mesh.bones) {
                //LoadBoneFromBinary(inFile, bone);
               m_pSelectedModelCom->LoadBoneFromBinary(inFile, bone);

            }

           //SelectedModelLoad(inFile, bone);
           //m_pSelectedModelCom->LoadBoneFromBinary(inFile, bone);


            // 오프셋 행렬 읽기
            uint32_t numOffsetMatrices;
            inFile.read(reinterpret_cast<char*>(&numOffsetMatrices), sizeof(numOffsetMatrices));
            mesh.offsetMatrices.resize(numOffsetMatrices);
            inFile.read(reinterpret_cast<char*>(mesh.offsetMatrices.data()), numOffsetMatrices * sizeof(XMFLOAT4X4));
        }
    }

    // MaterialData 읽기
    std::vector<MaterialData> materials(header.numMaterials);
    for (auto& material : materials) {
        for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
            size_t pathLength;
            inFile.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
            material.texturePaths[i].resize(pathLength);
            inFile.read(&material.texturePaths[i][0], pathLength);

            std::cout << "Loaded texture path: " << material.texturePaths[i] << std::endl;
        }
    }

    // AnimationData 읽기
    std::vector<AnimationData> animations(header.numAnimations);
    if (header.isAnim) {
        for (auto& anim : animations) {
            // Animation 이름 읽기
            size_t nameLength;
            inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            anim.name.resize(nameLength);
            inFile.read(&anim.name[0], nameLength);

            // Animation duration과 ticks per second 읽기
            inFile.read(reinterpret_cast<char*>(&anim.duration), sizeof(anim.duration));
            inFile.read(reinterpret_cast<char*>(&anim.ticksPerSecond), sizeof(anim.ticksPerSecond));

            // Animation 채널 읽기
            uint32_t numChannels;
            inFile.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
            anim.channels.resize(numChannels);
            for (auto& channel : anim.channels) {
                // 채널 이름 읽기
                size_t channelNameLength;
                inFile.read(reinterpret_cast<char*>(&channelNameLength), sizeof(channelNameLength));
                channel.name.resize(channelNameLength);
                inFile.read(&channel.name[0], channelNameLength);

                // Keyframes 읽기
                uint32_t numKeyframes;
                inFile.read(reinterpret_cast<char*>(&numKeyframes), sizeof(numKeyframes));
                channel.keyframes.resize(numKeyframes);
                inFile.read(reinterpret_cast<char*>(channel.keyframes.data()), numKeyframes * sizeof(Keyframe));
            }
        }
    }

    inFile.close();
}

void CIMGUI_Animation_Tab::ProcessEventsBetweenFrames2(int characterIndex, int animationIndex, float prevFrame, float currentFrame)
{
    //Frame이 멈춰서 중복처리되는경우는?
   // 프레임이 0인경우를 제외하고 프레임이 멈췄는데 정확하게 이벤트가 있을 수 있나?
       // 0.001초단위로 보면 불가능한건 아님

   //해결방법
   // 1. 이 함수를 play animation 주변에 넣어서, 애니메이션이 정지한경우 이 함수 호출 자체를 안함.  << 정석
   // 2. bool값을 만들어서 1회만 처리

   //일단 보류.




   //frame이 0인경우는 애니메이션 실행 말고, 애니메이션 변경시 0의 이벤트만 찾아서 하는걸로 처리.   (ProcessEventsFramesZero)
    if (prevFrame == 0)
    {

        //과거,현재 프레임이 모두 0으로 애니메이션 자체가 정지된 경우 함수 끝
        if (currentFrame == 0)
            return;

        //아니면 0프레임 이벤트 제외하고 실행하기 위해  과거프레임 조금 증가
        else
            prevFrame += 0.001;
    }





    // 캐릭터 인덱스 탐색
 
     auto characterIt = m_pFrameEvent->find(m_iTestModelIndex);   //테스트중
    if (characterIt != m_pFrameEvent->end())
    {
        auto& animationMap = characterIt->second;
        auto animationIt = animationMap.find(animationIndex);
        if (animationIt != animationMap.end())
        {
            auto& frameMap = animationIt->second;


            //0일때를 따로 만들었으니 제외해야하나?
            for (auto frameIt = frameMap.lower_bound(prevFrame); frameIt != frameMap.end() && frameIt->first <= currentFrame; ++frameIt)
            {
                // 해당 프레임에서의 이벤트 리스트를 출력
                for (string event : frameIt->second)
                {
                    //MSG_BOX(TEXT("프레임 " + frameIt->first + "에서 이벤트 발생: " + event));

                    _bool bdebug1 = false;

                    //m_pSelectedModelCom->m_Animations[m_pSelectedModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 100.f;
                    //현재 25에서 돌아오지 않는 문제가 있음...?


                    CFrameEvent_Manager::Get_Instance()->UseEvent_Test(event, m_pSelectedObject);

                }
            }

        }
    }

}




/*
void CIMGUI_Animation_Tab::ProcessEventsBetweenFrames(int characterIndex, int animationIndex, float prevFrame, float currentFrame)
{



    //Frame이 멈춰서 중복처리되는경우는?
    // 프레임이 0인경우를 제외하고 프레임이 멈췄는데 정확하게 이벤트가 있을 수 있나?
        // 0.001초단위로 보면 불가능한건 아님

    //해결방법
    // 1. 이 함수를 play animation 주변에 넣어서, 애니메이션이 정지한경우 이 함수 호출 자체를 안함.  << 정석
    // 2. bool값을 만들어서 1회만 처리
    
    //일단 보류.






    //frame이 0인경우는 애니메이션 실행 말고, 애니메이션 변경시 0의 이벤트만 찾아서 하는걸로 처리.   (ProcessEventsFramesZero)
    if (prevFrame == 0)
    {
        
        //과거,현재 프레임이 모두 0으로 애니메이션 자체가 정지된 경우 함수 끝
        if (currentFrame == 0)
            return;

        //아니면 0프레임 이벤트 제외하고 실행하기 위해  과거프레임 조금 증가
        else
            prevFrame += 0.001;
    }


    


    // 캐릭터 인덱스 탐색

    auto characterIt = FrameEvent.find(SELECT_HIT);   //테스트중
    if (characterIt != FrameEvent.end()) 
    {
        auto& animationMap = characterIt->second;
        auto animationIt = animationMap.find(animationIndex);
        if (animationIt != animationMap.end()) 
        {
            auto& frameMap = animationIt->second;


            //0일때를 따로 만들었으니 제외해야하나?
            for (auto frameIt = frameMap.lower_bound(prevFrame); frameIt != frameMap.end() && frameIt->first <= currentFrame; ++frameIt) 
            {
                // 해당 프레임에서의 이벤트 리스트를 출력
                for (string event : frameIt->second) 
                {
                    //MSG_BOX(TEXT("프레임 " + frameIt->first + "에서 이벤트 발생: " + event));

                    _bool bdebug1 = false;

                    //m_pSelectedModelCom->m_Animations[m_pSelectedModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 100.f;
                    //현재 25에서 돌아오지 않는 문제가 있음...?



                }
            }

        }
    }
}
*/


void CIMGUI_Animation_Tab::ProcessEventsFramesZero(int characterIndex, int animationIndex)
{
    //애니메이션 변경시 0번째 프레임이벤트를 실행하기 위함

    auto characterIt = m_pFrameEvent->find(m_iTestModelIndex);   //테스트중
    if (characterIt != m_pFrameEvent->end())
    {
        auto& animationMap = characterIt->second;
        auto animationIt = animationMap.find(animationIndex);
        if (animationIt != animationMap.end())
        {
            auto& frameMap = animationIt->second;

            auto frameIt = frameMap[0];

            for (auto event : frameIt)
            {

                CFrameEvent_Manager::Get_Instance()->UseEvent_Test(event, m_pSelectedObject);
            }
            
        }
    }

}


void CIMGUI_Animation_Tab::SaveEventTXT(const _char* TextFilePath, string strNeweventText)
{
    ifstream file(TextFilePath);

    // 파일을 제대로 열었는지 확인
    if (!file.is_open())
    {

        MSG_BOX(TEXT("CIMGUI_Animation_Tab:: SaveEventTXT 에서 txt 파일 로드 실패\nPath:"));
        return;
    }



    // 파일 내용 읽기
    string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // 새로운 이벤트 텍스트 추가
    fileContent += "\n" + strNeweventText;

    // 파일을 쓰기 모드로 다시 열기 (덮어쓰기)
    std::ofstream outFile(TextFilePath);

    if (!outFile.is_open())
    {
        MSG_BOX(TEXT("CIMGUI_Animation_Tab:: SaveEventTXT 에서 txt 파일 저장 실패\nPath:"));
        return;
    }

    // 수정된 파일 내용을 파일에 다시 저장
    outFile << fileContent;
    outFile.close();

    MSG_BOX(TEXT("이벤트가 성공적으로 추가되었습니다."));


}

string CIMGUI_Animation_Tab::Get_strAnimationEvent()
{
    string strEventTEXT{};

    //소수점 6자리까지 저장함
   strEventTEXT += Get_CharacterName();
   strEventTEXT += "," + g_iCurrentAnimationIndex;
   strEventTEXT += "," + Get_AnimationName();// to_string(m_pSelectedModelCom->m_iCurrentAnimationIndex);
   strEventTEXT += "," + to_string(m_pSelectedModelCom->m_fCurrentAnimPosition);
   strEventTEXT += ",AnimSpeedChange,";
   strEventTEXT += to_string(m_fAnimationUpdateTime);
   strEventTEXT += ",";
   strEventTEXT += to_string(m_fTool_TickPerSecond);




    return strEventTEXT;
}

string CIMGUI_Animation_Tab::Get_CharacterName()
{

    string strCharacterTEXT;

    switch (m_iTestModelIndex)
    {
    case Client::SELECT_GOKU:
        strCharacterTEXT += "SELECT_GOKU";
        break;

    case Client::SELECT_FRN:
        strCharacterTEXT += "SELECT_FRN";
        break;

    case Client::SELECT_21:
        strCharacterTEXT += "SELECT_21";
        break;

    case Client::SELECT_HIT:
        strCharacterTEXT += "SELECT_HIT";
        break;

    case Client::PLAY_GOKU:
        strCharacterTEXT += "PLAY_GOKU";
        break;

    case Client::PLAY_FRN:
        strCharacterTEXT += "PLAY_FRN";
        break;

    case Client::PLAY_21:
        strCharacterTEXT += "PLAY_21";
        break;

    case Client::PLAY_HIT:
        strCharacterTEXT += "PLAY_HIT";
        break;

    case Client::CHARACTER_INDEX_END:
    default:
        MSG_BOX(TEXT("CIMGUI_Animation_Tab::Get_strAnimationEvent() m_iTestModelIndex 에러"));
        break;
    }

    return strCharacterTEXT;
}

string CIMGUI_Animation_Tab::Get_AnimationName()
{
    
    std::vector<const char*> animNames;

    CAnimation*  AnimData = m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex];

    string strFullName = AnimData->m_szName;
    string AnimName{};

    size_t pos = strFullName.find_last_of('|');

    // 마지막 '|'가 존재하면 그 뒤의 부분만 추출
    if (pos != std::string::npos) 
    {
        AnimName = strFullName.substr(pos + 1);
    }
    else 
    {
        AnimName = strFullName;
    }

    return AnimName;

}

void CIMGUI_Animation_Tab::ExportAllAnimationToTXT()
{

    std::ofstream outFile("../Bin/FrameEventData/ExportAnimation.txt");

    if (!outFile) {
        std::cerr << "파일을 열 수 없습니다!" << std::endl;
        return;
    }


    _int iAnimationIndex = 0;

    //적용 후
    for (auto AnimData : m_pSelectedModelCom->m_Animations)
    {

        string strFullName = AnimData->m_szName;
        string AnimName{};
       
        size_t pos = strFullName.find_last_of('|');

        // 마지막 '|'가 존재하면 그 뒤의 부분만 추출
        if (pos != std::string::npos)
        {
            AnimName = strFullName.substr(pos + 1);
        }
        else
        {
            AnimName = strFullName;
        }


        string FinalText = "[\"" + AnimName +"\"] = ";
        FinalText += to_string(iAnimationIndex);

        outFile << FinalText << std::endl;  // 파일에 한 줄씩 기록

        iAnimationIndex++;
    }

    // 파일 닫기
    outFile.close();

    MSG_BOX(TEXT("추출성공"));

}




CIMGUI_Animation_Tab* CIMGUI_Animation_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_Animation_Tab* pInstance = new CIMGUI_Animation_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Animation_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_Animation_Tab::Free()
{
	__super::Free();

    if (m_pSelectedModelCom != nullptr)
    {
        Safe_Release(m_pSelectedModelCom);
    }
}

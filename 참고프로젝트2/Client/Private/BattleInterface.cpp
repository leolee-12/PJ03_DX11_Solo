
#include "stdafx.h"
#include "..\Public\BattleInterface.h"

#include "RenderInstance.h"
#include "GameInstance.h"


#include <fstream>  
#include <iostream> 
#include <vector>   
#include <string>   
#include <sstream>

#include "Animation.h"
#include "Character.h"

#include "Model_Preview.h"
#include "BattleInterface.h"


#include "Main_Camera.h"
#include "AttackObject.h"

#include "UI_Manager.h"
#include "Map_Manager.h"

#include "Opening_Kririn.h"

IMPLEMENT_SINGLETON(CBattleInterface_Manager)




CBattleInterface_Manager::CBattleInterface_Manager()
    :m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
}




void CBattleInterface_Manager::Gain_HitCount(_ushort iHitCount, _ushort iTeam)
{
    m_iHitCount[iTeam - 1] += iHitCount;
}

void CBattleInterface_Manager::Reset_HitCount(_ushort iTeam)
{
    m_iHitCount[iTeam - 1] = 0;

}

_ushort CBattleInterface_Manager::Get_HitCount(_ushort iTeam)
{
    return  m_iHitCount[iTeam - 1];
}

_bool CBattleInterface_Manager::Get_bSparkingEnable(_ushort iTeam)
{

    if (m_pGameInstance->Key_Pressing(DIK_HOME))
        return true;

    return  m_bSparkingEnable[iTeam - 1];
}

void CBattleInterface_Manager::Set_bSparkingEnable(_bool bSparkingEnable, _ushort iTeam)
{
    m_bSparkingEnable[iTeam - 1] = bSparkingEnable;
}

void CBattleInterface_Manager::Gain_KiGuage(_ushort iKi, _ushort iTeam)
{

    iKi *= 2;

    //기 게이지 꽉찬게 아니면
    if (m_iKiNumber[iTeam - 1] != 7)
    {
        //기 를 회복 
        m_iKiGuage[iTeam - 1] += iKi;

        //1줄 넘겼으면 다음줄로
        if (m_iKiGuage[iTeam - 1] >= 100)
        {
            m_iKiNumber[iTeam - 1]++;
            m_iKiGuage[iTeam - 1] -= 100;

            //줄 넘어간거 안보이면 위화감 드니 조금 올림
            if (m_iKiGuage[iTeam - 1] < 5)
            {
                m_iKiGuage[iTeam - 1] += 5;
            }
        }
    }




}

_bool CBattleInterface_Manager::Use_KiGuage(_ushort irequirementKi, _ushort iTeam)
{

    if (m_iKiNumber[iTeam - 1] >= irequirementKi)
    {
        m_iKiNumber[iTeam - 1] -= irequirementKi;
        return true;
    }

    return false;
}

_bool CBattleInterface_Manager::Use_KiRealGuage(_ushort irequirementKi, _ushort iTeam)
{
    if (m_iKiGuage[iTeam - 1] >= irequirementKi)
    {
        m_iKiGuage[iTeam - 1] -= irequirementKi;
        return true;
    }

    return false;
}

_ushort CBattleInterface_Manager::Get_KiGuage(_ushort iTeam)
{
    return  m_iKiGuage[iTeam - 1];
}

_ushort CBattleInterface_Manager::Get_KiNumber(_ushort iTeam)
{
    return  m_iKiNumber[iTeam - 1];
}

void CBattleInterface_Manager::Stop_CharacterWithoutMe(_ushort iTeam, _ubyte iSlot, _float fTime)
{


    for (auto pCharacter : m_p1TeamCharacter)
    {
        if (pCharacter != nullptr)
            pCharacter->Set_AnimationStop(fTime);
    }

    for (auto pCharacter : m_p2TeamCharacter)
    {
        if (pCharacter != nullptr)
            pCharacter->Set_AnimationStop(fTime);
    }


    if (iTeam == 1)
    {
        m_p1TeamCharacter[iSlot]->Set_UnlockAnimationStop();
    }
    else if (iTeam == 2)
    {
        m_p2TeamCharacter[iSlot]->Set_UnlockAnimationStop();
    }

}

void CBattleInterface_Manager::Stop_AllCharacter(_float fStopTime)
{
    for (auto pCharacter : m_p1TeamCharacter)
    {
        if (pCharacter != nullptr)
            pCharacter->Set_AnimationStop(fStopTime);
    }

    for (auto pCharacter : m_p2TeamCharacter)
    {
        if (pCharacter != nullptr)
            pCharacter->Set_AnimationStop(fStopTime);
    }
}

_ushort CBattleInterface_Manager::Get_iAliveMemberCount(_ushort iTeam)
{

    _ushort iAliveMebberCount = 0;

    if (iTeam == 1)
    {
        for (auto pCharacter : m_p1TeamCharacter)
        {
            if (pCharacter != nullptr)
            {
                if (pCharacter->Get_bDying() == false)
                    iAliveMebberCount++;
            }
        }
    }
    else if (iTeam == 2)
    {
        for (auto pCharacter : m_p2TeamCharacter)
        {
            if (pCharacter != nullptr)
            {
                if (pCharacter->Get_bDying() == false)
                    iAliveMebberCount++;
            }
        }
    }

    return iAliveMebberCount;

}

void CBattleInterface_Manager::Stop_AllAttackObject(_float fStopTime)
{

    for (auto pAttackObject : m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AttackObject")))
    {
        static_cast<CAttackObject*>(pAttackObject)->Set_UpdateStop(fStopTime);
    }

}

void CBattleInterface_Manager::SpecialKiGain(_ushort iKiNumber, _ushort iTeam)
{
    m_iKiNumber[iTeam - 1] = iKiNumber;
}



/*
void CBattleInterface_Manager::Tag_CharacterAIO(_ubyte iTeam, _ubyte NewCharacterslot)
{

    if (iTeam == 1)
    {
        for (auto pCharcter : m_p2TeamCharacter)
        {
            pCharcter->RegisterEnemy(m_p1TeamCharacter[NewCharacterslot]);
        }

        m_p1TeamCharacter[NewCharacterslot]->Tag_Out()
    }
    else if (iTeam == 2)
    {
        for (auto pCharcter : m_p1TeamCharacter)
        {
            pCharcter->RegisterEnemy(m_p2TeamCharacter[NewCharacterslot]);
        }
    }



}
*/

_bool CBattleInterface_Manager::Tag_CharacterAIO(_ubyte iTeam, _ubyte NewCharacterslot, _vector vPos)
{


    CMain_Camera* pMainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));


    if (iTeam == 1)
    {
        //나올 캐릭터가 죽어있으면 바로 끝
        if (m_p1TeamCharacter[NewCharacterslot]->Get_bDying())
            return false;


        for (auto pCharacter : m_p2TeamCharacter)
        {
            if (pCharacter != nullptr)
                pCharacter->RegisterEnemy(m_p1TeamCharacter[NewCharacterslot]);
        }
        m_i1TeamPlayingCharacterIndex = NewCharacterslot;
        m_p1TeamCharacter[NewCharacterslot]->Tag_Out(vPos);
        pMainCamera->Set_Player(m_p1TeamCharacter[m_i1TeamPlayingCharacterIndex]);
    }
    else if (iTeam == 2)
    {

        //나올 캐릭터가 죽어있으면 바로 끝
        if (m_p2TeamCharacter[NewCharacterslot]->Get_bDying())
            return false;

        for (auto pCharacter : m_p1TeamCharacter)
        {
            if (pCharacter != nullptr)
                pCharacter->RegisterEnemy(m_p2TeamCharacter[NewCharacterslot]);
        }
        m_i2TeamPlayingCharacterIndex = NewCharacterslot;
        m_p2TeamCharacter[NewCharacterslot]->Tag_Out(vPos);
        pMainCamera->Set_Player(m_p2TeamCharacter[m_i2TeamPlayingCharacterIndex]);
    }

    return true;

}

void CBattleInterface_Manager::Regist_Character(_ubyte iTeam, class CCharacter* pCharacter, _ubyte iSlot)
{
    if (iTeam == 1)
    {
        m_p1TeamCharacter[iSlot] = pCharacter;
    }
    else if (iTeam == 2)
    {
        m_p2TeamCharacter[iSlot - 2] = pCharacter;
    }


}

void CBattleInterface_Manager::Set_Character_pEnemy(_ubyte iTeam, _ubyte NewCharacterslot)
{

    if (iTeam == 1)
    {

        for (auto pCharacter : m_p2TeamCharacter)
        {
            if (pCharacter != nullptr)
                pCharacter->RegisterEnemy(m_p1TeamCharacter[NewCharacterslot]);
        }
        m_i1TeamPlayingCharacterIndex = NewCharacterslot;

    }

    else if (iTeam == 2)
    {
        for (auto pCharacter : m_p1TeamCharacter)
        {
            if (pCharacter != nullptr)
                pCharacter->RegisterEnemy(m_p2TeamCharacter[NewCharacterslot]);
        }
        m_i2TeamPlayingCharacterIndex = NewCharacterslot;

    }


}




CCharacter* CBattleInterface_Manager::EnemyInitalize(_ubyte iTeam)
{
    if (iTeam == 1)
    {
        return m_p2TeamCharacter[0];
    }
    else if (iTeam == 2)
    {
        return m_p1TeamCharacter[0];
    }
}

_bool CBattleInterface_Manager::Check_NextRoundFromDeathCharacter(_ubyte iTeam, _ubyte NewCharacterslot)
{

    if (m_b1vs1)
    {
        //승리
       // CUI_Manager::Get_Instance()->WinUI(LEVEL_GAMEPLAY);
         
        return true;
    }

    m_p1TeamCharacter[NewCharacterslot]->Set_AnimationStop(0.f);
    m_p1TeamCharacter[NewCharacterslot]->Set_AnimationStopWithoutMe(0.f);

    CUI_Manager::Get_Instance()->CutSceneUI(true);
    //사망한 팀의 다음캐릭터가 살아있는지 체크
    if (iTeam == 1)
    {
        m_p1TeamCharacter[m_i1TeamPlayingCharacterIndex]->Add_Move({ 0.f,200.f });
        m_p1TeamCharacter[m_i1TeamPlayingCharacterIndex]->Update_Collider();
        m_p1TeamCharacter[m_i1TeamPlayingCharacterIndex]->Set_bPlaying(false);


        //나올 캐릭터가 없으면 패배
        if (m_p1TeamCharacter[NewCharacterslot]->Get_bDying())
        {

            //2Team 현재 캐릭터 승리연출
            m_p2TeamCharacter[m_i2TeamPlayingCharacterIndex]->Play_WinAnimation();


        }

        //다음 캐릭터가 있으면 연출 + 다음라운드
        else
        {

            //이긴 팀에 캐릭터 더 있으면 맵 바꾸기
            if (m_i2TeamPlayingCharacterIndex == 0)
            {
                //살아있으면 맵바꾸고  캐릭터 변경
                if (m_p2TeamCharacter[1]->Get_bDying() == false)
                {
                    //맵변경
                    if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_SPACE)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_VOLCANO);

                    }
                    else  if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_VOLCANO)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_SPACE);

                    }

                    //진팀에서 달려오기
                    Set_Character_pEnemy(1, NewCharacterslot);
                    m_p1TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p1TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i1TeamPlayingCharacterIndex = NewCharacterslot;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));


                    //승자 새 캐릭터 달려오기
                    Set_Character_pEnemy(2, 1);
                    m_p2TeamCharacter[1]->Play_NewRound_Winner();
                    m_p2TeamCharacter[1]->Set_bPlaying(true);
                    m_p2TeamCharacter[0]->Set_bPlaying(false);
                    m_p2TeamCharacter[0]->Add_Move({ 200.f,100.f });
                    m_p2TeamCharacter[0]->Update_Collider();
                    m_p2TeamCharacter[0]->Set_bDynamicMove(false);
                    m_i2TeamPlayingCharacterIndex = 1;
                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));



                }

                // 1:1 된거면 맵 변경 하지 않고 맵 유지
                else //   if (m_p2TeamCharacter[1]->Get_bDying() == true) , m_i2TeamPlayingCharacterIndex == 0
                {
                    //진팀에서 달려오기
                    Set_Character_pEnemy(1, NewCharacterslot);
                    m_p1TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p1TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i1TeamPlayingCharacterIndex = NewCharacterslot;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));

                    //승자 기존 캐릭터 받아치기
                    Set_Character_pEnemy(2, 0);
                    m_p2TeamCharacter[0]->Play_NewRound_Winner();
                    m_p2TeamCharacter[0]->Set_bPlaying(true);


                    m_i2TeamPlayingCharacterIndex = 1;
                    //CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));
                }
            }
            else // m_i2TeamPlayingCharacterIndex == 1
            {

                //살아있으면 맵바꾸고  캐릭터 변경
                if (m_p2TeamCharacter[0]->Get_bDying() == false)
                {
                    //맵변경
                    if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_SPACE)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_VOLCANO);

                    }
                    else  if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_VOLCANO)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_SPACE);

                    }

                    //진팀에서 달려오기
                    Set_Character_pEnemy(1, NewCharacterslot);
                    m_p1TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p1TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i1TeamPlayingCharacterIndex = NewCharacterslot;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));


                    //승자 새 캐릭터 달려오기
                    Set_Character_pEnemy(2, 0);
                    m_p2TeamCharacter[0]->Play_NewRound_Winner();
                    m_p2TeamCharacter[0]->Set_bPlaying(true);
                    m_p2TeamCharacter[1]->Set_bPlaying(false);
                    m_p2TeamCharacter[1]->Add_Move({ 200.f,100.f });
                    m_p2TeamCharacter[1]->Update_Collider();
                    m_p2TeamCharacter[1]->Set_bDynamicMove(false);

                    m_i2TeamPlayingCharacterIndex = 0;
                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));



                }

                // 1:1 된거면 맵 변경 하지 않고 맵 유지
                else //   if (m_p2TeamCharacter[0]->Get_bDying() == true) , m_i2TeamPlayingCharacterIndex == 1
                {
                    //진팀에서 달려오기
                    Set_Character_pEnemy(1, NewCharacterslot);
                    m_p1TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p1TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i1TeamPlayingCharacterIndex = NewCharacterslot;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));

                    //승자 기존 캐릭터 받아치기
                    Set_Character_pEnemy(2, 1);
                    m_p2TeamCharacter[1]->Play_NewRound_Winner();
                    m_p2TeamCharacter[1]->Set_bPlaying(true);


                    m_i2TeamPlayingCharacterIndex = 1;
                    // CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));
                }

            }



        }
    }

    else if (iTeam == 2)
    {
        m_p2TeamCharacter[m_i1TeamPlayingCharacterIndex]->Add_Move({ 100.f,200.f });
        m_p2TeamCharacter[m_i1TeamPlayingCharacterIndex]->Update_Collider();
        m_p2TeamCharacter[m_i1TeamPlayingCharacterIndex]->Set_bPlaying(false);


        //나올 캐릭터가 없으면 패배
        if (m_p2TeamCharacter[NewCharacterslot]->Get_bDying())
        {

            //1Team 현재 캐릭터 승리연출
            m_p1TeamCharacter[m_i1TeamPlayingCharacterIndex]->Play_WinAnimation();


        }

        //다음 캐릭터가 있으면 연출 + 다음라운드
        else
        {

            //이긴 팀에 캐릭터 더 있으면 맵 바꾸기
            if (m_i1TeamPlayingCharacterIndex == 0)
            {
                //살아있으면 맵바꾸고  캐릭터 변경
                if (m_p1TeamCharacter[1]->Get_bDying() == false)
                {
                    //맵변경
                    if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_SPACE)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_VOLCANO);

                    }
                    else  if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_VOLCANO)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_SPACE);

                    }

                    //진팀에서 달려오기
                    Set_Character_pEnemy(2, NewCharacterslot);
                    m_p2TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p2TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i2TeamPlayingCharacterIndex = NewCharacterslot;
                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));



                    //승자 새 캐릭터 달려오기
                    Set_Character_pEnemy(1, 1);
                    m_p1TeamCharacter[1]->Play_NewRound_Winner();
                    m_p1TeamCharacter[1]->Set_bPlaying(true);
                    m_p1TeamCharacter[0]->Set_bPlaying(false);
                    m_p1TeamCharacter[0]->Add_Move({ 100.f,100.f });
                    m_p1TeamCharacter[0]->Update_Collider();
                    m_p1TeamCharacter[0]->Set_bDynamicMove(false);

                    m_i1TeamPlayingCharacterIndex = 1;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));


                }

                // 1:1 된거면 맵 변경 하지 않고 맵 유지
                else //   if (m_p1TeamCharacter[1]->Get_bDying() == true) , m_i1TeamPlayingCharacterIndex == 0
                {
                    //진팀에서 달려오기
                    Set_Character_pEnemy(2, NewCharacterslot);
                    m_p2TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p2TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i2TeamPlayingCharacterIndex = NewCharacterslot;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));

                    //승자 기존 캐릭터 받아치기
                    Set_Character_pEnemy(1, 0);
                    m_p1TeamCharacter[0]->Play_NewRound_Winner();
                    m_p1TeamCharacter[0]->Set_bPlaying(true);


                    m_i1TeamPlayingCharacterIndex = 1;
                    //CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));
                }
            }
            else // m_i1TeamPlayingCharacterIndex == 1
            {

                //살아있으면 맵바꾸고  캐릭터 변경
                if (m_p1TeamCharacter[0]->Get_bDying() == false)
                {
                    //맵변경
                    if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_SPACE)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_VOLCANO);

                    }
                    else  if (CMap_Manager::Get_Instance()->m_eCurMap == CMap_Manager::MAP_VOLCANO)
                    {
                        CMap_Manager::Get_Instance()->Map_Change(CMap_Manager::MAP_SPACE);

                    }

                    //진팀에서 달려오기
                    Set_Character_pEnemy(2, NewCharacterslot);
                    m_p2TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p2TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i2TeamPlayingCharacterIndex = NewCharacterslot;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));


                    //승자 새 캐릭터 달려오기
                    Set_Character_pEnemy(1, 0);
                    m_p1TeamCharacter[0]->Play_NewRound_Winner();
                    m_p1TeamCharacter[0]->Set_bPlaying(true);
                    m_p1TeamCharacter[1]->Set_bPlaying(false);
                    m_p1TeamCharacter[1]->Add_Move({ 300.f,100.f });
                    m_p1TeamCharacter[1]->Update_Collider();
                    m_p1TeamCharacter[1]->Set_bDynamicMove(false);
                    m_i1TeamPlayingCharacterIndex = 0;

                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(0));


                }

                // 1:1 된거면 맵 변경 하지 않고 맵 유지
                else //   if (m_p1TeamCharacter[0]->Get_bDying() == true) , m_i1TeamPlayingCharacterIndex == 1
                {
                    //진팀에서 달려오기
                    Set_Character_pEnemy(2, NewCharacterslot);
                    m_p2TeamCharacter[NewCharacterslot]->Play_NewRound_Loser();
                    m_p2TeamCharacter[NewCharacterslot]->Set_bPlaying(true);


                    m_i2TeamPlayingCharacterIndex = NewCharacterslot;
                    CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));


                    //승자 기존 캐릭터 받아치기
                    Set_Character_pEnemy(1, 1);
                    m_p1TeamCharacter[1]->Play_NewRound_Winner();
                    m_p1TeamCharacter[1]->Set_bPlaying(true);


                    m_i2TeamPlayingCharacterIndex = 1;
                    // CUI_Manager::Get_Instance()->UsingChangeCharacher(static_cast<CUI_Define::PLAYER_SLOT>(3));
                }

            }



        }
    }

    return false;

}

void CBattleInterface_Manager::Set_InvisibleWithoutMe(_ubyte iTeam, _ubyte iCharacterslot)
{

    //iCharacterslot 안씀

    for (auto pCharacter : m_p1TeamCharacter)
    {
        if (pCharacter != nullptr)
            pCharacter->Set_bInivisible(true);
    }
    for (auto pCharacter : m_p2TeamCharacter)
    {
        if (pCharacter != nullptr)
            pCharacter->Set_bInivisible(true);
    }


    if (iTeam == 1)
    {
        m_p1TeamCharacter[iCharacterslot]->Set_bInivisible(false);
    }
    else
    {
        m_p2TeamCharacter[iCharacterslot]->Set_bInivisible(false);
    }


}

void CBattleInterface_Manager::Character_Opening_AIO()
{

    CUI_Manager::Get_Instance()->CutSceneUI(false);

    CRenderInstance::Get_Instance()->Switch_AllBlackOut();

    //오공 vs 프리저 면 다른 모션으로 시작
    if (m_p1TeamCharacter[0]->Get_eCharacterIndex() == CHARACTER_INDEX::PLAY_GOKU && m_p2TeamCharacter[0]->Get_eCharacterIndex() == CHARACTER_INDEX::PLAY_FRN)
    {
        m_p1TeamCharacter[0]->Set_Animation(86); //오공 시네마틱 오프닝
        m_p2TeamCharacter[0]->Set_Animation(86); //프리저 시네마틱 오프닝


        m_p1TeamCharacter[0]->FlipDirection(1);
        m_p2TeamCharacter[0]->FlipDirection(1);

        m_p1TeamCharacter[0]->Set_AnimationStop(1.f);
        m_p2TeamCharacter[0]->Set_AnimationStop(1.f);

        m_p1TeamCharacter[0]->Set_bDynamicMove(true);
        m_p2TeamCharacter[0]->Set_bDynamicMove(true);

        m_p1TeamCharacter[0]->Set_bGrabbed(true);
        m_p2TeamCharacter[0]->Set_bGrabbed(true);

        m_bCinematicOpening = true;

        m_p1TeamCharacter[0]->Set_AnimationMoveXZ(true);
        m_p2TeamCharacter[0]->Set_AnimationMoveXZ(true);


        m_p1TeamCharacter[0]->Set_bAura(false);



        static_cast<COpening_Kririn*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")))->Set_bInivisible(false);


        static_cast<COpening_Kririn*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")))->Set_CurrentAnimationPositionJump(0.f);



        CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
        mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_GOKU_VS_FRIEZA_ENTRY, 0, m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character")), nullptr, true);


    }
    else
    {

        m_p1TeamCharacter[0]->Play_FirstOpening();
        m_p2TeamCharacter[0]->Set_bInivisible(true);

        //static_cast<COpening_Kririn*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")))->Set_Delete();
        static_cast<COpening_Kririn*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Opening")))->Destory();


    }
    //UI 안보이게뒀다가 나중에 켜기



}

void CBattleInterface_Manager::Character_Opening_EndForCharacter(_ubyte iTeam)
{

    if (m_bCinematicOpening)
    {
        //오프닝 UI, Inivisible 종료,
        m_p1TeamCharacter[0]->Set_bDynamicMove(false);
        m_p2TeamCharacter[0]->Set_bDynamicMove(false);

        m_p1TeamCharacter[0]->Set_bGrabbed(false);
        m_p2TeamCharacter[0]->Set_bGrabbed(false);

        m_p1TeamCharacter[0]->Set_IdleAnimation();
        m_p2TeamCharacter[0]->Set_IdleAnimation();

        m_p1TeamCharacter[0]->Character_Play_Animation(0.1f);
        m_p2TeamCharacter[0]->Character_Play_Animation(0.1f);

        //좌표도 바꾸나?
        m_p1TeamCharacter[0]->Set_AnimationStop(1.f);
        m_p2TeamCharacter[0]->Set_AnimationStop(1.f);

        m_p1TeamCharacter[0]->Set_bInivisible(false);
        m_p2TeamCharacter[0]->Set_bInivisible(false);


        m_p1TeamCharacter[0]->FlipDirection(1);
        m_p2TeamCharacter[0]->FlipDirection(-1);


        m_p1TeamCharacter[0]->Set_AnimationMoveXZ(false);
        m_p2TeamCharacter[0]->Set_AnimationMoveXZ(false);

        m_p1TeamCharacter[0]->Character_CinematicEnd();
        m_p2TeamCharacter[0]->Character_CinematicEnd();


        //원래는 오공 추가컷씬
        CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
        mainCamera->Set_Virtual_Camera(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_NORMAL);

        CUI_Manager::Get_Instance()->CutSceneUI(true);

        CUI_Manager::Get_Instance()->UsingCreateStartUI();

        return;
    }

    if (iTeam == 1)
    {

      
        m_p1TeamCharacter[0]->Set_bInivisible(true);
        m_p2TeamCharacter[0]->Play_FirstOpening();

    }
    else if (iTeam == 2)
    {


        //오프닝 UI, Inivisible 종료,
        m_p1TeamCharacter[0]->Set_bDynamicMove(false);
        m_p2TeamCharacter[0]->Set_bDynamicMove(false);

        m_p1TeamCharacter[0]->Set_bGrabbed(false);
        m_p2TeamCharacter[0]->Set_bGrabbed(false);

        m_p1TeamCharacter[0]->Set_IdleAnimation();
        m_p2TeamCharacter[0]->Set_IdleAnimation();

        m_p1TeamCharacter[0]->Character_Play_Animation(0.1f);
        m_p2TeamCharacter[0]->Character_Play_Animation(0.1f);

        //좌표도 바꾸나?
        m_p1TeamCharacter[0]->Set_AnimationStop(1.f);
        m_p2TeamCharacter[0]->Set_AnimationStop(1.f);

        m_p1TeamCharacter[0]->Set_bInivisible(false);
        m_p2TeamCharacter[0]->Set_bInivisible(false);

        CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
        mainCamera->Set_Virtual_Camera(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_NORMAL);


        CUI_Manager::Get_Instance()->CutSceneUI(true);
        CUI_Manager::Get_Instance()->UsingCreateStartUI();

    }
}

void CBattleInterface_Manager::Set_b1VS1(_bool b1vs1)
{
    m_b1vs1 = b1vs1;
}

bool CBattleInterface_Manager::Check_survivor(_ubyte iTeam, _ubyte NewCharacterslot)
{
    if(m_b1vs1)        
        return false;

    else
    {
        if (iTeam == 1)
        {
            if (m_p1TeamCharacter[NewCharacterslot]->Get_bDying())
                return false;

        }
        else if (iTeam == 2)
        {
            if (m_p2TeamCharacter[NewCharacterslot]->Get_bDying())
                return false;
        }
    }

    return true;
}


void CBattleInterface_Manager::Set_CharaDesc(_uint iIndex, _ushort iTeam, CUI_Define::PLAYER_SLOT eSlot, wstring PrototypeTag, CUI_Define::PLAYER_ID ePlayerID)
{
    m_tCharaCreateDesc[iIndex] = { iTeam  , eSlot  , PrototypeTag,ePlayerID };
}



void CBattleInterface_Manager::Gain_HitAttackStep(_ushort iHitCount, _ushort iTeam)
{
    m_iHitAttackStep[iTeam - 1] += iHitCount;
}

void CBattleInterface_Manager::Reset_HitAttackStep(_ushort iTeam)
{
    m_iHitAttackStep[iTeam - 1] = 0;
}

_ushort CBattleInterface_Manager::Get_HitAttackStep(_ushort iTeam)
{
    return m_iHitAttackStep[iTeam - 1];
}

void CBattleInterface_Manager::Free()
{
    __super::Free();
}


#pragma once
#include "Game_PKM_Defines.h"
#include "Event_Defines.h"
#include "Interaction.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Game_PKM)

class CLevel_GamePlay;

struct EVENT_CONTEXT
{
    CGameInstance* pGameInstance = { nullptr };
    CLevel_GamePlay* pLevelGamePlay = { nullptr };

    CGameObject* pCaller = { nullptr };
    CGameObject* pTarget = { nullptr };
    INTERACTION_EVENT eInteractionEvent = { INTERACTION_EVENT::TALK };

    INPUT_STATE ePrevInputState = { INPUT_STATE::GAMEPLAY };
    _bool bInputLockedByEvent = { false };

    CAMERA_EVENT_SNAPSHOT tCameraSnapshot = {};

    unordered_map<_wstring, CGameObject*> ActorAliases;

    CGameObject* Find_Actor(const _wstring& strAlias) const
    {
        if (L"Caller" == strAlias || L"$caller" == strAlias)
            return pCaller;

        if (L"Target" == strAlias || L"$target" == strAlias)
            return pTarget;

        auto iter = ActorAliases.find(strAlias);
        if (iter == ActorAliases.end())
            return nullptr;

        return iter->second;
    }

    void Bind_Actor(const _wstring& strAlias, CGameObject* pActor)
    {
        if (true == strAlias.empty())
            return;

        if (nullptr == pActor)
        {
            ActorAliases.erase(strAlias);
            return;
        }

        ActorAliases[strAlias] = pActor;
    }
};

NS_END
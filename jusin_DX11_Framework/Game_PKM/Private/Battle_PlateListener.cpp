#include "Battle_PlateListener.h"
#include "BattlePlate.h"

CBattle_PlateListener::CBattle_PlateListener()
{
}

HRESULT CBattle_PlateListener::Initialize()
{
    return S_OK;
}

void CBattle_PlateListener::Bind(CBattlePlate* pBattlePlate)
{
    m_pBattlePlate = pBattlePlate;
}

void CBattle_PlateListener::On_PokemonSwitched(const EVENT_POKEMON_SWITCHED& tEvent)
{
    (void)tEvent;

    if (nullptr == m_pBattlePlate)
        return;

    m_pBattlePlate->Snap_HPDisplay();
}

CBattle_PlateListener* CBattle_PlateListener::Create()
{
    CBattle_PlateListener* pInstance = new CBattle_PlateListener();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CBattle_PlateListener");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBattle_PlateListener::Free()
{
    m_pBattlePlate = nullptr;

    __super::Free();
}
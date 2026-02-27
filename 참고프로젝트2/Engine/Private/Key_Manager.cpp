
#include "..\Public\Key_Manager.h"
//#include "Key_Manager.h"

CKey_Manager::CKey_Manager()
{
}

HRESULT CKey_Manager::Initialize()
{

    ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
    for (int i = 0; i < VK_MAX; i++)
    {
        m_bKeyState[i] = false;
    }

    return S_OK;
}

_bool CKey_Manager::Key_Down(_uint _iKey)
{
    if (!m_bKeyState[_iKey] && (GetAsyncKeyState(_iKey)))
        return true;


    else
        return false;
}

_bool CKey_Manager::Key_Pressing(_uint _iKey)
{
    if (GetAsyncKeyState(_iKey) & 0x8000)
        return true;

    return false;
}

_bool CKey_Manager::Key_Up(_uint _iKey)
{
    if (m_bKeyState[_iKey] && !(GetAsyncKeyState(_iKey)))
    {
        return true;
    }

    return false;
}

void CKey_Manager::Update()
{
    for (int i = 0; i < VK_MAX; i++)
    {
        if (GetAsyncKeyState(i))
        {
            m_bKeyState[i] = true;
        }
        else
            m_bKeyState[i] = false;
    }
}

CKey_Manager* CKey_Manager::Create()
{
    CKey_Manager* pInstance = new CKey_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CKey Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKey_Manager::Free()
{
    __super::Free();

}




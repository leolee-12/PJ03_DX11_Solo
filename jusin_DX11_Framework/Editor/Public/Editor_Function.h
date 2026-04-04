#ifndef Editor_Function_h__
#define Editor_Function_h__
#include <commdlg.h>
#include <shobjidl_core.h>
#include "Transform.h"

NS_BEGIN(Editor)

static _bool Open_FileDialog(_char* pOutPath, _uint iMaxPath, const _char* pFilter)
{
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = pOutPath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = iMaxPath;
    ofn.lpstrFilter = pFilter;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    return GetOpenFileNameA(&ofn);
}

static _bool Open_FolderDialog(_char* pOutPath, _uint iMaxPath)
{
    IFileOpenDialog* pDialog = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
        IID_IFileOpenDialog, (void**)&pDialog)))
        return false;

    DWORD dwFlags = {};
    pDialog->GetOptions(&dwFlags);
    pDialog->SetOptions(dwFlags | FOS_PICKFOLDERS | FOS_NOCHANGEDIR);

    if (FAILED(pDialog->Show(g_hWnd)))
    {
        pDialog->Release();
        return false;
    }

    IShellItem* pItem = nullptr;
    if (FAILED(pDialog->GetResult(&pItem)))
    {
        pDialog->Release();
        return false;
    }

    wchar_t* pszPath = nullptr;
    pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
    WideCharToMultiByte(CP_ACP, 0, pszPath, -1, pOutPath, iMaxPath, nullptr, nullptr);

    CoTaskMemFree(pszPath);
    pItem->Release();
    pDialog->Release();

    return true;
}

static void Set_WorldMatrix(CTransform* pTransform, const _float* pPos, const _float* pRot, const _float* pScale)
{
    float mat[16];
    ImGuizmo::RecomposeMatrixFromComponents(pPos, pRot, pScale, mat);
    _float4x4 m; memcpy(&m, mat, sizeof(float) * 16);
    pTransform->Set_State(STATE::RIGHT,    XMLoadFloat4((_float4*)&m._11));
    pTransform->Set_State(STATE::UP,       XMLoadFloat4((_float4*)&m._21));
    pTransform->Set_State(STATE::LOOK,     XMLoadFloat4((_float4*)&m._31));
    pTransform->Set_State(STATE::POSITION, XMLoadFloat4((_float4*)&m._41));
}

NS_END

#endif // Editor_Function_h__
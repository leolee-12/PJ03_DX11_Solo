#ifndef Editor_Function_h__
#define Editor_Function_h__
#include <commdlg.h>

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
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    return GetOpenFileNameA(&ofn);
}

NS_END

#endif // Editor_Function_h__
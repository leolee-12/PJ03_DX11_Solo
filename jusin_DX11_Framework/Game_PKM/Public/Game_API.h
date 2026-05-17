#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Engine)
class CUISequence;
NS_END

NS_BEGIN(Game_PKM)
class CUIController;

#pragma region 공용 로직
HRESULT Start_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eStartLevelID);
HRESULT Ready_PersistentObjects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
HRESULT Ready_StaticTables();
void Cleanup_StaticTables();
#pragma endregion

#pragma region Resources
HRESULT Ready_Prototypes_For_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
HRESULT Ready_Fonts();
HRESULT Ready_SharedTextures(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
#pragma endregion

#pragma region UI 컨트롤러 Hub 래퍼
HRESULT UI_Register(CUIController* pCtrl, _uint iOwnerLevel);	// AddRef on register
void    UI_Unregister(CUIController* pCtrl);	// Safe_Release on unregister
void    UI_Update_All(_float fTimeDelta);
void    UI_Close_All();							// Hub 전체 정리
void    UI_Cleanup_Level(_uint iOwnerLevel);	// 특정 레벨 소유 컨트롤러만 정리
_bool   UI_Is_AnyOpen();
void    UI_Set_Cursor_Sequence(Engine::CUISequence* pSeq);  // weak 주입. nullptr 로 해제.
void    UI_Cleanup();							// 앱 종료 시 1회 호출 - Hub 파괴
#pragma endregion

NS_END

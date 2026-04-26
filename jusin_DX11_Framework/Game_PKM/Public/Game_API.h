#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

HRESULT Ready_Prototypes_For_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
HRESULT Ready_Prototypes_For_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

HRESULT Ready_Fonts();
HRESULT Ready_SharedTextures();

HRESULT Start_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eStartLevelID);

NS_END

#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

HRESULT Ready_Prototype_For_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

HRESULT Start_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eStartLevelID);

NS_END
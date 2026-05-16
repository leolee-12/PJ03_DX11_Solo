#include "Interaction.h"

CInteraction::CInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CInteraction::CInteraction(const CInteraction& Prototype)
	: CComponent(Prototype)
{
}

void CInteraction::Free()
{
	__super::Free();
}
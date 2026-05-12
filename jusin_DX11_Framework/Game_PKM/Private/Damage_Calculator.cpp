#include "Damage_Calculator.h"
#include "Damage_Modifiers.h"
#include "IDamageModifier.h"

CDamage_Calculator::CDamage_Calculator()
{
}

HRESULT CDamage_Calculator::Initialize()
{
	m_vModifiers.reserve(16);

	IDamageModifier* pModifier = nullptr;

	pModifier = CStatStageModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CTypeChartModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CStabModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CAbilityModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CItemModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CWeatherModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CFieldModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CCritModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	pModifier = CRandomRollModifier::Create();
	if (FAILED(Add_Modifier(pModifier))) { Safe_Release(pModifier); return E_FAIL; }
	Safe_Release(pModifier);

	return S_OK;
}

HRESULT CDamage_Calculator::Add_Modifier(IDamageModifier* pModifier)
{
	if (nullptr == pModifier)
		return E_FAIL;

	Safe_AddRef(pModifier);
	m_vModifiers.push_back(pModifier);

	return S_OK;
}

HRESULT CDamage_Calculator::Insert_Modifier(_uint iIndex, IDamageModifier* pModifier)
{
	if (nullptr == pModifier || iIndex > m_vModifiers.size())
		return E_FAIL;

	Safe_AddRef(pModifier);
	m_vModifiers.insert(m_vModifiers.begin() + iIndex, pModifier);

	return S_OK;
}

void CDamage_Calculator::Clear_Modifiers()
{
	for (auto& pModifier : m_vModifiers)
		Safe_Release(pModifier);

	m_vModifiers.clear();
}

void CDamage_Calculator::Calculate(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe)
{
	for (auto* pModifier : m_vModifiers)
	{
		if (nullptr == pModifier)
			continue;

		pModifier->Apply(ctx, pipe);
	}
}

CDamage_Calculator* CDamage_Calculator::Create()
{
	CDamage_Calculator* pInstance = new CDamage_Calculator();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CDamage_Calculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDamage_Calculator::Free()
{
	Clear_Modifiers();

	__super::Free();
}
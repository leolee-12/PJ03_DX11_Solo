#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

enum class LEVEL_ENTRY_PAYLOAD : _ubyte
{
	NONE,
	BATTLE_ENV,
	END
};

inline constexpr _uint g_kLevelEntryPayloadBytes = 128;

struct LEVEL_ENTRY_DESC
{
	LEVEL eNextLevelID = { LEVEL::END };
	LEVEL_ENTRY_PAYLOAD ePayload = { LEVEL_ENTRY_PAYLOAD::NONE };
	_uint iPayloadSize = {};
	_byte byPayload[g_kLevelEntryPayloadBytes] = {};

	void Clear()
	{
		eNextLevelID = LEVEL::END;
		ePayload = LEVEL_ENTRY_PAYLOAD::NONE;
		iPayloadSize = 0;
		memset(byPayload, 0, sizeof(byPayload));
	}

	HRESULT Set_Payload(LEVEL_ENTRY_PAYLOAD ePayloadType, const void* pPayload, _uint iSize)
	{
		if (nullptr == pPayload || 0 == iSize || iSize > g_kLevelEntryPayloadBytes)
			return E_FAIL;

		ePayload = ePayloadType;
		iPayloadSize = iSize;
		memset(byPayload, 0, sizeof(byPayload));
		memcpy(byPayload, pPayload, iSize);

		return S_OK;
	}

	const void* Get_Payload(LEVEL_ENTRY_PAYLOAD ePayloadType, _uint iExpectedSize) const
	{
		if (ePayload != ePayloadType)
			return nullptr;

		if (iPayloadSize != iExpectedSize)
			return nullptr;

		return byPayload;
	}
};

NS_END
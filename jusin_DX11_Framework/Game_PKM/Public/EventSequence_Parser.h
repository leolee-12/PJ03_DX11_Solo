#pragma once
#include "Event_Defines.h"

NS_BEGIN(Game_PKM)

class CEvent_Definition;

class CEventSequence_Parser final
{
private:
	CEventSequence_Parser() = delete;
	~CEventSequence_Parser() = delete;

public:
	static HRESULT Load_From_File(const _tchar* pFilePath, vector<CEvent_Definition*>& OutSequences);
};

NS_END
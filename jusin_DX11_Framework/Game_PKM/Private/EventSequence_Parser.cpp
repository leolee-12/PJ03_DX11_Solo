#include "EventSequence_Parser.h"
#include "Event_Definition.h"

#include <fstream>
#include <sstream>
#include <cctype>
#include <cstring>

namespace
{
    string Trim_(const string& str)
    {
        const size_t iBegin = str.find_first_not_of(" \t\r\n");
        if (string::npos == iBegin)
            return "";

        const size_t iEnd = str.find_last_not_of(" \t\r\n");
        return str.substr(iBegin, iEnd - iBegin + 1);
    }

    string Strip_Comment_(const string& str)
    {
        const size_t iComment = str.find('#');
        if (string::npos == iComment)
            return str;

        return str.substr(0, iComment);
    }

    string Normalize_ActionName_(const string& str)
    {
        string strOut;
        strOut.reserve(str.size());

        for (char ch : str)
        {
            if ('_' == ch || '-' == ch || ' ' == ch || '\t' == ch)
                continue;

            strOut.push_back(static_cast<char>(::toupper(static_cast<unsigned char>(ch))));
        }

        return strOut;
    }

    _wstring To_Wide_(const string& str)
    {
        return _wstring(str.begin(), str.end());
    }

    EVENT_ACTION_KIND Parse_ActionKind_(const string& strValue)
    {
        const string strKey = Normalize_ActionName_(strValue);

        if ("LOCKINPUT" == strKey)          return EVENT_ACTION_KIND::LOCK_INPUT;
        if ("RESTOREINPUT" == strKey)       return EVENT_ACTION_KIND::RESTORE_INPUT;
        if ("WAITSECONDS" == strKey)        return EVENT_ACTION_KIND::WAIT_SECONDS;
        if ("WAITDIALOGUE" == strKey)       return EVENT_ACTION_KIND::WAIT_DIALOGUE;
        if ("MESSAGEKEY" == strKey)         return EVENT_ACTION_KIND::MESSAGE_KEY;
        if ("MESSAGETEXT" == strKey)        return EVENT_ACTION_KIND::MESSAGE_TEXT;
        if ("CAMERAPUSH" == strKey)         return EVENT_ACTION_KIND::CAMERA_PUSH;
        if ("CAMERAPOP" == strKey)          return EVENT_ACTION_KIND::CAMERA_POP;
        if ("CAMERABLENDTOACTOR" == strKey) return EVENT_ACTION_KIND::CAMERA_BLEND_TO_ACTOR;
        if ("CAMERAFOLLOWACTOR" == strKey)  return EVENT_ACTION_KIND::CAMERA_FOLLOW_ACTOR;
        if ("ACTORFACE" == strKey)          return EVENT_ACTION_KIND::ACTOR_FACE;
        if ("ACTORMOVETO" == strKey)        return EVENT_ACTION_KIND::ACTOR_MOVE_TO;
        if ("ACTORSETANIM" == strKey)       return EVENT_ACTION_KIND::ACTOR_SET_ANIM;
        if ("ACTORSETVISIBLE" == strKey)    return EVENT_ACTION_KIND::ACTOR_SET_VISIBLE;
        if ("SPAWNNPC" == strKey)           return EVENT_ACTION_KIND::SPAWN_NPC;
        if ("DESPAWNACTOR" == strKey)       return EVENT_ACTION_KIND::DESPAWN_ACTOR;
        if ("REQUESTBATTLE" == strKey)      return EVENT_ACTION_KIND::REQUEST_BATTLE;
        if ("DEBUGLOG" == strKey)           return EVENT_ACTION_KIND::DEBUG_LOG;

        return EVENT_ACTION_KIND::NONE;
    }

    _bool Parse_SequenceID_(const string& strLine, _wstring& strOut)
    {
        if (0 != strLine.rfind("Sequence", 0))
            return false;

        string strRest = Trim_(strLine.substr(strlen("Sequence")));
        const size_t iBrace = strRest.find('{');
        if (string::npos != iBrace)
            strRest = Trim_(strRest.substr(0, iBrace));

        if (true == strRest.empty())
            return false;

        strOut = To_Wide_(strRest);
        return true;
    }

    _bool Parse_Step_(const string& strLine, EVENT_STEP_DESC& tOut)
    {
        const size_t iBegin = strLine.find('{');
        const size_t iEnd = strLine.rfind('}');

        if (string::npos == iBegin || string::npos == iEnd || iEnd <= iBegin)
            return false;

        string strBody = strLine.substr(iBegin + 1, iEnd - iBegin - 1);
        stringstream ss(strBody);
        string token;

        tOut = {};

        while (getline(ss, token, ';'))
        {
            token = Trim_(token);
            if (true == token.empty())
                continue;

            const size_t iEqual = token.find('=');
            if (string::npos == iEqual)
                continue;

            const string strKey = Trim_(token.substr(0, iEqual));
            const string strValue = Trim_(token.substr(iEqual + 1));

            if ("Action" == strKey)
                tOut.eKind = Parse_ActionKind_(strValue);
            else
                tOut.Params[strKey] = strValue;
        }

        return EVENT_ACTION_KIND::NONE != tOut.eKind;
    }

    void Release_Sequences_(vector<CEvent_Definition*>& Sequences)
    {
        for (CEvent_Definition*& pSequence : Sequences)
            Safe_Release(pSequence);

        Sequences.clear();
    }
}

HRESULT CEventSequence_Parser::Load_From_File(const _tchar* pFilePath, vector<CEvent_Definition*>&
    OutSequences)
{
    Release_Sequences_(OutSequences);

    if (nullptr == pFilePath)
        return E_FAIL;

    ifstream file(pFilePath);
    if (false == file.is_open())
    {
#ifdef _DEBUG
        OutputDebugStringA("[EventParser Warn] failed to open event file\n");
#endif
        return S_FALSE;
    }

    string line;
    CEvent_Definition* pCurrent = { nullptr };

    while (getline(file, line))
    {
        line = Trim_(Strip_Comment_(line));
        if (true == line.empty())
            continue;

        _wstring strSequenceID;
        if (true == Parse_SequenceID_(line, strSequenceID))
        {
            if (nullptr != pCurrent)
            {
                OutSequences.push_back(pCurrent);
                pCurrent = nullptr;
            }

            pCurrent = CEvent_Definition::Create(strSequenceID);
            if (nullptr == pCurrent)
            {
                Release_Sequences_(OutSequences);
                return E_FAIL;
            }

            continue;
        }

        if (0 == line.rfind("Step", 0))
        {
            if (nullptr == pCurrent)
                continue;

            EVENT_STEP_DESC tStep{};
            if (true == Parse_Step_(line, tStep))
                pCurrent->Add_Step(tStep);

            continue;
        }

        if ("}" == line)
        {
            if (nullptr != pCurrent)
            {
                OutSequences.push_back(pCurrent);
                pCurrent = nullptr;
            }

            continue;
        }
    }

    if (nullptr != pCurrent)
    {
        OutSequences.push_back(pCurrent);
        pCurrent = nullptr;
    }

#ifdef _DEBUG
    OutputDebugStringA(("[EventParser] Load sequence count = " + to_string(OutSequences.size()) +
        "\n").c_str());
#endif

    return true == OutSequences.empty() ? S_FALSE : S_OK;
}
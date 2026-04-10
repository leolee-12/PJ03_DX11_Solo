#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_Release(T& Instance)
	{
		unsigned int iRefCnt = { 0 };

		if (nullptr != Instance)
		{
			iRefCnt = Instance->Release();

			if (0 == iRefCnt)
				Instance = nullptr;
		}

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_AddRef(T& Instance)
	{
		unsigned int iRefCnt = { 0 };

		if (nullptr != Instance)
			iRefCnt = Instance->AddRef();

		return iRefCnt;
	}

    template <typename T>
    unsigned int Find_KeyIndex(const vector<T>& keys, float t)
    {
        if (keys.size() <= 1)
            return 0;

        for (unsigned int i = 0; i < keys.size() - 1; ++i)
        {
            if (t < keys[i + 1].fTrackPosition)
                return i;
        }

        return static_cast<unsigned int>(keys.size() - 2);
    }

    static void Debug_DumpMatrix(const char* tag, const _float4x4& m)
    {
#ifdef _DEBUG
        char buf[1024];
        sprintf_s(buf,
            "[%s]\n"
            "[% .4f % .4f % .4f % .4f]\n"
            "[% .4f % .4f % .4f % .4f]\n"
            "[% .4f % .4f % .4f % .4f]\n"
            "[% .4f % .4f % .4f % .4f]\n",
            tag,
            m._11, m._12, m._13, m._14,
            m._21, m._22, m._23, m._24,
            m._31, m._32, m._33, m._34,
            m._41, m._42, m._43, m._44);
        OutputDebugStringA(buf);
#endif
    }
}

#endif // Engine_Function_h__

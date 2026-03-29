#ifndef Engine_WName_h__
#define Engine_WName_h__

namespace Engine
{
	// 1. 문자열 ID 변환
	typedef unsigned char	_uint8;
	typedef unsigned int	_uint32;

	using WNameID = _uint32;

	// FNV-1a 32bit Hash - constexpr 연산 가능, 충돌 확률 낮음
	constexpr WNameID WName(const wchar_t* wStr, size_t iLength) noexcept
	{
		unsigned int hash = 2166136261u;

		for (size_t i = 0; i < iLength; ++i)
		{
			hash ^= static_cast<_uint8>(wStr[i]);
			hash *= 16777619u;
			hash ^= static_cast<_uint8>(wStr[i] >> 8);
			hash *= 16777619u;
		}

		return hash;
	}

	constexpr WNameID WName(const wchar_t* wStr) noexcept
	{
		size_t iLength = 0;
		while (wStr[iLength]) ++iLength;
		return WName(wStr, iLength);
	}

	constexpr WNameID operator""_wn(const wchar_t* wStr, size_t iLength) noexcept { return WName(wStr, iLength); }

	constexpr WNameID AName(const char* str, size_t iLength) noexcept
	{
		_uint32 hash = 2166136261u;

		for (size_t i = 0; i < iLength; ++i)
		{
			hash ^= static_cast<_uint8>(str[i]);
			hash *= 16777619u;
		}

		return hash;
	}

	constexpr WNameID operator""_an(const char* str, size_t iLength) noexcept { return AName(str, iLength); }



	// 2. 문자열 컨테이너
	static constexpr size_t DEFAULT_THRESHOLD = 8;
	static constexpr size_t ALWAYS_HASHMAP = 0;

	template<typename T, size_t THRESHOLD = DEFAULT_THRESHOLD>
	class WNameMap
	{
	private:
		vector<pair<WNameID, T>> m_Small;
		unordered_map<WNameID, T> m_Large;
		bool m_bLarge = false;

	private:
		void promote()
		{
			for (auto& pair : m_Small) m_Large.emplace(pair.first, move(pair.second));
			m_Small.clear();
			m_bLarge = true;
		}

	public:
		void emplace(WNameID id, T value)
		{
			if (!m_bLarge && m_Small.size() < THRESHOLD)
			{
				m_Small.emplace_back(id, move(value));
			}
			else
			{
				if (!m_bLarge) { /* promote */ promote(); }
				m_Large.emplace(id, move(value));
			}
		}

		T* find(WNameID id)
		{
			if (!m_bLarge)
			{
				for (auto& pair : m_Small)
					if (pair.first == id) return &pair.second;
				return nullptr;
			}
			auto iter = m_Large.find(id);
			return iter != m_Large.end() ? &iter->second : nullptr;
		}

		template<typename F>
		void for_each(F&& func)
		{
			if (!m_bLarge) { for (auto& pair : m_Small) func(pair); }
			else { for (auto& pair : m_Large) func(pair); }
		}

		void clear()
		{
			m_Small.clear();
			m_Large.clear();
			m_bLarge = false;
		}
	};

	template<typename T>
	class WNameMap<T, ALWAYS_HASHMAP>
	{
		unordered_map<WNameID, T> m_Map;
	public:
		void emplace(WNameID id, T value) { m_Map.emplace(id, move(value)); }
		T* find(WNameID id)
		{
			auto iter = m_Map.find(id);
			return iter != m_Map.end() ? &iter->second : nullptr;
		}
		template<typename F> void for_each(F&& func) { for (auto& pair : m_Map) func(pair); }
		void clear() { m_Map.clear(); }
	};

	// 3. 문자열 디버그용 메서드
#ifdef _DEBUG

	struct WNameRegistry
	{
		static unordered_map<WNameID, wstring>& Get()
		{
			static unordered_map<WNameID, wstring> strMap;
			return strMap;
		}

		static WNameID Register(WNameID id, wstring wStr)
		{
			Get().emplace(id, move(wStr));
			return id;
		}

		static const wchar_t* Lookup(WNameID id)	// 원본 문자열 확인
		{
			auto it = Get().find(id);
			return it != Get().end() ? it->second.c_str() : L"<unknown>";
		}
	};

	struct WNameAutoReg
	{
		WNameAutoReg(WNameID id, const wchar_t* wStr)
		{
			WNameRegistry::Register(id, wStr);
		}
	};

	// 런타임 문자열로 ID 생성 시 자동 등록
	inline WNameID WNameRT(const wstring& wStr)
	{
		WNameID id = WName(wStr.c_str(), wStr.size());
		auto& reg = WNameRegistry::Get();
		auto it = reg.find(id);
		if (it != reg.end())
		{
			if (it->second != wStr)
			{
				// 같은 ID에 다른 문자열이 등록되어 있으면 해시 충돌
				assert(it->second == wStr && "WName hash collision detected!");
			}
		}
		else
		{
			reg.emplace(id, wStr);
		}
		return id;
	}

	// 런타임 사용 용도 ~ Editor 등
#define WNAME(wStr) ::Engine::WNameRT(wStr)

	// 컴파일 타임 사용 용도 ~ Tags.h
#define WNAME_TAG(name, wStr)							\
	inline constexpr WNameID name = WName(wStr);		\
	inline const WNameAutoReg name##_reg_(name, wStr);
#else
	inline WNameID WNameRT(const wstring& wStr) noexcept { return WName(wStr.c_str(), wStr.size()); }

#define WNAME(wStr) ::Engine::WName(wStr)

#define WNAME_TAG(name, wStr)							\
	inline constexpr WNameID name = WName(wStr);

#endif	// _DEBUG

	// 4. 문자열 관련 비멤버 함수
	inline string WtoS(const wstring& wStr)
	{
		if (wStr.empty()) return {};
		int size = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		string result(size - 1, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, result.data(), size, nullptr, nullptr);
		return result;
	}

	inline wstring StoW(const string& str)
	{
		if (str.empty()) return {};
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		wstring result(size - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), size);
		return result;
	}

#define KOR(str) (const char*)u8##str
}

#endif // Engine_WName_h__
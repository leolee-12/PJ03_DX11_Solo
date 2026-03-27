#ifndef Engine_WName_h__
#define Engine_WName_h__

#include <unordered_map>
#include  <vector>

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

		static const wchar_t* Lookup(WNameID id)
		{
			auto it = Get().find(id);
			return it != Get().end() ? it->second.c_str() : L"<unknown>";
		}
	};

	// 런타임 문자열로 ID 생성 시 자동 등록
	inline WNameID WNameRT(const wstring& wStr)
	{
		WNameID id = WName(wStr.c_str(), wStr.size());
		WNameRegistry::Register(id, wStr);
		return id;
	}
#else
	inline WNameID WNameRT(const wstring& wStr) noexcept { return WName(wStr.c_str(), wStr.size()); }
#endif	// _DEBUG
}

#endif // Engine_WName_h__
#pragma once

#include "Engine_Defines.h"


BEGIN(Engine)

#pragma region 액션
typedef struct _ACTION
{
	_ACTION() : bAction() {}
	~_ACTION() {}

	bool bAction;

	/// <summary>
	/// 값 대입을 보기좋게 만든 것
	/// </summary>
	// 실행
	void Act()
	{
		bAction = true;
	}

	/// <summary>
	/// 값 대입을 보기좋게 만든 것
	/// </summary>
	// 초기화
	void Update()
	{
		bAction = false;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	// ture 반환후 초기화
	bool Sync()
	{
		if (bAction)
		{
			bAction = false;
			return true;
		}
		return false;
	}

	bool IsAct()
	{
		return bAction;
	}
}ACTION;

template<typename Key>
using MAP_ACTION = unordered_map<Key, ACTION>;

template<typename Key>
class CMapAction_Updator
{
public:
	void operator() (pair<const Key, ACTION>& Action)
	{
		Action.second.Update();
	}
};

template<typename T, typename Key = underlying_type_t<T>>
struct ACTION_SET
{
	ACTION_SET() {}
	~ACTION_SET() {}

public:
	map<Key, ACTION> mapAction;

	void Add_Action(Key&& tKey)
	{
		mapAction.emplace(tKey, ACTION());
	}
	template<typename Type = underlying_type_t<T>>
	void Add_Action(Type tKey)
	{
		mapAction.emplace(tKey, ACTION());
	}

	ACTION& operator[] (Key&& tKey)
	{
		return mapAction[tKey];
	}
	template<typename Type = underlying_type_t<T>>
	ACTION& operator[] (Type tKey)
	{
		return mapAction[tKey];
	}

	void Act(Key&& tKey)
	{
		mapAction[tKey].Act();
	}
	template<typename Type = underlying_type_t<T>>
	void Act(Type tKey)
	{
		mapAction[tKey].Act();
	}

	_bool IsAct(Key&& tKey)
	{
		return mapAction[tKey].IsAct();
	}
	template<typename Type = underlying_type_t<T>>
	_bool IsAct(Type tKey)
	{
		return mapAction[tKey].IsAct();
	}

	void Reset()
	{
		for_each(mapAction.begin(), mapAction.end(), CMapAction_Updator<Key>());
	}
};
#pragma endregion

END
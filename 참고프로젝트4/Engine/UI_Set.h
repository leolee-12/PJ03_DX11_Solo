#pragma once
#include "UIObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI_Set : public CBase // UIObject하나당 하나씩
{
public:
	typedef struct tagMotionSet_Data { 
		string								strMotionName; //모션의 이름(Click 등)
		vector<CUIObject::UI_MOTION_DESC>	vecMotionData; //움직임 데이터(키프레임, Pos 등)
		MOTION_SETTING_DESC						Motion_Setting_Data; //모션 세팅 데이터(듀레이션 등)
	}UI_FULLMOTION_SET;

private:
	CUI_Set();
	virtual ~CUI_Set() = default;

public:
	HRESULT Initialize_Prototype(string _UISetName);

	void InPut_FullMotion_Data(string _MotionName, string _UIName, 
		vector<CUIObject::UI_MOTION_DESC>& _vecMotionData, MOTION_SETTING_DESC _MotionSetData);
	
	void Delete_Motion_Data(string _MotionName); //개체가 가지고 있는 모션을 지우는 함수
	string Get_UISetName() { return m_strUISetName; }

private:
	string									m_strUISetName = { "" }; //UISet 전체의 이름(예: Command)
	map<string, vector<UI_FULLMOTION_SET>>	m_mapFullMotionSet = {};  //키값 : UI 개체 이름(예: Menu1)

	_int		m_iCmpUICount = { 0 }; //UISet에 속한 UI 개체 수
	_int		m_iMotionCount = { 0 }; //UISet에 있는 모션의 개수
	_int		m_iCurrentMotionIdx = { 0 };

public:
	static CUI_Set* Create(string _UISetName);
	virtual void Free() override;
};

END
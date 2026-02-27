#pragma once

#include <map>
#include <string>   

#include "AnimationEvent_Defines.h"
#include "Client_Defines.h"

BEGIN(Client)


using MAP_AnimationIndexPedia = map<string, _int>;
using MAP_AnimationContainer = map<_int, MAP_AnimationIndexPedia>;

class CAnimationNameMap 
{
public:
	CAnimationNameMap();
	void Initalize();

private:



	

public:
	_int Get_AnimationIndex(_int Characterenum, string strAnimationName);


private:
	MAP_AnimationContainer m_MAP_AnimationPedia;
};
END;
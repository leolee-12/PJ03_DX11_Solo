#pragma once

#include "Engine_Defines.h"


NS_BEGIN(Engine)

/* 모든 클래스들의 부모가 되는 클래스다 .*/
/* 모든 클래스에게 필요한 기능을 상속하여 내려준다. */
/* 레퍼런스 카운트 관리의 기능ㅇ */
class ENGINE_DLL CBase abstract
{
protected:           
	CBase();
	virtual ~CBase() = default;

public:
	/* 레퍼런스 카운트를 증가하낟. */
	/* 증가한 결과를 보내준다. */
	_uint AddRef();

	/* 레퍼런스 카운트를 감소한다. or 삭제한다. */
	/* 감소하기 전의 결과를 보내준다. */
	_uint Release();

protected:
	_uint			m_iRefCnt = {};

public:
	virtual void Free();
};

NS_END
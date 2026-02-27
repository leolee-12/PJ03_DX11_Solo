#pragma once

#include "Engine_Defines.h"
#include "Renderer_Defines.h"

/* 모든 클래스의 부모가 되기위한 클래스이다. */
/* 레퍼런스 카운트를 관리(AddRef, Release)한다. */

BEGIN(Engine)

/* abstract  : 클래스를 추상화하겠다. */
class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* RefCnt를 증가시켜주겠다. */
	_uint AddRef();

	/* RefCnt를 감소시켜주겠다. or 삭제한다. */
	_uint Release();

private:
	_uint m_iRefCnt = { 0 };

public:
	_bool m_bDead = { false };
public:
	/* 소멸자를 대체한다. */
	virtual void Free() {}
};

END


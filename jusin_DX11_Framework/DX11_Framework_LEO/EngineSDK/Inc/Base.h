#pragma once
#include "Engine_Defines.h"

/* ------------------------------------------------------------ */
// CBase : RefCnt 관리 기능을 모든 자식 클래스에게 상속시킴
/* ------------------------------------------------------------ */

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;
	
public:
	/* RefCnt 증가 */
	_uint AddRef();

	/* RefCnt 감소 or 객체 삭제 */
	_uint Release();

protected:
	_uint m_iRefCnt = { };

public:
	virtual void Free();
};

NS_END
#ifndef SharedTextureBinder_h__
#define SharedTextureBinder_h__

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ISharedTextureBinder
{
public:
	virtual ~ISharedTextureBinder() = default;
	
	virtual HRESULT Bind_SharedTextures(class CShader* pShader, const vector<UI_SHARED_TEXTURE_BINDING_DESC>& Bindings) = 0;
};

NS_END

#endif // SharedTextureBinder_h__
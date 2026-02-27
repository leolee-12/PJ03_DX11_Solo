#pragma once
#include "IMGUI_Tab.h"


BEGIN(Client)

class CIMGUI_Level_Tab : public CIMGUI_Tab
{

protected:
	CIMGUI_Level_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Level_Tab() = default;

public:
	HRESULT Initialize() override;
	void Render(_float fTimeDelta) override;

	virtual const _char* GetTabName() const { return "Level"; };

private:
	HRESULT IMGUI_ShowLevels();

public:
	static CIMGUI_Level_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

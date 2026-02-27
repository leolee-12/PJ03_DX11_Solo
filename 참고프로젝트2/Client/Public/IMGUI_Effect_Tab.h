#pragma once
#include "IMGUI_Tab.h"


BEGIN(Client)

class CIMGUI_Effect_Tab : public CIMGUI_Tab
{

protected:
	CIMGUI_Effect_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Effect_Tab() = default;

public:
	HRESULT Initialize() override;
	void Render(_float fTimeDelta) override;
	void Push_Initialize();
	void Save_To_Effect_Layer(_uint iCurTestEffectIndex, const wstring& strEffectLayerTag, void* pArg = nullptr);
	HRESULT Save_Selected_Effects_File();
	HRESULT Save_All_Effects_File();

	string WStringToUTF8(const std::wstring& wstr);
	wstring UTF8ToWString(const string& utf8Str);
	virtual const _char* GetTabName() const { return "Effect"; };

	void Render_For_Each_Effect();
	void Render_For_Effect_Layer();
	void Render_For_Layer_KeyFrame(_float fTimeDelta);
	void Render_For_Effect_KeyFrame();
	void Render_For_Effect_Color();
	void Render_For_Layer_Transform();

private:
	class CEffect_Manager* m_pEffect_Manager = { nullptr };
	_float4x4 m_IdentityMatrix;
	vector<string> ModelName;
	vector<string> TextureName;
	vector<EFFECT_LAYER_DATA> m_vecEffectData;
	string EffectLayerKey;
	wstring EffectName;
	_bool m_isInitialize = { false };
	_float SelectCurTime = { 0.f };
	wstring PreLayer;
	wstring CurLayer;

public:
	static CIMGUI_Effect_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

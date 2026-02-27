//#pragma once
//#include "Imgui_Tab.h"
//#include "ImGuizmo.h"
//
//#include "Model_Data.h"
//
//BEGIN(Client)
//
//class CImgui_Tab_BakeBinary : public CImgui_Tab
//{
//private:
//	CImgui_Tab_BakeBinary(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
//	virtual	~CImgui_Tab_BakeBinary() = default;
//
//public:
//	virtual HRESULT Initialize();
//	virtual void Tick(_cref_time fTimeDelta);
//	virtual HRESULT Render();
//
//public:
//	HRESULT Bake_Binary(string filePathName, MODEL_DATA& ModelData);
//	HRESULT BakeBinary_FromAssimp();
//
//#ifdef _DEBUG
//private:
//	Assimp::Importer		m_Importer;
//	const aiScene* m_pAIScene = { nullptr };
//#endif
//	_int					m_eModelType = { MODELTYPE::ANIM };
//
//	vector<pair<const wstring, CGameObject*>> m_PrototypeList;
//
//public:
//	static CImgui_Tab_BakeBinary* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
//	virtual void	Free() override;
//
//};
//
//END

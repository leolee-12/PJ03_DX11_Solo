#ifdef _DEBUG
#pragma once
#include "Imgui_Tab.h"
#include "ImGuizmo.h"
#include "VIBuffer_Instancing.h"

#include "Imgui_Tab_ParticleEdit.h"
#include "Imgui_Tab_TrailEdit.h"

BEGIN(Client)

class CImgui_Tab_ModelAnim : public CImgui_Tab
{
private:
	CImgui_Tab_ModelAnim(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_ModelAnim() = default;

public:
	virtual HRESULT Initialize( CImgui_Tab_ParticleEdit* pParticleTab, CImgui_Tab_TrailEdit* pTrailTab);
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	CImgui_Tab_ParticleEdit* m_pParticleTab = { nullptr };
	CImgui_Tab_TrailEdit* m_pTrailTab = { nullptr };
	GETSET_EX2(CImgui_Tab_ParticleEdit*, m_pParticleTab, ParticleTab, GET, SET)
		vector<class CCharacter*> CharacterList;

	_int m_iPickIndex = { 0 };
	class CCharacter* m_pPickCharacter = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_int m_iPlayParticleFrameIndex = { 0 };
	_int m_iPlayTrailFrameIndex = { 0 };
	_int m_iCurFrameIndex = { 0 };
	_bool m_bPlayOnFrame = { false };
	_bool m_bPlayOnce[2] = { false,false };

	map<string, _uint>* m_AnimationsName;
	string m_strCurAnimname = {};
	_bool m_bAnimLoop = { false };
	_float m_fAnimSpeed = { 1.0f };
	_bool m_bCharacterMove = { false };
public:
	static CImgui_Tab_ModelAnim* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CImgui_Tab_ParticleEdit* pParticleTab, CImgui_Tab_TrailEdit* pTrailTab);
	virtual void	Free() override;

};

END

#endif // DEBUG

#ifdef _DEBUG
#pragma once

#include "Imgui_Tab.h"


BEGIN(Client)

/// <summary>
/// 모델과 애니메이션 리스트를 띄우고
/// 선택한 모델과 애니메이션을 적용한다.
/// </summary>
class CImgui_Tab_AnimMask : public CImgui_Tab
{
	INFO_CLASS(CImgui_Tab_AnimMask, CImgui_Tab)

private:
	CImgui_Tab_AnimMask(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_AnimMask() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	static CImgui_Tab_AnimMask* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;


private:
	void Change_MaskAnimation();

private:
	weak_ptr<class CAnimObject>		m_pAnimObject;

public:
	struct TRecorder
	{
		_int Cur;
		_int Prev;
	};

private:
	TRecorder	m_iAnimSelect_Index = { -1, -1 };
	wstring		m_AnimSelect_Name = { TEXT("") };

	wstring			m_BasisBone_Name = { TEXT("") };
	vector<wstring> m_EndBone_Names = { TEXT("")};
	_uint			m_iNumEndBones = { 0 };
	_bool			m_bLoop = { false };
	_uint			m_iRecursive = { UINT_MAX };
};

END
#endif // DEBUG

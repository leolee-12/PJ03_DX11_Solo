#ifdef _DEBUG
#pragma once

#include "Imgui_Tab.h"


BEGIN(Client)

/// <summary>
/// 모델과 애니메이션 리스트를 띄우고
/// 선택한 모델과 애니메이션을 적용한다.
/// </summary>
class CImgui_Tab_AnimAdd : public CImgui_Tab
{
	INFO_CLASS(CImgui_Tab_AnimAdd, CImgui_Tab)

private:
	CImgui_Tab_AnimAdd(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_AnimAdd() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	static CImgui_Tab_AnimAdd* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;


private:
	void Change_AddAnimation();

private:
	weak_ptr<class CAnimObject>		m_pAnimObject;

public:
	struct TRecorder
	{
		_int Cur;
		_int Prev;
	};

private:
	TRecorder		m_iAnimSelect_Index = { -1, -1 };
	wstring			m_AnimSelect_Name = { TEXT("") };

	wstring			m_BasisBone_Name = { TEXT("") };
	vector<wstring>	m_EndBone_Name = { TEXT("") };
	_uint			m_iNumEndBones = { 0 };
	_bool			m_bLoop = { false };
	_float			m_fSpeed = { 1.f };
	_float			m_fWeight = { 1.f };
	_uint			m_iRecursive = { UINT_MAX };

};

END
#endif // DEBUG

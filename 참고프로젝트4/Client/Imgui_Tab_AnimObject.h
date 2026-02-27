#ifdef _DEBUG
#pragma once

#include "Imgui_Tab.h"


BEGIN(Client)

/// <summary>
/// 모델과 애니메이션 리스트를 띄우고
/// 선택한 모델과 애니메이션을 적용한다.
/// </summary>
class CImgui_Tab_AnimObject : public CImgui_Tab
{
	INFO_CLASS(CImgui_Tab_AnimObject, CImgui_Tab)

private:
	CImgui_Tab_AnimObject(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_AnimObject() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	static CImgui_Tab_AnimObject* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

public:
	// 모델을 에디터에 불러오기
	void Load_ModelObjectToEditor();

	// 모델 오브젝트를 에디터에서 삭제
	void Delete_ModelObjectFromEditor();

	void TogglePlay_AnimObject();
	void Play_AnimObject();
	void Pause_AnimObject();
	void Reset_AnimObject();
	void Set_TrackPos_AnimObject(_int iIndex);
	_int Get_TrackPos_AnimObject();
	_int Get_Duration_AnimObject();

	shared_ptr<class CAnimObject>& Get_AnimObject() { return m_pAnimObject; }

private:
	shared_ptr<class CAnimObject>		m_pAnimObject = { nullptr };


public:
	struct TRecorder
	{
		_int Cur;
		_int Prev;
	};

	// 모델 리스트 박스
private:
	TRecorder	m_iModelSelect_Index = { -1, -1 };
	wstring		m_ModelSelect_Name = TEXT("");

public:
	void Change_Animation();

private:
	TRecorder	m_iAnimSelect_Index = { -1, -1 };
	wstring		m_AnimSelect_Name = { TEXT("") };


private:
	_int	m_iBoneSelect_index = { -1 };
};

END
#endif // DEBUG

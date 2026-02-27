#ifdef _DEBUG
#pragma once

#include "Imgui_Tab.h"

#include "ImCurveEdit.h"
#include "ImSequencer.h"
#include "Utility/DelegateTemplate.h"
#include "AnimNotify.h"
#include "AnimNotifyCollection_Client.h"
#include "AnimNotify_Sound.h"
#include "AnimNotify_Effect.h"
#include "AnimNotify_Event.h"
#include "AnimNotifyComp.h"

using namespace ImSequencer;

BEGIN(Client)

class CImgui_Tab_AnimSequencer : public CImgui_Tab
{
    INFO_CLASS(CImgui_Tab_AnimSequencer, CImgui_Tab)

private:
    CImgui_Tab_AnimSequencer(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual	~CImgui_Tab_AnimSequencer() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
    static CImgui_Tab_AnimSequencer* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void	Free() override;


public:
    HRESULT Render_AnimationTrack();
    HRESULT Render_EffectTrack();
    HRESULT Render_SoundTrack();
    HRESULT Render_EventTrack();

    HRESULT Render_EffectNotify();
    HRESULT Render_SoundNotify();
    HRESULT Render_EventNotify();
    HRESULT Render_BakeNotifies();


public:
    enum ESeqType
    {
        ESeqType_Animation,
        ESeqType_Effect,
        ESeqType_Sound,
        ESeqType_Event,
        ESeqType_End,
    };

public:
    // 노티파이 초기화
    void Reset_NotifyEvents();
    // 노티파이 공간 확보
    void Reserve_NotifyContainer();
    // 노티파이 컬렉션 생성기 등록
    void Regist_NotifyCreatorCallback();

    _bool IsNotifyCollection_Valid();

public: // 파싱
    HRESULT Parse_Notifies(const wstring& strFilePath);
    HRESULT Read_Notifies(const wstring& strFilePath);

private:
    // 시퀀서 변수들
    _int        m_iPrevFrame = { 0 };
    _int        m_iCurrentFrame = 0;
    _int        m_iSelectedEntity = { -1 };
    _int        m_iStartFrame = { 0 };
    _int        m_iEndFrame = { 100 };

    // 시퀀서 트랙
    _bool       m_bIsSeqTrackOpen[ESeqType_End] = { true };

    // 시퀀서 트랙별 키
    vector<_int_32> m_Keys[ESeqType_End];
    shared_ptr<CAnimNotifyComp> m_pNotifyComp = { nullptr };
    weak_ptr<class CAnimObject>       m_pAnimObject;


    // 사운드 노티파이 추가 관련
private:
    _bool                   m_bIsSoundNotify_ModelOpen = { false };
    _int_32                 m_iSoundNotify_TrackPos = { 0 };
    FNotifyType_Sound       m_SoundNotifyDesc = {};
    _int                    m_iSelectedSoundGroup_Index = { -1 };
    string                  m_strCurrentSound_Name = "";
    _int                    m_iSelectedSound_Index = { -1 };
    string                  m_strAddNotify_SoundGroup = "";
    string                  m_strAddNotify_SoundName = "";

    const _char*            m_EffectNotifyNameList[5] = { "Particle", "Rect", "Mesh", "Trail", "Group" };
    _int                    m_iSelectedEffectGroup_Index = { -1 };
    string                  m_strCurrentEffectGroup_Name = "";

    _int                    m_iSelectedEffect_Index = { -1 };
    FNotifyType_Effect      m_EffectNotifyDesc = {};


private:
    FNotifyType_Event       m_EventNotifyDesc = {};
    

};

END
#endif // DEBUG

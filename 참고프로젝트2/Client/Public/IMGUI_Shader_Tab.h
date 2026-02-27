#pragma once
#include "IMGUI_Tab.h"
#include "Shader_Texture.h"
#include "Shader_Tab_Defines.h"
#include "Effect.h"
#include "Effect_Layer.h"

BEGIN(Engine)
class CGameObject;
END
//#include "GameObject.h"
BEGIN(Client)

class CIMGUI_Shader_Tab : public CIMGUI_Tab
{
public:
	enum ENDNODE_TYPE { END_DIFFUSE, END_ALPHA };
	enum NODE_TYPE { NODE_BASIC, NODE_FUNCTION, NODE_LINK, NODE_END};
	enum FUNCTION_TYPE { FUNCTION_TEXMOVE, FUNCTION_SPRITE, FUNCTION_END };
public:
	typedef struct
	{
		_int MoveTex_node_id = { 1500 };
		_float2 fDirection = { 0.f,0.f };
		_float fSpeed = { 0.f };
	}MoveTex_Node;

	typedef struct
	{
		_bool isLoop = { false };
		_int Sprite_node_id = { 3000 };
		_float2 fSpriteSizeNumber = { 0.f,0.f };
		_float fSpeed = { 0.f };
	}Sprite_Node;

	typedef struct
	{
		_int iID;
		ImTextureID Texture;
		ImTextureID Alpha;
	}SRV_Texture;


protected:
	CIMGUI_Shader_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, CEffect* pEffect);
	CIMGUI_Shader_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture);


	virtual ~CIMGUI_Shader_Tab() = default;
	
public:
	HRESULT Initialize() override;
	HRESULT Load_Initialize(string strFilename);

	
	void Update(_float fTimeDelta);
	void Render(_float fTimeDelta) override;
	void TabPos_Init(); 
	void Create_NodeTexture(string szPath);
	void Push_ShaderTexture(CShader_Texture* pShaderTexture) {
		m_NodeTextures.push_back(pShaderTexture);
	}
	virtual const _char* GetTabName() const { return "Shader"; };
	void Render_MainNode();
	void Render_TextureNode();
	void Render_MoveTexNode();
	void Render_SpriteNode();
	void Render_Link();
	void Check_Create_Link();
	void Check_Delete_Link();
	void Draw_MusicButton(CShader_Texture* pShaderTexture);

	void Click_Save_Shader_Tab(string fileName);
	void Click_Load_Shader_Tab(string fileName);
	void Create_Link(_int start_attr, _int end_attr);
	void Load_NodeTextures(vector<Save_Key>& PrototypeKeys);

	void Save_Shader_Tab(string fileName, const Shader_Tab_Save& shaderTabSave, _int version = 2);
	void Load_Shader_Tab(string fileName, Shader_Tab_Save& shaderTabSave);

	void Update_TestToLayer_TextureCom(CTexture* pTexture, CEffect* pEffect) { m_TestEffectModel_Texture = pTexture; 
	m_pEffect = pEffect;
	}

	void Add_Clone_EffectToShader_Texture(CEffect* pEffect);
	_int Update_Clone_EffectToShader_Texture(CEffect* pEffect, _float fTimeDelta);
	void Delete_Clone_EffectToShader_Texture(CEffect* pEffect);
private:
	unordered_map<CEffect*, vector<CShader_Texture*>*> m_CopyClones;

public:
	_int m_iNumberId = { -1 };
	_bool m_TabPick = { false };

	string m_FileName;
private: /* 이 m_PrototypeKeys 로 Load_NodeTextures() 함수호출로 객체들을 다 생성한다 */
	//vector<*>			m_ShaderTexture_TexCoords;
	vector<Save_Key>			m_PrototypeKeys;							
	/*----------------------------m_PrototypeKeys-----------------------------------------------------------------------------------------*/
	CTexture*					m_pDefaultTexture = { nullptr };			// 꼇다 뺏다 할때 필요한 흰색텍스쳐컴
	vector<CShader_Texture*>	m_NodeTextures;								// 텍스쳐 노드에 텍스쳐객체들
	vector<SRV_Texture>			m_NodeTextureSRVs;							// 그 텍스쳐객체가 그리는 렌더타겟의 그려진 결과 ShaderReasouceView*
public:
	CTexture*					m_TestEffectModel_Texture = { nullptr };	// 누나가 생성한 테스트이펙트객체의 텍스쳐 컴포넌트
	/*------------------------------------------------------------------------------------------------------------------------------------*/
private:
	_int						m_iNodeTextureCount = 0;		// 노드 텍스쳐의 개수
	_bool						isStart = false;				// 외부에서 껏다켯다 할때 쓰는 변수

private: /* ImNodes 와 관련된 멤버 변수 */
	vector<pair<_int, _int>>	links;							// 노드와 노드 사이를 이어주기 위한 벡터 ( 단순히 int형 ID를 가지고 있음 )

	const _int					m_iMain_node_id = -1;			// 메인 노드 id
	_int						m_iMain_Input_Diffuse_id = 0;	// 디퓨즈에 연결된 id

	_int						m_iAttributeCount = 4;			// 노드 Attribute 개수 이게 늘어나면 노드 최대 개수가 줄어듦

	_int						unique_node_id = 1;				// 노드 최대 개수 500개
	vector<_int>				node_ids;						// 이건 텍스쳐 노드로 쓰자

	_int						m_MoveTex_node_id = 1501;		// 임시로 정해준 무브텍스 노드 아이디 시작점 
	list<MoveTex_Node>			m_MoveTex_Node_ids;				// 무브텍스 노드들

	_int						m_Sprite_node_id = 3001;		// 임시로 정해준 스프라이트 노드 아이디 시작점 
	list<Sprite_Node>			m_Sprite_Node_ids;				// 스프라이트 노드들
	
	unordered_map<int, ImVec2>	node_positions;					// 탭 전환시 노드 위치가 초기화되는데 그걸 막기위해 항상 노드들의 위치를 모두 저장

public:
	void Save_ClientBinary();

public:
	void RemoveSubstring(std::wstring& wstr, const std::wstring& toRemove);
	string WStringToString(const std::wstring& wstr);
	void ShowTextWithoutPrefix(const std::wstring& wstr);
	size_t GetRemainingFileSize(ifstream& file);
	size_t GetFileSize(ifstream& file);
public:
	CEffect* m_pEffect = { nullptr };
	static CIMGUI_Shader_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, CEffect* pEffect);
	static CIMGUI_Shader_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture);
	static CIMGUI_Shader_Tab* Create_Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, string strFilename, CEffect* pEffect);

	virtual void Free() override;
};

END

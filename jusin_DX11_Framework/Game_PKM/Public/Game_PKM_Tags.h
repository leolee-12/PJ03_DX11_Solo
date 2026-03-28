#ifndef Game_PKM_Tags_h__
#define Game_PKM_Tags_h__

namespace Game_PKM
{
	// Texture
	static Engine::WNameID PROTO_COM_TEXTURE_BACKGROUND = WNAME(L"Prototype_Component_Texture_BackGround");
	static Engine::WNameID PROTO_COM_TEXTURE_TERRAIN = WNAME(L"Prototype_Component_Texture_Terrain");
	
	// Shader
	static Engine::WNameID PROTO_COM_SHADER_VTXTEX = WNAME(L"Prototype_Component_Shader_VtxTex");
	static Engine::WNameID PROTO_COM_SHADER_VTXNORTEX = WNAME(L"Prototype_Component_Shader_VtxNorTex");
	static Engine::WNameID PROTO_COM_SHADER_VTXMESH = WNAME(L"Prototype_Component_Shader_VtxMesh");

	// VIBuffer & Model
	static Engine::WNameID PROTO_COM_VIBUFFER_RECT = WNAME(L"Prototype_Component_VIBuffer_Rect");
	static Engine::WNameID PROTO_COM_VIBUFFER_TERRAIN = WNAME(L"Prototype_Component_VIBuffer_Terrain");
	static Engine::WNameID PROTO_COM_MODEL_FIONA = WNAME(L"Prototype_Component_Model_Fiona");
	static Engine::WNameID PROTO_COM_MODEL_FORKLIFT = WNAME(L"Prototype_Component_Model_ForkLift");

	// Object
	static Engine::WNameID PROTO_OBJ_BACKGROUND = WNAME(L"Prototype_GameObject_BackGround");
	static Engine::WNameID PROTO_OBJ_TERRAIN = WNAME(L"Prototype_GameObject_Terrain");
	static Engine::WNameID PROTO_OBJ_CAMERA_FREE = WNAME(L"Prototype_GameObject_Camera_Free");
	static Engine::WNameID PROTO_OBJ_MONSTER = WNAME(L"Prototype_GameObject_Monster");



	// Layer
	static Engine::WNameID LAYER_BACKGROUND = WNAME(L"Layer_BackGround");
	static Engine::WNameID LAYER_CAMERA = WNAME(L"Layer_Camera");
	static Engine::WNameID LAYER_PLAYER = WNAME(L"Layer_Player");
	static Engine::WNameID LAYER_MONSTER = WNAME(L"Layer_Monster");
}

#endif // Game_PKM_Tags_h__
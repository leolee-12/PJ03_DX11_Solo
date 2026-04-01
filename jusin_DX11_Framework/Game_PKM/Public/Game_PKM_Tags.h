#ifndef Game_PKM_Tags_h__
#define Game_PKM_Tags_h__

NS_BEGIN(Game_PKM)

// Texture
WNAME_TAG(PROTO_COM_TEXTURE_BACKGROUND, L"Prototype_Component_Texture_BackGround");
WNAME_TAG(PROTO_COM_TEXTURE_TERRAIN, L"Prototype_Component_Texture_Terrain");

// Shader
WNAME_TAG(PROTO_COM_SHADER_VTXTEX, L"Prototype_Component_Shader_VtxTex");
WNAME_TAG(PROTO_COM_SHADER_VTXNORTEX, L"Prototype_Component_Shader_VtxNorTex");
WNAME_TAG(PROTO_COM_SHADER_VTXMESH, L"Prototype_Component_Shader_VtxMesh");
WNAME_TAG(PROTO_COM_SHADER_VTXANIMMESH, L"Prototype_Component_Shader_VtxAnimMesh");

// VIBuffer & Model
WNAME_TAG(PROTO_COM_VIBUFFER_RECT, L"Prototype_Component_VIBuffer_Rect");
WNAME_TAG(PROTO_COM_VIBUFFER_TERRAIN, L"Prototype_Component_VIBuffer_Terrain");
WNAME_TAG(PROTO_COM_MODEL_FIONA, L"Prototype_Component_Model_Fiona");
WNAME_TAG(PROTO_COM_MODEL_FORKLIFT, L"Prototype_Component_Model_ForkLift");

// Layer
WNAME_TAG(LAYER_BACKGROUND, L"Layer_BackGround");
WNAME_TAG(LAYER_CAMERA, L"Layer_Camera");
WNAME_TAG(LAYER_PLAYER, L"Layer_Player");
WNAME_TAG(LAYER_MONSTER, L"Layer_Monster");

// Object
WNAME_TAG(PROTO_OBJ_BACKGROUND, L"Prototype_GameObject_BackGround");
WNAME_TAG(PROTO_OBJ_TERRAIN, L"Prototype_GameObject_Terrain");
WNAME_TAG(PROTO_OBJ_CAMERA_FREE, L"Prototype_GameObject_Camera_Free");
WNAME_TAG(PROTO_OBJ_MONSTER, L"Prototype_GameObject_Monster");
WNAME_TAG(PROTO_OBJ_FORKLIFT, L"Prototype_GameObject_ForkLift");

// Component
WNAME_TAG(COM_VIBUFFER, L"Com_VIBuffer");
WNAME_TAG(COM_TEXTURE, L"Com_Texture");
WNAME_TAG(COM_SHADER, L"Com_Shader");
WNAME_TAG(COM_MODEL, L"Com_Model");

NS_END

#endif // Game_PKM_Tags_h__
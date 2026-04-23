#ifndef Game_PKM_Tags_h__
#define Game_PKM_Tags_h__

NS_BEGIN(Game_PKM)

// Texture
WNAME_TAG(PROTO_COM_TEXTURE_BACKGROUND, L"Prototype_Component_Texture_BackGround");
WNAME_TAG(PROTO_COM_TEXTURE_TERRAIN_DIFF, L"Prototype_Component_Texture_Terrain_Diffuse");
WNAME_TAG(PROTO_COM_TEXTURE_TERRAIN_MASK, L"Prototype_Component_Texture_Terrain_Mask");
WNAME_TAG(PROTO_COM_TEXTURE_TERRAIN_BRUSH, L"Prototype_Component_Texture_Terrain_Brush");
WNAME_TAG(PROTO_COM_TEXTURE_SKY, L"Prototype_Component_Texture_Sky");
WNAME_TAG(PROTO_COM_TEXTURE_SNOW, L"Prototype_Component_Texture_Snow");

WNAME_TAG(PROTO_COM_TEXTURE_DUMMY_WHITE, L"Prototype_Component_Texture_Dummy_White");
WNAME_TAG(PROTO_COM_TEXTURE_TITLE_PBGF_DIFF, L"Prototype_Component_Texture_Title_pbgf_Diff");
WNAME_TAG(PROTO_COM_TEXTURE_TITLE_PBGF_NOISE, L"Prototype_Component_Texture_Title_pbgf_Noise");
WNAME_TAG(PROTO_COM_TEXTURE_TITLE_LOGO_DIFF, L"Prototype_Component_Texture_Title_Logo_Diff");
WNAME_TAG(PROTO_COM_TEXTURE_TITLE_BG_GRAD, L"Prototype_Component_Texture_Title_BG_Grad");
WNAME_TAG(PROTO_COM_TEXTURE_PIKA_MASK, L"Prototype_Component_Texture_Title_Pika_Mask");

WNAME_TAG(PROTO_COM_TEXTURE_MENU_PARTNER, L"Prototype_Component_Texture_Menu_Partner");
WNAME_TAG(PROTO_COM_TEXTURE_MENU_DEX, L"Prototype_Component_Texture_Menu_Dex");
WNAME_TAG(PROTO_COM_TEXTURE_MENU_BAG, L"Prototype_Component_Texture_Menu_Bag");
WNAME_TAG(PROTO_COM_TEXTURE_MENU_ENTRY, L"Prototype_Component_Texture_Menu_Entry");
WNAME_TAG(PROTO_COM_TEXTURE_MENU_LINK, L"Prototype_Component_Texture_Menu_Link");
WNAME_TAG(PROTO_COM_TEXTURE_MENU_REPORT, L"Prototype_Component_Texture_Menu_Report");

// Shader
WNAME_TAG(PROTO_COM_SHADER_VTXTEX, L"Prototype_Component_Shader_VtxTex");
WNAME_TAG(PROTO_COM_SHADER_VTXNORTEX, L"Prototype_Component_Shader_VtxNorTex");
WNAME_TAG(PROTO_COM_SHADER_VTXMESH, L"Prototype_Component_Shader_VtxMesh");
WNAME_TAG(PROTO_COM_SHADER_VTXANIMMESH, L"Prototype_Component_Shader_VtxAnimMesh");
WNAME_TAG(PROTO_COM_SHADER_VTXCUBE, L"Prototype_Component_Shader_VtxCube");
WNAME_TAG(PROTO_COM_SHADER_VTXRECTINST, L"Prototype_Component_Shader_VtxRectInstance");
WNAME_TAG(PROTO_COM_SHADER_VTXPOINTINST, L"Prototype_Component_Shader_VtxPointInstance");

WNAME_TAG(PROTO_COM_SHADER_PLAYER_LGPE, L"Prototype_Component_Shader_Player_LGPE");
WNAME_TAG(PROTO_COM_SHADER_UI, L"Prototype_Component_Shader_UI");

// VIBuffer & Model
WNAME_TAG(PROTO_COM_VIBUFFER_RECT, L"Prototype_Component_VIBuffer_Rect");
WNAME_TAG(PROTO_COM_VIBUFFER_TERRAIN, L"Prototype_Component_VIBuffer_Terrain");
WNAME_TAG(PROTO_COM_VIBUFFER_CUBE, L"Prototype_Component_VIBuffer_Cube");
WNAME_TAG(PROTO_COM_VIBUFFER_INST_SNOW, L"Prototype_Component_VIBuffer_Instance_Snow");
WNAME_TAG(PROTO_COM_VIBUFFER_INST_EXPLOSION, L"Prototype_Component_VIBuffer_Instance_Explosion");
WNAME_TAG(PROTO_COM_MODEL_FIONA, L"Prototype_Component_Model_Fiona");
WNAME_TAG(PROTO_COM_MODEL_FORKLIFT, L"Prototype_Component_Model_ForkLift");

WNAME_TAG(PROTO_COM_MODEL_PM0001_00, L"Prototype_Component_Model_ÀÌ»óÇØ¾¾");
WNAME_TAG(PROTO_COM_MODEL_PM0004_00, L"Prototype_Component_Model_ÆÄÀÌ¸®");
WNAME_TAG(PROTO_COM_MODEL_PM0007_00, L"Prototype_Component_Model_²¿ºÎ±â");
WNAME_TAG(PROTO_COM_MODEL_PM0025_00, L"Prototype_Component_Model_ÇÇÄ«Ãò");

WNAME_TAG(PROTO_COM_MODEL_HERO, L"Prototype_Component_Model_Hero");

WNAME_TAG(PROTO_COM_MODEL_TOWN01, L"Prototype_Component_Model_Town01");
WNAME_TAG(PROTO_COM_MODEL_ROAD01, L"Prototype_Component_Model_Road01");

// Navigation & Collider
WNAME_TAG(PROTO_COM_NAVIGATION_TERRAIN, L"Prototype_Component_Navigation_Terrain");
WNAME_TAG(PROTO_COM_NAVIGATION_MAP, L"Prototype_Component_Navigation_Map");
WNAME_TAG(PROTO_COM_COLLIDER_AABB, L"Prototype_Component_Collider_AABB");
WNAME_TAG(PROTO_COM_COLLIDER_OBB, L"Prototype_Component_Collider_OBB");
WNAME_TAG(PROTO_COM_COLLIDER_SPHERE, L"Prototype_Component_Collider_Sphere");

// Layer
WNAME_TAG(LAYER_BACKGROUND, L"Layer_BackGround");
WNAME_TAG(LAYER_CAMERA, L"Layer_Camera");
WNAME_TAG(LAYER_PLAYER, L"Layer_Player");
WNAME_TAG(LAYER_MONSTER, L"Layer_Monster");
WNAME_TAG(LAYER_EFFECT, L"Layer_Effect");
WNAME_TAG(LAYER_UI, L"Layer_UI");

// Object
WNAME_TAG(PROTO_OBJ_BACKGROUND, L"Prototype_GameObject_BackGround");
WNAME_TAG(PROTO_OBJ_TERRAIN, L"Prototype_GameObject_Terrain");
WNAME_TAG(PROTO_OBJ_CAMERA_FREE, L"Prototype_GameObject_Camera_Free");
WNAME_TAG(PROTO_OBJ_MONSTER, L"Prototype_GameObject_Monster");
WNAME_TAG(PROTO_OBJ_FORKLIFT, L"Prototype_GameObject_ForkLift");
WNAME_TAG(PROTO_OBJ_PLAYER, L"Prototype_GameObject_Player");
WNAME_TAG(PROTO_OBJ_BODY_PLAYER, L"Prototype_GameObject_Body_Player");
WNAME_TAG(PROTO_OBJ_WEAPON, L"Prototype_GameObject_Weapon");
WNAME_TAG(PROTO_OBJ_SKY, L"Prototype_GameObject_Sky");
WNAME_TAG(PROTO_OBJ_SNOW, L"Prototype_GameObject_Snow");
WNAME_TAG(PROTO_OBJ_EXPLOSION, L"Prototype_GameObject_Explosion");

WNAME_TAG(PROTO_OBJ_PLAYER_LGPE, L"Prototype_GameObject_Player_LGPE");
WNAME_TAG(PROTO_OBJ_BODY_HERO, L"Prototype_GameObject_Body_Hero");
WNAME_TAG(PROTO_OBJ_TOWN01, L"Prototype_MapObject_Town01");
WNAME_TAG(PROTO_OBJ_ROAD01, L"Prototype_MapObject_Road01");

WNAME_TAG(PROTO_UI_CONTAINER, L"Prototype_UIContainer");
WNAME_TAG(PROTO_UI_IMAGE, L"Prototype_UIImage");
WNAME_TAG(PROTO_UI_TEXT, L"Prototype_UIText");
WNAME_TAG(PROTO_UI_BUTTON, L"Prototype_UIButton");
WNAME_TAG(PROTO_UI_PROGRESSBAR, L"Prototype_UIProgressBar");

// Component
WNAME_TAG(COM_TEXTURE_DIFF, L"Com_Texture_Diffuse");
WNAME_TAG(COM_TEXTURE_MASK, L"Com_Texture_Mask");
WNAME_TAG(COM_TEXTURE_BRUSH, L"Com_Texture_Brush");


// Part
WNAME_TAG(PART_BODY, L"Part_Body");
WNAME_TAG(PART_WEAPON, L"Part_Weapon");

// Font
WNAME_TAG(FONT_MALGUN, L"Font_Malgun");
WNAME_TAG(FONT_NANUMBARUNGOTHIC, L"Font_NanumBarunGothic");
WNAME_TAG(FONT_NOTOSANSKR, L"Font_NotoSansKR");

NS_END

#endif // Game_PKM_Tags_h__
#ifndef Engine_Tags_h__
#define Engine_Tags_h__

namespace Engine
{
	WNAME_TAG(COM_TRANSFORM, L"Com_Transform");
	WNAME_TAG(COM_VIBUFFER, L"Com_VIBuffer");
	WNAME_TAG(COM_TEXTURE, L"Com_Texture");
	WNAME_TAG(COM_TEXTURE_BACK, L"Com_Texture_Back");
	WNAME_TAG(COM_TEXTURE_FILL, L"Com_Texture_Fill");
	WNAME_TAG(COM_SHADER, L"Com_Shader");
	WNAME_TAG(COM_MODEL, L"Com_Model");
	WNAME_TAG(COM_NAVIGATION, L"Com_Navigation");
	WNAME_TAG(COM_COLLIDER_AABB, L"Com_Collider_AABB");
	WNAME_TAG(COM_COLLIDER_OBB, L"Com_Collider_OBB");
	WNAME_TAG(COM_COLLIDER_SPHERE, L"Com_Collider_Sphere");
	WNAME_TAG(COM_UI_ANIMATOR, L"Com_UI_Animator");



	WNAME_TAG(TARGET_DIFFUSE, L"Target_Diffuse");
	WNAME_TAG(TARGET_NORMAL, L"Target_Normal");
	WNAME_TAG(TARGET_DEPTH, L"Target_Depth");
	WNAME_TAG(TARGET_SPECULAR, L"Target_Specular");
	WNAME_TAG(TARGET_AMBIENT, L"Target_Ambient");
	WNAME_TAG(TARGET_SHADE, L"Target_Shade");
	WNAME_TAG(TARGET_PICKPOS, L"Target_PickPos");
	WNAME_TAG(TARGET_LIGHTDEPTH, L"Target_LightDepth");



	WNAME_TAG(MRT_GAMEOBJECTS, L"MRT_GameObjects");
	WNAME_TAG(MRT_LIGHTACC, L"MRT_LightAcc");
	WNAME_TAG(MRT_SHADOWOBJECTS, L"MRT_ShadowObjects");



	WNAME_TAG(PROTO_UI_CONTAINER, L"Prototype_UIContainer");
	WNAME_TAG(PROTO_UI_SEQUENCE, L"Prototype_UISequence");
	WNAME_TAG(PROTO_UI_IMAGE, L"Prototype_UIImage");
	WNAME_TAG(PROTO_UI_TEXT, L"Prototype_UIText");
	WNAME_TAG(PROTO_UI_BUTTON, L"Prototype_UIButton");
	WNAME_TAG(PROTO_UI_PROGRESSBAR, L"Prototype_UIProgressBar");
}

#endif // Engine_Tags_h__
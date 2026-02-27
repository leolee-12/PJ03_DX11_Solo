#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{	
	enum class WINMODE { FULL, WIN, END };
	enum class LIGHT { DIRECTIONAL, POINT, END };
	enum class D3DTS { VIEW, PROJECTION, END };
	enum class MODELTYPE { ANIM, NONANIM };
	enum class RENDERGROUP { PRIORITY, SHADOW, NONBLEND, NONLIGHT, BLEND, UI, END };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	enum class MOUSEKEYSTATE{ LB, RB, WHEEL, XB, END };
	enum class MOUSEMOVESTATE { X, Y, WHEEL, END };
	enum class POINT { A, B, C, END };
	enum class LINE { AB, BC, CA, END };
	enum class COLLIDER { AABB, OBB, SPHERE, END };
	enum class CORNER { LT, RT, RB, LB, END };
	enum class NEIGHBOR { LEFT, TOP, RIGHT, BOTTOM, END };
	

}
#endif // Engine_Enum_h__

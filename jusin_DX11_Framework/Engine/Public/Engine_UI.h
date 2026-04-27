#ifndef Engine_UI_h__
#define Engine_UI_h__

namespace Engine
{
	enum class UI_ANCHOR
	{
		TL, TC, TR,
		ML, MC, MR,
		BL, BC, BR,
		END
	};

	enum class UI_LAYOUT { NONE, CANVAS, HORIZONTAL, VERTICAL, OVERLAY, END };

	enum class UI_TYPE { WIDGET, CONTAINER, IMAGE, TEXT, BUTTON, PROGRESSBAR, END };

	enum class UI_TEXT_ALIGN { LEFT, CENTER, RIGHT, END };

	enum class UI_PROGRESS_DIR { LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM, BOTTOM_TO_TOP, END };

	enum class UI_EASE : unsigned int
	{
		LINEAR,
		EASE_IN_SINE, EASE_OUT_SINE, EASE_IN_OUT_SINE,
		EASE_IN_QUAD, EASE_OUT_QUAD, EASE_IN_OUT_QUAD,
		EASE_IN_CUBIC, EASE_OUT_CUBIC, EASE_IN_OUT_CUBIC,
		END
	};

	enum class UI_TWEEN_TARGET : unsigned int
	{
		SIZE_X, SIZE_Y,
		ROTATION,
		POSITION_X, POSITION_Y,
		COLOR_R, COLOR_G, COLOR_B, COLOR_A,
		FILL_AMOUNT,
		ANCHOR_OFFSET_X, ANCHOR_OFFSET_Y,
		BACK_COLOR_R, BACK_COLOR_G, BACK_COLOR_B, BACK_COLOR_A,
		END
	};

	enum class UI_TWEEN_LOOP : unsigned int { NONE, LOOP, PINGPONG, END };

	enum class UI_SEQ_STEP_KIND : unsigned int
	{
		PLAY_ANIM,		// target->Get_Animator()->Play_Animation(AnimName, target)
		SET_VISIBLE,	// target->Set_Visible(bVisible)
		WAIT,			// fWaitSec 경과 대기
		USE_CALLBACK,	// fnCallback 호출
		END
	};

	namespace detail
	{
		template <typename E>
		struct EnumStringPair { E e; const char* s; };

		inline constexpr EnumStringPair<UI_LAYOUT> kUILayout[] = {
			{ UI_LAYOUT::NONE,			"NONE"			},
			{ UI_LAYOUT::CANVAS,		"CANVAS"		},
			{ UI_LAYOUT::HORIZONTAL,	"HORIZONTAL"	},
			{ UI_LAYOUT::VERTICAL,		"VERTICAL"		},
			{ UI_LAYOUT::OVERLAY,		"OVERLAY"		},
		};

		inline constexpr EnumStringPair<UI_TYPE> kUIType[] = {
			{ UI_TYPE::WIDGET,		"WIDGET"		},
			{ UI_TYPE::CONTAINER,	"CONTAINER"		},
			{ UI_TYPE::IMAGE,		"IMAGE"			},
			{ UI_TYPE::TEXT,		"TEXT"			},
			{ UI_TYPE::BUTTON,		"BUTTON"		},	
			{ UI_TYPE::PROGRESSBAR,	"PROGRESSBAR"	},
		};

		inline constexpr EnumStringPair<UI_PROGRESS_DIR> kProgressDir[] = {
			{ UI_PROGRESS_DIR::LEFT_TO_RIGHT, "LEFT_TO_RIGHT" },
			{ UI_PROGRESS_DIR::RIGHT_TO_LEFT, "RIGHT_TO_LEFT" },
			{ UI_PROGRESS_DIR::TOP_TO_BOTTOM, "TOP_TO_BOTTOM" },
			{ UI_PROGRESS_DIR::BOTTOM_TO_TOP, "BOTTOM_TO_TOP" },
		};

		inline constexpr EnumStringPair<UI_TWEEN_TARGET> kTweenTarget[] = {
			{ UI_TWEEN_TARGET::SIZE_X,          "SIZE_X"          },
			{ UI_TWEEN_TARGET::SIZE_Y,          "SIZE_Y"          },
			{ UI_TWEEN_TARGET::ROTATION,        "ROTATION"        },
			{ UI_TWEEN_TARGET::POSITION_X,      "POSITION_X"      },
			{ UI_TWEEN_TARGET::POSITION_Y,      "POSITION_Y"      },
			{ UI_TWEEN_TARGET::COLOR_R,         "COLOR_R"         },
			{ UI_TWEEN_TARGET::COLOR_G,         "COLOR_G"         },
			{ UI_TWEEN_TARGET::COLOR_B,         "COLOR_B"         },
			{ UI_TWEEN_TARGET::COLOR_A,         "COLOR_A"         },
			{ UI_TWEEN_TARGET::FILL_AMOUNT,     "FILL_AMOUNT"     },
			{ UI_TWEEN_TARGET::ANCHOR_OFFSET_X, "ANCHOR_OFFSET_X" },
			{ UI_TWEEN_TARGET::ANCHOR_OFFSET_Y, "ANCHOR_OFFSET_Y" },
			{ UI_TWEEN_TARGET::BACK_COLOR_R,    "BACK_COLOR_R"    },
			{ UI_TWEEN_TARGET::BACK_COLOR_G,    "BACK_COLOR_G"    },
			{ UI_TWEEN_TARGET::BACK_COLOR_B,    "BACK_COLOR_B"    },
			{ UI_TWEEN_TARGET::BACK_COLOR_A,    "BACK_COLOR_A"    },
		};

		inline constexpr EnumStringPair<UI_EASE> kEase[] = {
			{ UI_EASE::LINEAR,            "LINEAR"            },
			{ UI_EASE::EASE_IN_SINE,      "EASE_IN_SINE"      },
			{ UI_EASE::EASE_OUT_SINE,     "EASE_OUT_SINE"     },
			{ UI_EASE::EASE_IN_OUT_SINE,  "EASE_IN_OUT_SINE"  },
			{ UI_EASE::EASE_IN_QUAD,      "EASE_IN_QUAD"      },
			{ UI_EASE::EASE_OUT_QUAD,     "EASE_OUT_QUAD"     },
			{ UI_EASE::EASE_IN_OUT_QUAD,  "EASE_IN_OUT_QUAD"  },
			{ UI_EASE::EASE_IN_CUBIC,     "EASE_IN_CUBIC"     },
			{ UI_EASE::EASE_OUT_CUBIC,    "EASE_OUT_CUBIC"    },
			{ UI_EASE::EASE_IN_OUT_CUBIC, "EASE_IN_OUT_CUBIC" },
		};

		inline constexpr EnumStringPair<UI_TWEEN_LOOP> kTweenLoop[] = {
			{ UI_TWEEN_LOOP::NONE,     "NONE"     },
			{ UI_TWEEN_LOOP::LOOP,     "LOOP"     },
			{ UI_TWEEN_LOOP::PINGPONG, "PINGPONG" },
		};

		inline constexpr EnumStringPair<UI_SEQ_STEP_KIND> kStepKind[] = {
			{ UI_SEQ_STEP_KIND::PLAY_ANIM,    "PLAY_ANIM"    },
			{ UI_SEQ_STEP_KIND::SET_VISIBLE,  "SET_VISIBLE"  },
			{ UI_SEQ_STEP_KIND::WAIT,         "WAIT"         },
			{ UI_SEQ_STEP_KIND::USE_CALLBACK, "USE_CALLBACK" },
		};

		inline constexpr EnumStringPair<UI_ANCHOR> kAnchor[] = {
			{ UI_ANCHOR::TL, "TL" }, { UI_ANCHOR::TC, "TC" }, { UI_ANCHOR::TR, "TR" },
			{ UI_ANCHOR::ML, "ML" }, { UI_ANCHOR::MC, "MC" }, { UI_ANCHOR::MR, "MR" },
			{ UI_ANCHOR::BL, "BL" }, { UI_ANCHOR::BC, "BC" }, { UI_ANCHOR::BR, "BR" },
		};

		inline constexpr EnumStringPair<UI_TEXT_ALIGN> kTextAlign[] = {
			{ UI_TEXT_ALIGN::LEFT,   "LEFT"   },
			{ UI_TEXT_ALIGN::CENTER, "CENTER" },
			{ UI_TEXT_ALIGN::RIGHT,  "RIGHT"  },
		};

		template <typename E, size_t N>
		inline const char* Enum_To_String(E e, const EnumStringPair<E>(&table)[N])
		{
			for (const auto& p : table)
				if (p.e == e) return p.s;
			return "END";
		}

		template <typename E, size_t N>
		inline E Enum_From_String(const char* psz, const EnumStringPair<E>(&table)[N], E eFallback)
		{
			if (nullptr == psz) return eFallback;
			for (const auto& p : table)
				if (0 == std::strcmp(p.s, psz)) return p.e;
			return eFallback;
		}
	}

	inline const char* To_String(UI_TYPE e) { return detail::Enum_To_String(e, detail::kUIType); }
	inline const char* To_String(UI_LAYOUT e) { return detail::Enum_To_String(e, detail::kUILayout); }
	inline const char* To_String(UI_PROGRESS_DIR e) { return detail::Enum_To_String(e, detail::kProgressDir); }
	inline const char* To_String(UI_TWEEN_TARGET e) { return detail::Enum_To_String(e, detail::kTweenTarget); }
	inline const char* To_String(UI_EASE e) { return detail::Enum_To_String(e, detail::kEase); }
	inline const char* To_String(UI_TWEEN_LOOP e) { return detail::Enum_To_String(e, detail::kTweenLoop); }
	inline const char* To_String(UI_SEQ_STEP_KIND e) { return detail::Enum_To_String(e, detail::kStepKind); }
	inline const char* To_String(UI_ANCHOR e) { return detail::Enum_To_String(e, detail::kAnchor); }
	inline const char* To_String(UI_TEXT_ALIGN e) { return detail::Enum_To_String(e, detail::kTextAlign); }

	inline UI_TYPE			UI_TYPE_From_String(const char* s) { return detail::Enum_From_String(s, detail::kUIType, UI_TYPE::END); }
	inline UI_LAYOUT		UI_LAYOUT_From_String(const char* s) { return detail::Enum_From_String(s, detail::kUILayout, UI_LAYOUT::END); }
	inline UI_PROGRESS_DIR	UI_PROGRESS_DIR_From_String(const char* s) { return detail::Enum_From_String(s, detail::kProgressDir, UI_PROGRESS_DIR::END); }
	inline UI_TWEEN_TARGET  UI_TWEEN_TARGET_From_String(const char* s) { return detail::Enum_From_String(s, detail::kTweenTarget, UI_TWEEN_TARGET::END); }
	inline UI_EASE          UI_EASE_From_String(const char* s) { return detail::Enum_From_String(s, detail::kEase, UI_EASE::END); }
	inline UI_TWEEN_LOOP    UI_TWEEN_LOOP_From_String(const char* s) { return detail::Enum_From_String(s, detail::kTweenLoop, UI_TWEEN_LOOP::END); }
	inline UI_SEQ_STEP_KIND UI_SEQ_STEP_KIND_From_String(const char* s) { return detail::Enum_From_String(s, detail::kStepKind, UI_SEQ_STEP_KIND::END); }
	inline UI_ANCHOR        UI_ANCHOR_From_String(const char* s) { return detail::Enum_From_String(s, detail::kAnchor, UI_ANCHOR::END); }
	inline UI_TEXT_ALIGN    UI_TEXT_ALIGN_From_String(const char* s) { return detail::Enum_From_String(s, detail::kTextAlign, UI_TEXT_ALIGN::END); }



	struct UIANCHOR_DESC
	{
		UI_ANCHOR eAnchor = { UI_ANCHOR::MC };
		float fOffsetX = {};
		float fOffsetY = {};
		bool bUseAnchoredPos = { false };
	};

	struct UILAYOUT_DESC
	{
		UI_LAYOUT eLayout = { UI_LAYOUT::NONE };
		float fPadding = {};
		float fSpacing = {};
	};

	struct UILAYOUT_SLOT_DESC
	{
		XMFLOAT4 vMargin = {};	// (left, top, right, bottom)
		float fDesiredSizeX = {};
		float fDesiredSizeY = {};
	};

	struct UI_SHARED_TEXTURE_BINDING_DESC
	{
		string strRole;
		string strSharedTexName;
		string strShaderVarName;
		unsigned int iTextureIndex = { static_cast<unsigned int>(-1) };
	};
}

#endif // Engine_UI_h__
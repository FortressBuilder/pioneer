// Copyright © 2008-2016 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Expand.h"
#include "LuaObject.h"

namespace UI {

class LuaExpand {
public:

};

}

using namespace UI;

template <> const char *LuaObject<UI::Expand>::s_type = "UI.Expand";

template <> void LuaObject<UI::Expand>::RegisterClass()
{
	static const char *l_parent = "UI.Single";

	static const luaL_Reg l_methods[] = {

		{ 0, 0 }
	};

	LuaObjectBase::CreateClass(s_type, l_parent, l_methods, 0, 0);
	LuaObjectBase::RegisterPromotion(l_parent, s_type, LuaObject<UI::Expand>::DynamicCastPromotionTest);
}

#pragma once
#include "lua.hpp"

namespace VV2
{
	class LuaLibrary
	{
	public:
		LuaLibrary();

		virtual void Register(lua_State* state) = 0;
	};
}
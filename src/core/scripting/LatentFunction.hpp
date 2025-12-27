#pragma once
#include "LuaScript.hpp"

namespace VV2::Lua
{
	int LatentFunctionInternal(lua_State* state, lua_CFunction func);

	// a latent function is a function that can yield
	template<lua_CFunction function>
	int LatentFunction(lua_State* state)
	{
		return LatentFunctionInternal(state, function);
	}
}
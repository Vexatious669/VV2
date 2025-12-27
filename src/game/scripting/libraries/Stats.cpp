#include "core/scripting/LuaLibrary.hpp"
#include "core/scripting/LuaScript.hpp"
#include "core/scripting/LuaUtils.hpp"
#include "game/gta/Stats.hpp"

namespace VV2::Lua
{
	class Stats : LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

		static int SetInt(lua_State* state)
		{
			VV2::Stats::SetInt(CheckStringSafe(state, 1), luaL_checkinteger(state, 2));
			return 0;
		}

		static int SetBool(lua_State* state)
		{
			VV2::Stats::SetBool(CheckStringSafe(state, 1), CheckBooleanSafe(state, 2));
			return 0;
		}

		static int SetFloat(lua_State* state)
		{
			VV2::Stats::SetFloat(CheckStringSafe(state, 1), luaL_checknumber(state, 2));
			return 0;
		}

		// SetDate

		static int SetString(lua_State* state)
		{
			VV2::Stats::SetString(CheckStringSafe(state, 1), CheckStringSafe(state, 2));
			return 0;
		}

		static int SetPackedInt(lua_State* state)
		{
			VV2::Stats::SetPackedInt(luaL_checkinteger(state, 1), luaL_checkinteger(state, 2));
			return 0;
		}

		static int SetPackedBool(lua_State* state)
		{
			VV2::Stats::SetPackedBool(luaL_checkinteger(state, 1), CheckBooleanSafe(state, 2));
			return 0;
		}

		static int SetPackedBoolRange(lua_State* state)
		{
			int start = luaL_checkinteger(state, 1);
			int end = luaL_checkinteger(state, 2);
			bool value = CheckBooleanSafe(state, 3);
			for (int i = start; i <= end; i++)
				VV2::Stats::SetPackedBool(i, value);
			return 0;
		}

		static int GetInt(lua_State* state)
		{
			lua_pushinteger(state, VV2::Stats::GetInt(CheckStringSafe(state, 1)));
			return 0;
		}

		static int GetBool(lua_State* state)
		{
			lua_pushboolean(state, VV2::Stats::GetBool(CheckStringSafe(state, 1)));
			return 0;
		}

		static int GetFloat(lua_State* state)
		{
			lua_pushnumber(state, VV2::Stats::GetFloat(CheckStringSafe(state, 1)));
			return 0;
		}

		// GetDate

		static int GetString(lua_State* state)
		{
			lua_pushstring(state, VV2::Stats::GetString(CheckStringSafe(state, 1)));
			return 0;
		}

		static int GetPackedInt(lua_State* state)
		{
			lua_pushinteger(state, VV2::Stats::GetPackedInt(luaL_checkinteger(state, 1)));
			return 0;
		}

		static int GetPackedBool(lua_State* state)
		{
			lua_pushboolean(state, VV2::Stats::GetPackedBool(luaL_checkinteger(state, 1)));
			return 0;
		}

		virtual void Register(lua_State* state) override
		{
			lua_newtable(state);
			SetFunction(state, SetInt, "set_int");
			SetFunction(state, SetBool, "set_bool");
			SetFunction(state, SetFloat, "set_float");
			SetFunction(state, SetString, "set_string");
			SetFunction(state, SetPackedInt, "set_packed_int");
			SetFunction(state, SetPackedBool, "set_packed_bool");
			SetFunction(state, SetPackedBoolRange, "set_packed_bool_range");
			SetFunction(state, GetInt, "get_int");
			SetFunction(state, GetBool, "get_bool");
			SetFunction(state, GetFloat, "get_float");
			SetFunction(state, GetString, "get_string");
			SetFunction(state, GetPackedInt, "get_packed_int");
			SetFunction(state, GetPackedBool, "get_packed_bool");
			lua_setglobal(state, "stats");
		}
	};

	Stats _Stats;
}
#include "LuaLibrary.hpp"
#include "LuaManager.hpp"

namespace VV2
{
	LuaLibrary::LuaLibrary()
	{
		LuaManager::RegisterLibrary(this);
	}
}
#include "LuaResource.hpp"
#include "LuaManager.hpp"

namespace VV2
{
	void LuaResourceType::Register()
	{
		m_Index = LuaManager::RegisterResourceType(this);
	}
}
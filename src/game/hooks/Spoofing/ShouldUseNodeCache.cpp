#include "core/hooking/DetourHook.hpp"
#include "game/backend/NodeHooks.hpp"
#include "game/hooks/Hooks.hpp"

namespace VV2::Hooks
{
	bool Hooks::Spoofing::ShouldUseNodeCache(void* node, int flags)
	{
		if (NodeHooks::ShouldSkipNodeCache(reinterpret_cast<CProjectBaseSyncDataNode*>(node)))
			return false;

		return BaseHook::Get<Spoofing::ShouldUseNodeCache, DetourHook<decltype(&Spoofing::ShouldUseNodeCache)>>()->Original()(node, flags);
	}
}
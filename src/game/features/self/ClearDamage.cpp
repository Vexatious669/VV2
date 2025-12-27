#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"

namespace VV2::Features
{
	class ClearDamage : Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			Self::GetPed().ClearDamage();
		}
	};

	static ClearDamage _ClearDamage{"cleardamage", "Clear Damage", "Clears all the dirt and blood on you."};
}
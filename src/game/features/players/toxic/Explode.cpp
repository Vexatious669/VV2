#include "game/commands/PlayerCommand.hpp"
#include "types/fx/ExplosionType.hpp"

namespace VV2::Features
{
	class Explode : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (auto ped = player.GetPed())
				ped.Explode(ExplosionType::BLIMP, 900.f);
		}
	};

	static Explode _Explode{"explode", "Explode", "Blows the player up"};
}
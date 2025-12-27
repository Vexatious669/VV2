#include "Onboarding.hpp"
#include "GUI.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/BoolCommand.hpp"
#include "game/backend/AnticheatBypass.hpp"
#include "game/pointers/Pointers.hpp"
#include <shellapi.h>

namespace VV2
{
	static BoolCommand _OnboardingComplete{"$onboardingcomplete", "", ""};

	void ProcessOnboarding()
	{
		if (_OnboardingComplete.GetState())
			return;

		static bool ensure_popup_open = [] {
			ImGui::OpenPopup("IMPORTANT! PLEASE READ!");
			GUI::SetOnboarding(true);
			return true;
		}();

		const auto window_size = ImVec2{700, 500};
		const auto window_position = ImVec2{(*Pointers.ScreenResX - window_size.x) / 2, (*Pointers.ScreenResY - window_size.y) / 2};

		ImGui::SetNextWindowSize(window_size, ImGuiCond_Once);
		ImGui::SetNextWindowPos(window_position, ImGuiCond_Once);

		if (ImGui::BeginPopupModal("IMPORTANT! PLEASE READ!", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::TextWrapped("%s",
			    "Welcome to VV2! You can press INSERT or Ctrl+\\ to open the menu. With the introduction of BattlEye, the ability to join and stay in public sessions has been severely limited. "
			    "You have an option to play only with other VV2 users, or you can choose to connect to regular BattlEye-protected sessions. "
			    "You will automatically be kicked out of regular sessions in under three minutes, and you may be temporarily blacklisted from "
			    "joining for up to two days, even after re-enabling BattlEye");
			static int value = 0;
			ImGui::RadioButton("Play with VV2 users", &value, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Play with everyone (Broken!)", &value, 1);
			ImGui::TextWrapped("%s",
			    "You can always change your choice by toggling Network > Spoofing > Join VV2-only Sessions. Our official repository is at "
			    "https://github.com/Vexatious669/VV2. Make sure to only download the menu from GitHub to avoid malware. "
			    "You can use the repository to report bugs, suggest features, and contribute by making pull requests. ");
			if (ImGui::Button("Open VV2 Git Github"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/Vexatious669/VV2", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::SameLine();
			if (ImGui::Button("Open OG Github"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/YimMenu/YimMenuV2", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::TextWrapped("%s",
			    "Check for updates reguarly; But high-key, mess around with VV2!");
			if (ImGui::Button("Close"))
			{
				Commands::GetCommand<BoolCommand>("cheaterpool"_J)->SetState(!value);
				_OnboardingComplete.SetState(true);
				GUI::SetOnboarding(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
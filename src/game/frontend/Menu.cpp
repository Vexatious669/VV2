// Menu.cpp
// Responsible for initializing the UI, setting ImGui style and loading fonts.
// Heavily commented to help reviewers understand intent and behaviour.

#include "Menu.hpp"

#include "imgui.h"
#include "imgui_internal.h"

// Core subsystems used by the menu
#include "core/commands/Commands.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "core/renderer/Renderer.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"

// Fonts and pointers
#include "game/frontend/fonts/Fonts.hpp"
#include "game/pointers/Pointers.hpp"

// Top-level submenu implementations (each provides categories/items)
#include "submenus/Self.hpp"
#include "submenus/Vehicle.hpp"
#include "submenus/Teleport.hpp"
#include "submenus/Network.hpp"
#include "submenus/Players.hpp"
#include "submenus/Recovery.hpp"
#include "submenus/Settings.hpp"
#include "submenus/Debug.hpp"
#include "submenus/World.hpp"

// Utilities used during initialization / file paths
#include "core/filemgr/FileMgr.hpp"
#include "core/memory/ModuleMgr.hpp"
#include "Onboarding.hpp"

namespace VV2
{
	// Initialize menu subsystem:
	//  - register submenus with UIManager
	//  - register a renderer callback which draws the UI each frame
	void Menu::Init()
	{
		// Register available submenus. This is the canonical place to add new top-level
		// submenus for the UI; UIManager will own and lay them out when drawing.
		UIManager::AddSubmenu(std::make_shared<Submenus::Self>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Vehicle>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Teleport>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Network>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Players>());
		UIManager::AddSubmenu(std::make_shared<Submenus::World>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Recovery>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Settings>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Debug>());

		// Register a renderer callback. Renderer will call this every frame (priority -1).
		// The lambda:
		//  - processes onboarding UI state,
		//  - checks whether GUI is open,
		//  - sets the font and a temporary window background colour,
		//  - triggers UIManager::Draw which renders the full UI,
		//  - then restores ImGui state (pop style/font).
		Renderer::AddRendererCallBack(
		    [&] {
			    ProcessOnboarding();

			    // If GUI is currently closed (toggle), don't draw anything.
			    if (!GUI::IsOpen())
				    return;

			    // Push our custom font for the menu.
			    ImGui::PushFont(Menu::Font::g_DefaultFont);

			    // Temporarily override the Window background colour for this draw pass.
			    // We push a solid black here; the style definition contains the canonical
			    // colours but this override enforces a strict bg for the main window.
			    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImU32(ImColor(0, 0, 0)));

			    // Let UIManager draw the entire UI hierarchy (submenus, categories, items).
			    UIManager::Draw();

			    // Restore ImGui style/font state.
			    ImGui::PopStyleColor();
			    ImGui::PopFont();
		    },
		    -1);
	}

	// SetupStyle configures the ImGuiStyle used through the UI.
	// Keep this function focused on colour, rounding and other visual defaults.
	void Menu::SetupStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		// ---------------------------------------------------------------------
		// Text
		// ---------------------------------------------------------------------
		// Primary and disabled text colours (RGBA as floats).
		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);           // #FFFFFF - main text
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);   // #808080 - disabled text

		// ---------------------------------------------------------------------
		// Backgrounds
		// ---------------------------------------------------------------------
		// Main window background, child panel backgrounds and popup background.
		// Values chosen to create a charcoal -> dark-gray hierarchy.
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);       // #000000 - window background
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);        // #141414 - inner panels
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.07f, 0.07f, 0.95f);        // #171212 - popups (slightly translucent)

		// ---------------------------------------------------------------------
		// Borders / frames
		// ---------------------------------------------------------------------
		// Border tone and input field background.
		style.Colors[ImGuiCol_Border] = ImVec4(0.28f, 0.28f, 0.28f, 0.25f);         // #474747 - subtle border
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.17f, 0.15f, 0.15f, 1.00f);        // #2b2626 - fields / frames

		// Frame hover/active colours — part of the accent family.
		// We converted the previous green accents to a red family (hover/active slightly different).
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.459f, 0.114f, 0.114f, 0.75f); // #751d1d - frame hover (alpha 0.75)
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.451f, 0.267f, 0.267f, 0.85f);  // #734444 - frame active

		// ---------------------------------------------------------------------
		// Buttons (accent colours)
		// ---------------------------------------------------------------------
		// Base / hover / active states for buttons. Keep these consonant with frame accents.
		style.Colors[ImGuiCol_Button] = ImVec4(0.459f, 0.114f, 0.114f, 1.00f);       // #751d1d - base button (red)
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.412f, 0.251f, 0.251f, 1.00f); // #694040 - hover
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.580f, 0.149f, 0.164f, 1.00f);  // #94262a - pressed

		// ---------------------------------------------------------------------
		// Title bars
		// ---------------------------------------------------------------------
		// Title bar backgrounds (collapsed / active / default)
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0706f, 0.0588f, 0.0588f, 1.00f);   // #120f0f
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1098f, 0.0902f, 0.0902f, 1.00f); // #1c1717
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.051f, 0.039f, 0.039f, 1.00f);   // #0d0a0a

		// ---------------------------------------------------------------------
		// Tabs
		// ---------------------------------------------------------------------
		// Tabs on the top of content windows. We use red-leaning colours to match buttons.
		style.Colors[ImGuiCol_Tab] = ImVec4(0.349f, 0.192f, 0.196f, 0.85f);         // #593132 - base tab
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.416f, 0.231f, 0.231f, 0.85f);  // #6a3b3b - hovered
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.592f, 0.168f, 0.184f, 1.00f);   // #972b2f - active

		// ---------------------------------------------------------------------
		// Headers (selectable list headers, group headings)
		// ---------------------------------------------------------------------
		// Visible when using Selectable/CollapsingHeader styled items.
		style.Colors[ImGuiCol_Header] = ImVec4(0.353f, 0.180f, 0.180f, 1.00f);      // #5a2e2e
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.431f, 0.227f, 0.227f, 1.00f);// #6e3a3a
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.560f, 0.176f, 0.184f, 1.00f); // #8f2d2f

		// ---------------------------------------------------------------------
		// Plot / histogram colours
		// ---------------------------------------------------------------------
		// Plots use a subtle red palette with a bright hover highlight for readability.
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.439f, 0.251f, 0.251f, 0.25f);    // #704040 (subtle)
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.439f, 0.439f, 1.00f); // #FF7070 - highlight
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.416f, 0.184f, 0.184f, 1.00f); // #6a2f2f
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.580f, 0.149f, 0.164f, 1.00f); // #94262a

		// ---------------------------------------------------------------------
		// Rounding and general shape
		// ---------------------------------------------------------------------
		// Use gentle rounding on frames/windows to match the original look.
		style.GrabRounding = style.FrameRounding = style.ChildRounding = style.WindowRounding = 8.0f;
	}

	// Return a glyph range array containing only Cyrillic ranges used for merging
	// with a Latin font. This keeps the embedded main font compact while providing
	// Cyrillic coverage from a system font.
	static const ImWchar* GetGlyphRangesCyrillicOnly()
	{
		static const ImWchar ranges[] =
		    {
		        0x0400,
		        0x052F, // Cyrillic + Cyrillic Supplement
		        0x2DE0,
		        0x2DFF, // Cyrillic Extended-A
		        0xA640,
		        0xA69F, // Cyrillic Extended-B
		        0,
		    };
		return &ranges[0];
	}

	// Load the main fonts used by the UI and ensure Cyrillic glyphs are available.
	// We embed a primary font (stored in Fonts::MainFont) and merge Arial for Cyrillic.
	static ImFont* CreateFontWithCyrillicSupport(ImGuiIO& io, float size)
	{
		ImFontConfig FontCfg{};
		FontCfg.FontDataOwnedByAtlas = false; // Font data isn't owned by ImGui atlas; we supply a pointer.

		// Add the embedded main font from memory. This keeps the binary self-contained.
		auto font = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), size, &FontCfg, io.Fonts->GetGlyphRangesDefault());

		// Merge Arial for Cyrillic ranges only. This prevents duplicating the full Arial set.
		FontCfg.MergeMode = true;
		io.Fonts->AddFontFromFileTTF((std::filesystem::path(std::getenv("SYSTEMROOT")) / "Fonts" / "arial.ttf").string().c_str(), size, &FontCfg, GetGlyphRangesCyrillicOnly());

		// Rebuild atlas (AddFontFromMemoryTTF/AddFontFromFileTTF queue fonts; Build ensures they are compiled).
		io.Fonts->Build();

		return font;
	}

	// SetupFonts configures ImGui IO font files and paths. The imgui.ini location
	// is placed under %appdata%/VV2 so user-specific settings (window positions etc.)
	// persist between sessions.
	void Menu::SetupFonts()
	{
		auto& IO = ImGui::GetIO();

		// Compose the path used for ImGui's .ini file (persisted layout).
		auto file_path = std::filesystem::path(std::getenv("appdata")) / "VV2" / "imgui.ini";
		static auto path = file_path.string();
		IO.IniFilename = path.c_str();

		// Disable ImGui log file generation (not used).
		IO.LogFilename = NULL;

		ImFontConfig FontCfg{};
		FontCfg.FontDataOwnedByAtlas = false;

		// Start with a clean slate for fonts.
		IO.Fonts->Clear();

		// Create and store commonly used fonts at configured sizes.
		Menu::Font::g_DefaultFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_DefaultFontSize);
		Menu::Font::g_OptionsFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_OptionsFontSize);
		Menu::Font::g_ChildTitleFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_ChildTitleFontSize);
		Menu::Font::g_ChatFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_ChatFontSize);
		Menu::Font::g_OverlayFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_OverlayFontSize);

		// Add the icon font (Font Awesome or similar) for UI icons; we use the full glyph range.
		static const ImWchar full_range[] = {0x0020, 0xFFFF, 0};
		Menu::Font::g_AwesomeFont = IO.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::IconFont), sizeof(Fonts::IconFont), Menu::Font::g_AwesomeFontSize, &FontCfg, full_range);

		// Inform UIManager which font to use for options rendering and notify the renderer
		// that fonts were updated (so cached font glyph atlases are refreshed).
		UIManager::SetOptionsFont(Menu::Font::g_OptionsFont);
		Renderer::SetFontsUpdated();
	}
}

#include "includes.h"
#include "mem.h"
#include "font_awesome.h"
#include "font.h"
#include "BackgroundGeometry.h"
#include "CustomSlider.h"
#include "CustomCheckbox.h"
#include "MenuBackground.h"
#include "helpmarker.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <algorithm>
#include <list>
#include <memory>
#include <type_traits>
#include <vector>
#include <utility>
#include <iterator>
#include <functional>
#include <cstdint>
#include <cstddef>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Game Version Detection Code
namespace FullGameSDK {
    class UObject;
    class UWorld;
    class UClass;
}

namespace Beta1SDK {
    class UObject;
    class UWorld;
    class UClass;
}

namespace Alpha4SDK {
    class UObject;
    class UWorld;
    class UClass;
}

namespace FullGameSDK {
    using namespace std;
    #include "Full-Game-SDK/SDK.hpp"
}

namespace Beta1SDK {
    using namespace std;
    #include "Beta-1-SDK/BasicSDK.hpp"
}

namespace Alpha4SDK {
    using namespace std;
    #include "Alpha-4-SDK/BasicSDK.hpp"
}

namespace Offsets {
	namespace FullGame {
		constexpr auto GOBJECTS = 47602432;
		constexpr auto APPENDSTRING = 6967488;
		constexpr auto GNAMES = 51231584;
		constexpr auto GWORLD = 52292480;
		constexpr auto PROCESSEVENT = 8557840;
		constexpr auto INDEX_PROCESSEVENT = 65;
	}
	namespace Beta1 {
		constexpr auto GOBJECTS = 52348144;
		constexpr auto APPENDSTRING = 3466464;
		constexpr auto GNAMES = 52316248;
		constexpr auto GWORLD = 52366640;
		constexpr auto PROCESSEVENT = 4408032;
		constexpr auto INDEX_PROCESSEVENT = 62;
	}
	namespace Alpha4 {
		constexpr auto GOBJECTS = 48521248;
		constexpr auto APPENDSTRING = 2857888;
		constexpr auto GNAMES = 48280824;
		constexpr auto GWORLD = 48592336;
		constexpr auto PROCESSEVENT = 4137456;
		constexpr auto INDEX_PROCESSEVENT = 58;
	}
}

uintptr_t ModuleBase = 0;
uintptr_t GObjects = 0;
uintptr_t GNames = 0;
uintptr_t GWorld = 0;
uintptr_t ProcessEvent = 0;
void** GObjectsArray = nullptr;
void** GNamesArray = nullptr;
void** GWorldPtr = nullptr;

enum class GameVersion { UNKNOWN, FULL_GAME, ALPHA_4, BETA_1 };

static GameVersion currentGameVersion = GameVersion::UNKNOWN;

GameVersion DetectGameVersion() {
	char processName[MAX_PATH];
	GetModuleFileNameA(NULL, processName, MAX_PATH);
	std::string exePath(processName);
	std::transform(exePath.begin(), exePath.end(), exePath.begin(), ::tolower);

	if (exePath.find("helloneighborreborn") != std::string::npos) {
		HMODULE hModule = GetModuleHandleA("HelloNeighborReborn-Win64-Shipping.exe");
		if (hModule) {
			uintptr_t alpha4Addr = (uintptr_t)hModule + Offsets::Alpha4::GOBJECTS;
			if (!IsBadReadPtr((void*)alpha4Addr, sizeof(void*))) {
				uintptr_t gobjectsValue = *(uintptr_t*)alpha4Addr;
				if (gobjectsValue != 0) {
					uintptr_t gnamesAddr = (uintptr_t)hModule + Offsets::Alpha4::GNAMES;
					if (!IsBadReadPtr((void*)gnamesAddr, sizeof(void*))) {
						uintptr_t gnamesValue = *(uintptr_t*)gnamesAddr;
						if (gnamesValue != 0) return GameVersion::ALPHA_4;
					}
				}
			}
		}
	}
	else if (exePath.find("helloneighbor") != std::string::npos) {
		HMODULE hModule = GetModuleHandleA("HelloNeighbor-Win64-Shipping.exe");
		if (hModule) {
			uintptr_t fullGameAddr = (uintptr_t)hModule + Offsets::FullGame::GOBJECTS;
			if (!IsBadReadPtr((void*)fullGameAddr, sizeof(void*))) {
				uintptr_t gobjectsValue = *(uintptr_t*)fullGameAddr;
				if (gobjectsValue != 0) {
					uintptr_t gnamesAddr = (uintptr_t)hModule + Offsets::FullGame::GNAMES;
					if (!IsBadReadPtr((void*)gnamesAddr, sizeof(void*))) {
						uintptr_t gnamesValue = *(uintptr_t*)gnamesAddr;
						if (gnamesValue != 0) return GameVersion::FULL_GAME;
					}
				}
			}
			uintptr_t beta1Addr = (uintptr_t)hModule + Offsets::Beta1::GOBJECTS;
			if (!IsBadReadPtr((void*)beta1Addr, sizeof(void*))) {
				uintptr_t gobjectsValue = *(uintptr_t*)beta1Addr;
				if (gobjectsValue != 0) {
					uintptr_t gnamesAddr = (uintptr_t)hModule + Offsets::Beta1::GNAMES;
					if (!IsBadReadPtr((void*)gnamesAddr, sizeof(void*))) {
						uintptr_t gnamesValue = *(uintptr_t*)gnamesAddr;
						if (gnamesValue != 0) return GameVersion::BETA_1;
					}
				}
			}
		}
	}
	return GameVersion::UNKNOWN;
}

bool InitializeSDK(GameVersion version) {
	try {
		const char* moduleName = (version == GameVersion::ALPHA_4) ?
			"HelloNeighborReborn-Win64-Shipping.exe" :
			"HelloNeighbor-Win64-Shipping.exe";

		ModuleBase = (uintptr_t)GetModuleHandleA(moduleName);
		if (!ModuleBase) return false;

		switch (version) {
		case GameVersion::FULL_GAME:
			GObjects = ModuleBase + Offsets::FullGame::GOBJECTS;
			GNames = ModuleBase + Offsets::FullGame::GNAMES;
			GWorld = ModuleBase + Offsets::FullGame::GWORLD;
			ProcessEvent = ModuleBase + Offsets::FullGame::PROCESSEVENT;
			break;
		case GameVersion::BETA_1:
			GObjects = ModuleBase + Offsets::Beta1::GOBJECTS;
			GNames = ModuleBase + Offsets::Beta1::GNAMES;
			GWorld = ModuleBase + Offsets::Beta1::GWORLD;
			ProcessEvent = ModuleBase + Offsets::Beta1::PROCESSEVENT;
			break;
		case GameVersion::ALPHA_4:
			GObjects = ModuleBase + Offsets::Alpha4::GOBJECTS;
			GNames = ModuleBase + Offsets::Alpha4::GNAMES;
			GWorld = ModuleBase + Offsets::Alpha4::GWORLD;
			ProcessEvent = ModuleBase + Offsets::Alpha4::PROCESSEVENT;
			break;
		default: return false;
		}

		GWorldPtr = reinterpret_cast<void**>(GWorld);
		GObjectsArray = reinterpret_cast<void**>(GObjects);
		GNamesArray = reinterpret_cast<void**>(GNames);

		return (GObjects && GNames && GWorld && ProcessEvent &&
			GObjectsArray && GNamesArray && GWorldPtr);
	}
	catch (...) { return false; }
}

void HandleGameVersion(GameVersion version) {
	currentGameVersion = version;
	switch (version) {
	case GameVersion::FULL_GAME:
		if (InitializeSDK(version)) std::cout << "Full Game SDK initialized" << std::endl;
		break;
	case GameVersion::BETA_1:
		if (InitializeSDK(version)) std::cout << "Beta 1 SDK initialized" << std::endl;
		break;
	case GameVersion::ALPHA_4:
		if (InitializeSDK(version)) std::cout << "Alpha 4 SDK initialized" << std::endl;
		break;
	default:
		std::cout << "Unknown Hello Neighbor version" << std::endl;
		break;
	}
}

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 7.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 4.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(5.0f, 2.0f);
	style.FrameRounding = 3.0f;
	style.FrameBorderSize = 1.0f;
	style.ItemSpacing = ImVec2(6.0f, 6.0f);
	style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
	style.IndentSpacing = 25.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 1.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1030042767524719f, 0.1030032485723495f, 0.1030032485723495f, 0.9356223344802856f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.9200000166893005f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.2899999916553497f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.239999994635582f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3600000143051147f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.3499999940395355f);

	ImFontConfig font;
	font.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF((void*)rawData, sizeof(rawData), 14.5f, &font);
	ImFontConfig iconConfig;
	iconConfig.MergeMode = true;
	iconConfig.PixelSnapH = true;
	iconConfig.OversampleH = 3;
	iconConfig.OversampleV = 3;
	iconConfig.FontDataOwnedByAtlas = true;
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 15.f, &iconConfig, icon_ranges);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	if (GetAsyncKeyState(VK_HOME) & 1) {
		globals::show = !globals::show;
	}
	if (globals::show)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImVec2 windowSize = ImGui::GetIO().DisplaySize;
		float width = windowSize.x;
		float height = windowSize.y;
		static std::vector<Point> points = initializePoints(NUM_POINTS, width, height);
		static bool enableEffect = true;
		static bool enableNetwork = true;
		static float pointSize = 1.0f;
		static float lineThickness = 1.0f;
		static ImVec4 customColor = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);
		static bool useRGB = false;
		ImGui::SetNextWindowSize(ImVec2(900, 450));
		ImGui::Begin(("NeighborTools by @maybekoi"), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImVec2 menuPos = ImGui::GetWindowPos();
		ImVec2 menuSize = ImGui::GetWindowSize();
		if (!initialized)
		{
			InitializeParticles(menuSize);
			initialized = true;
		}
		{
			ImGui::SameLine();
			if (ImGui::Button(("\xef\x95\x8c Options"), ImVec2(70, 20)))
			{
				globals::tab = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button(("\xef\x87\xa2 Misc"), ImVec2(70, 20)))
			{
				globals::tab = 1;
			}
			ImGui::SameLine();
			if (ImGui::Button(("\xef\x82\x85 Settings"), ImVec2(80, 20)))
			{
				globals::tab = 2;
			}
		}
		ImGui::Separator();
		if (globals::tab == 0)
		{
			if (ImGui::InputDouble(("coins"), &globals::coins))
			{
				// nan
			}
		}
		if (globals::tab == 0)
		{
			if (ImGui::InputDouble(("ultracoins"), &globals::ultracoins))
			{
				// nan
			}
		}

		if (globals::tab == 2)
		{
			ImGui::Text("Geometry Settings");
			if (CustomCheckbox("Effekt aktivieren", &enableEffect)) {}
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 90.f);
			if (ImGui::ColorEdit4("##GeometryColor1", (float*)&customColor, ImGuiColorEditFlags_NoInputs)) {}
			if (CustomCheckbox("Use RGB Color", &useRGB)) {}
			if (CustomSliderFloat("Point Size", &pointSize, 1.0f, 10.0f)) {}
			if (CustomSliderFloat("Line Thickness", &lineThickness, 0.5f, 5.0f)) {}
			if (CustomCheckbox("Enable Particles", &enableParticles));
			if (CustomSliderFloat("Particle Size Multiplier", &particleSizeMultiplier, 0.5f, 3.0f));
			if (CustomSliderFloat("Particle Speed Multiplier", &particleSpeedMultiplier, 0.5f, 3.0f));
		}

		float availableHeight = ImGui::GetContentRegionAvail().y;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + availableHeight - ImGui::GetTextLineHeight());
		ImVec4 textColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		const char* gameVersionStr = "UNKNOWN VERSION";
		switch (currentGameVersion) {
			case GameVersion::FULL_GAME:
				gameVersionStr = "FULL GAME";
				break;
			case GameVersion::BETA_1:
				gameVersionStr = "BETA 1";
				break;
			case GameVersion::ALPHA_4:
				gameVersionStr = "ALPHA 4";
				break;
			default:
				gameVersionStr = "UNKNOWN VERSION";
				break;
		}
		ImGui::TextColored(textColor, "Menu toggle keybind: HOME | CURRENT GAME: Hello Neighbor - %s", gameVersionStr);

		if (enableEffect) {
			updatePoints(points, width, height);
			ImU32 color = useRGB ? getRGBColor() : ImGui::ColorConvertFloat4ToU32(customColor);
			ImDrawList* drawList = ImGui::GetBackgroundDrawList();
			drawNetwork(drawList, points, color, pointSize, lineThickness);
		}

		if (enableParticles)
		{
			float deltaTime = CalculateDeltaTime();
			UpdateParticles(deltaTime, menuSize);
			RenderParticles(menuPos, menuSize);
		}

		ImGui::End();
		ImGui::Render();
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		HandleGameVersion(DetectGameVersion());
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}
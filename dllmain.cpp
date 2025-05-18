#include "pch.h"
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
    // Full Game Offsets
    namespace FullGame {
        constexpr auto GOBJECTS = 47602432;
        constexpr auto APPENDSTRING = 6967488;
        constexpr auto GNAMES = 51231584;
        constexpr auto GWORLD = 52292480;
        constexpr auto PROCESSEVENT = 8557840;
        constexpr auto INDEX_PROCESSEVENT = 65;
    }

    // Beta 1 Offsets
    namespace Beta1 {
        constexpr auto GOBJECTS = 52348144;
        constexpr auto APPENDSTRING = 3466464;
        constexpr auto GNAMES = 52316248;
        constexpr auto GWORLD = 52366640;
        constexpr auto PROCESSEVENT = 4408032;
        constexpr auto INDEX_PROCESSEVENT = 62;
    }

    // Alpha 4 Offsets
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

enum class GameVersion {
    UNKNOWN,
    FULL_GAME,
    ALPHA_4,
    BETA_1
};

GameVersion DetectGameVersion() {
    char processName[MAX_PATH];
    GetModuleFileNameA(NULL, processName, MAX_PATH);
    std::string exePath(processName);
    
    std::cout << "Current executable path: " << exePath << std::endl;
    
    std::transform(exePath.begin(), exePath.end(), exePath.begin(), ::tolower);
    std::cout << "Lowercase path: " << exePath << std::endl;
    
    if (exePath.find("helloneighborreborn") != std::string::npos) {
        std::cout << "Found HelloNeighborReborn executable" << std::endl;
        HMODULE hModule = GetModuleHandleA("HelloNeighborReborn-Win64-Shipping.exe");
        if (hModule) {
            std::cout << "Got module handle for HelloNeighborReborn" << std::endl;
            std::cout << "Module base: 0x" << std::hex << (uintptr_t)hModule << std::dec << std::endl;
            
            uintptr_t alpha4Addr = (uintptr_t)hModule + Offsets::Alpha4::GOBJECTS;
            std::cout << "Checking Alpha 4 GObjects at: 0x" << std::hex << alpha4Addr << std::dec << std::endl;
            if (!IsBadReadPtr((void*)alpha4Addr, sizeof(void*))) {
                uintptr_t gobjectsValue = *(uintptr_t*)alpha4Addr;
                std::cout << "GObjects value: 0x" << std::hex << gobjectsValue << std::dec << std::endl;
                if (gobjectsValue != 0) {
                    uintptr_t gnamesAddr = (uintptr_t)hModule + Offsets::Alpha4::GNAMES;
                    if (!IsBadReadPtr((void*)gnamesAddr, sizeof(void*))) {
                        uintptr_t gnamesValue = *(uintptr_t*)gnamesAddr;
                        std::cout << "GNames value: 0x" << std::hex << gnamesValue << std::dec << std::endl;
                        if (gnamesValue != 0) {
                            std::cout << "All Alpha 4 offsets verified successfully!" << std::endl;
                            return GameVersion::ALPHA_4;
                        }
                    }
                }
            }
            std::cout << "Alpha 4 offset verification failed" << std::endl;
        }
    }
    else if (exePath.find("helloneighbor") != std::string::npos) {
        std::cout << "Found HelloNeighbor executable" << std::endl;
        HMODULE hModule = GetModuleHandleA("HelloNeighbor-Win64-Shipping.exe");
        if (hModule) {
            std::cout << "Got module handle for HelloNeighbor" << std::endl;
            std::cout << "Module base: 0x" << std::hex << (uintptr_t)hModule << std::dec << std::endl;

            uintptr_t fullGameAddr = (uintptr_t)hModule + Offsets::FullGame::GOBJECTS;
            std::cout << "Checking Full Game GObjects at: 0x" << std::hex << fullGameAddr << std::dec << std::endl;
            if (!IsBadReadPtr((void*)fullGameAddr, sizeof(void*))) {
                uintptr_t gobjectsValue = *(uintptr_t*)fullGameAddr;
                std::cout << "Full Game GObjects value: 0x" << std::hex << gobjectsValue << std::dec << std::endl;
                if (gobjectsValue != 0) {
                    uintptr_t gnamesAddr = (uintptr_t)hModule + Offsets::FullGame::GNAMES;
                    if (!IsBadReadPtr((void*)gnamesAddr, sizeof(void*))) {
                        uintptr_t gnamesValue = *(uintptr_t*)gnamesAddr;
                        std::cout << "Full Game GNames value: 0x" << std::hex << gnamesValue << std::dec << std::endl;
                        if (gnamesValue != 0) {
                            std::cout << "All Full Game offsets verified successfully!" << std::endl;
                            return GameVersion::FULL_GAME;
                        }
                    }
                }
            }
            std::cout << "Full Game offset verification failed, trying Beta 1..." << std::endl;

            uintptr_t beta1Addr = (uintptr_t)hModule + Offsets::Beta1::GOBJECTS;
            std::cout << "Checking Beta 1 GObjects at: 0x" << std::hex << beta1Addr << std::dec << std::endl;
            if (!IsBadReadPtr((void*)beta1Addr, sizeof(void*))) {
                uintptr_t gobjectsValue = *(uintptr_t*)beta1Addr;
                std::cout << "Beta 1 GObjects value: 0x" << std::hex << gobjectsValue << std::dec << std::endl;
                if (gobjectsValue != 0) {
                    uintptr_t gnamesAddr = (uintptr_t)hModule + Offsets::Beta1::GNAMES;
                    if (!IsBadReadPtr((void*)gnamesAddr, sizeof(void*))) {
                        uintptr_t gnamesValue = *(uintptr_t*)gnamesAddr;
                        std::cout << "Beta 1 GNames value: 0x" << std::hex << gnamesValue << std::dec << std::endl;
                        if (gnamesValue != 0) {
                            std::cout << "All Beta 1 offsets verified successfully!" << std::endl;
                            return GameVersion::BETA_1;
                        }
                    }
                }
            }
            std::cout << "Beta 1 offset verification failed" << std::endl;
        } else {
            std::cout << "Failed to get module handle for HelloNeighbor" << std::endl;
        }
    } else {
        std::cout << "Executable name doesn't match any known Hello Neighbor version" << std::endl;
    }
    
    std::cout << "Detection failed - returning UNKNOWN" << std::endl;
    return GameVersion::UNKNOWN;
}

bool InitializeSDK(GameVersion version) {
    try {
        const char* moduleName = (version == GameVersion::ALPHA_4) ? 
            "HelloNeighborReborn-Win64-Shipping.exe" : 
            "HelloNeighbor-Win64-Shipping.exe";
            
        ModuleBase = (uintptr_t)GetModuleHandleA(moduleName);
        if (!ModuleBase) {
            std::cout << "Failed to get module base address for " << moduleName << std::endl;
            return false;
        }
        std::cout << "Module Base: 0x" << std::hex << ModuleBase << std::dec << std::endl;

        switch (version) {
            case GameVersion::FULL_GAME:
                GObjects = ModuleBase + Offsets::FullGame::GOBJECTS;
                GNames = ModuleBase + Offsets::FullGame::GNAMES;
                GWorld = ModuleBase + Offsets::FullGame::GWORLD;
                ProcessEvent = ModuleBase + Offsets::FullGame::PROCESSEVENT;
                GWorldPtr = reinterpret_cast<void**>(GWorld);
                break;
            case GameVersion::BETA_1:
                GObjects = ModuleBase + Offsets::Beta1::GOBJECTS;
                GNames = ModuleBase + Offsets::Beta1::GNAMES;
                GWorld = ModuleBase + Offsets::Beta1::GWORLD;
                ProcessEvent = ModuleBase + Offsets::Beta1::PROCESSEVENT;
                GWorldPtr = reinterpret_cast<void**>(GWorld);
                break;
            case GameVersion::ALPHA_4:
                GObjects = ModuleBase + Offsets::Alpha4::GOBJECTS;
                GNames = ModuleBase + Offsets::Alpha4::GNAMES;
                GWorld = ModuleBase + Offsets::Alpha4::GWORLD;
                ProcessEvent = ModuleBase + Offsets::Alpha4::PROCESSEVENT;
                GWorldPtr = reinterpret_cast<void**>(GWorld);
                break;
            default:
                std::cout << "Unsupported game version for SDK initialization" << std::endl;
                return false;
        }

        if (!GObjects || !GNames || !GWorld || !ProcessEvent) {
            std::cout << "Failed to initialize SDK offsets" << std::endl;
            return false;
        }

        std::cout << "GObjects: 0x" << std::hex << GObjects << std::dec << std::endl;
        std::cout << "GNames: 0x" << std::hex << GNames << std::dec << std::endl;
        std::cout << "GWorld: 0x" << std::hex << GWorld << std::dec << std::endl;
        std::cout << "ProcessEvent: 0x" << std::hex << ProcessEvent << std::dec << std::endl;

        GObjectsArray = reinterpret_cast<void**>(GObjects);
        GNamesArray = reinterpret_cast<void**>(GNames);

        if (!GObjectsArray || !GNamesArray || !GWorldPtr) {
            std::cout << "Failed to initialize SDK globals" << std::endl;
            return false;
        }

        if (version == GameVersion::FULL_GAME) {
            auto world = reinterpret_cast<FullGameSDK::UWorld**>(GWorldPtr);
            if (*world) {
                std::cout << "Successfully found Full Game World object" << std::endl;
            }
        } else if (version == GameVersion::BETA_1) {
            auto world = reinterpret_cast<Beta1SDK::UWorld**>(GWorldPtr);
            if (*world) {
                std::cout << "Successfully found Beta 1 World object" << std::endl;
            }
        } else if (version == GameVersion::ALPHA_4) {
            auto world = reinterpret_cast<Alpha4SDK::UWorld**>(GWorldPtr);
            if (*world) {
                std::cout << "Successfully found Alpha 4 World object" << std::endl;
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cout << "SDK initialization failed: " << e.what() << std::endl;
    }
    return false;
}

void HandleGameVersion(GameVersion version) {
    switch (version) {
        case GameVersion::FULL_GAME:
            std::cout << "Hello Neighbor - Full Game" << std::endl;
            if (InitializeSDK(version)) {
                std::cout << "Full Game SDK initialized successfully" << std::endl;
            }
            break;
        case GameVersion::BETA_1:
            std::cout << "Hello Neighbor - Beta 1" << std::endl;
            if (InitializeSDK(version)) {
                std::cout << "Beta 1 SDK initialized successfully" << std::endl;
            }
            break;
        case GameVersion::ALPHA_4:
            std::cout << "Hello Neighbor - Alpha 4" << std::endl;
            if (InitializeSDK(version)) {
                std::cout << "Alpha 4 SDK initialized successfully" << std::endl;
            }
            break;
        default:
            std::cout << "Unknown Hello Neighbor version" << std::endl;
            std::cout << "You're either on a unsupported alpha/beta or you're NOT on Hello Neighbor!" << std::endl;
            break;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        
        SetConsoleTitle(L"NeighborTools - Random Shit I make for HN made by @maybekoi");
        
        HandleGameVersion(DetectGameVersion());
        break;
        
    case DLL_PROCESS_DETACH:
        FreeConsole();
        break;
        
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}


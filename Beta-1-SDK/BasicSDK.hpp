#pragma once
#include <cstdint>

namespace SDK {
    using FName = int32_t;
    using FString = wchar_t*;
    using FScriptDelegate = void*;
    using FScriptMulticastDelegate = void*;
    using UObject = void*;
    using UClass = void*;
    using UFunction = void*;
    using UWorld = void*;
    using UStruct = void*;
    using UEnum = void*;
    using UProperty = void*;
}

namespace Offsets {
    constexpr auto GOBJECTS = 0x031EC4F0;
    constexpr auto GNAMES = 0x031E4858;
    constexpr auto GWORLD = 0x031F0D30;
    constexpr auto PROCESSEVENT = 0x004342E0;
    constexpr auto INDEX_PROCESSEVENT = 62;
}

namespace SDK {
    struct FUObjectItem {
        UObject* Object;
        int32_t Flags;
        int32_t ClusterIndex;
        int32_t SerialNumber;
    };

    struct FNameEntry {
        uint16_t bIsWide : 1;
        uint16_t LowercaseProbeHash : 5;
        uint16_t Len : 10;
        union {
            char AnsiName[1024];
            wchar_t WideName[1024];
        };
    };

    struct TUObjectArray {
        enum { ElementsPerChunk = 64 * 1024 };
        FUObjectItem** Objects;
        FUObjectItem* PreAllocatedObjects;
        int32_t MaxElements;
        int32_t NumElements;
        int32_t MaxChunks;
        int32_t NumChunks;
    };

    struct FNamePool {
        FNameEntry* Entries[0x10000];
    };
}

extern SDK::TUObjectArray* GObjects;
extern SDK::FNamePool* GNames;
extern SDK::UWorld** GWorld;
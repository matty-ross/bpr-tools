#include "Dispatcher.h"

#include <cstdlib>
#include <ws2tcpip.h>


Dispatcher::Dispatcher()
{
    WSADATA data = {};
    [[maybe_unused]] const int result = WSAStartup(MAKEWORD(2, 2), &data);
}

Dispatcher::~Dispatcher()
{
    if (mAnalyzerSocket != INVALID_SOCKET)
        closesocket(mAnalyzerSocket);

    WSACleanup();
}

void Dispatcher::CreateAnalyzerSocket(const char* host, uint16_t port)
{
    mAnalyzerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, host, &address.sin_addr.s_addr);

    connect(mAnalyzerSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}

void Dispatcher::ForwardData(const uint8_t* event_data, int32_t event_type, uint32_t event_size)
{
#pragma warning(disable: 4200) // warning C4200: nonstandard extension used: zero-sized array in struct/union
    struct alignas(4) EventData
    {
        int32_t Type;
        uint32_t Size;
        uint8_t Data[];
    };
#pragma warning(default: 4200)
    
    // allocate memory for the packet
    const size_t data_size = sizeof(EventData) + event_size;
    EventData* const data = static_cast<EventData*>(malloc(data_size));
    if (data == nullptr)
        return;
    
    // build the packet and send it
    data->Type = event_type;
    data->Size = event_size;
    memcpy_s(data->Data, event_size, event_data, event_size);
    send(mAnalyzerSocket, reinterpret_cast<const char*>(data), data_size, 0);
    
    free(data);
}


void __stdcall ProcessGameEventsDetour(const uint8_t* event_data, int32_t event_type, uint32_t event_size)
{
    gDispatcher->ForwardData(event_data, event_type, event_size);
}

__declspec(naked) void ProcessGameEventsDetourThunk()
{
    __asm
    {
        // save context
        pushfd
        pushad

        // call the detour
        push [esi - 0xC]
        push ecx
        push esi
        call ProcessGameEventsDetour

        // restore context and jump back to the original code
        popad
        popfd
        jmp dword ptr [ecx * 4 + 0x00A28E10]
    }
}

void Dispatcher::HookProcessGameEvents()
{
    static constexpr uintptr_t HOOK_ADDRESS = 0x00A254DD;
    static constexpr size_t HOOK_SIZE = 7;

    // change memory page protection
    DWORD old_protection = 0;
    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDRESS), HOOK_SIZE, PAGE_EXECUTE_READWRITE, &old_protection);

    // install a detour
    *reinterpret_cast<uint8_t*>(HOOK_ADDRESS + 0x0) = 0xE9;
    *reinterpret_cast<uintptr_t*>(HOOK_ADDRESS + 0x1) = reinterpret_cast<uintptr_t>(ProcessGameEventsDetourThunk) - HOOK_ADDRESS - 0x5;
    memset(reinterpret_cast<void*>(HOOK_ADDRESS + 0x5), 0x90, HOOK_SIZE - 5);

    // restore memory page protection
    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDRESS), HOOK_SIZE, old_protection, &old_protection);
}
#include "Dispatcher.h"
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

void Dispatcher::ForwardData(int32_t event_type, uint32_t event_size, const uint8_t* event_data)
{
    const size_t data_size = sizeof(int32_t) + sizeof(uint32_t) + event_size;
    uint8_t* const data = new uint8_t[data_size];

    *reinterpret_cast<int32_t*>(data + 0x0) = event_type;
    *reinterpret_cast<uint32_t*>(data + 0x4) = event_size;
    memcpy_s(data + 0x8, event_size, event_data, event_size);
    send(mAnalyzerSocket, reinterpret_cast<const char*>(data), data_size, 0);

    delete[] data;
}


void __stdcall ProcessGameEventsDetour(const uint8_t* event_data, int32_t event_type, uint32_t event_size)
{
    gDispatcher->ForwardData(event_type, event_size, event_data);
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
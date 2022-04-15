#include "ProxyClient.h"
#include <ws2tcpip.h>

ProxyClient::ProxyClient()
{
    WSADATA data = {};
    [[maybe_unused]] const int result = WSAStartup(MAKEWORD(2, 2), &data);
}

ProxyClient::~ProxyClient()
{
    if (mRecvSocket != INVALID_SOCKET)
        closesocket(mRecvSocket);
    if (mSendSocket != INVALID_SOCKET)
        closesocket(mSendSocket);
    
    WSACleanup();
}

void ProxyClient::CreateRecvSocket(const char* host, uint16_t port)
{
    mRecvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, host, &address.sin_addr.s_addr);
    
    connect(mRecvSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}

void ProxyClient::CreateSendSocket(const char* host, uint16_t port)
{
    mSendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, host, &address.sin_addr.s_addr);
    
    connect(mSendSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}

void ProxyClient::ForwardRecvData(const void* data, size_t size)
{
    send(mRecvSocket, reinterpret_cast<const char*>(data), size, 0);
}

void ProxyClient::ForwardSendData(const void* data, size_t size)
{
    send(mSendSocket, reinterpret_cast<const char*>(data), size, 0);
}


void __stdcall SendHandlerDetour(const void* data, uint32_t size)
{
    gProxyClient->ForwardSendData(data, size);
}

__declspec(naked) void SendHandlerDetourThunk()
{
    __asm
    {
        // save context
        pushfd
        pushad

        // call the detour
        push [ebp + 0x10]
        push [ebp + 0xC]
        call SendHandlerDetour

        // restore context
        popad
        popfd

        // original code
        push [ebp + 0x20]
        mov esi, [ebp + 0x8]

        // jump back to the original code
        push 0x03A0CB6A
        ret
    }
}

void ProxyClient::HookSendHandler()
{
    static constexpr uintptr_t HOOK_ADDRESS = 0x03A0CB64;
    static constexpr size_t HOOK_SIZE = 6;

    // change memory page protection
    DWORD old_protection = 0;
    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDRESS), HOOK_SIZE, PAGE_EXECUTE_READWRITE, &old_protection);

    *reinterpret_cast<uint8_t*>(HOOK_ADDRESS) = 0xE9;
    *reinterpret_cast<uintptr_t*>(HOOK_ADDRESS + 0x1) = reinterpret_cast<uintptr_t>(SendHandlerDetourThunk) - HOOK_ADDRESS - 0x5;
    memset(reinterpret_cast<void*>(HOOK_ADDRESS + 0x5), 0x90, HOOK_SIZE - 5);

    // restore memory page protection
    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDRESS), HOOK_SIZE, old_protection, &old_protection);
}

void __stdcall RecvHandlerDetour(const void* data, uint32_t size)
{
    gProxyClient->ForwardRecvData(data, size);
}

__declspec(naked) void RecvHandlerDetourThunk()
{
    __asm
    {
        // save context
        pushfd
        pushad

        // call the detour
        push [ebp + 0x14]
        push ebx
        call RecvHandlerDetour

        // restore context
        popad
        popfd

        // original code
        add esp, 0x10
        test eax, eax

        // jump back to the original code
        push 0x03A0D0B6
        ret
    }
}

void ProxyClient::HookRecvHandler()
{
    static constexpr uintptr_t HOOK_ADDRESS = 0x03A0D0B1;
    static constexpr size_t HOOK_SIZE = 5;

    // change memory page protection
    DWORD old_protection = 0;
    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDRESS), HOOK_SIZE, PAGE_EXECUTE_READWRITE, &old_protection);

    *reinterpret_cast<uint8_t*>(HOOK_ADDRESS) = 0xE9;
    *reinterpret_cast<uintptr_t*>(HOOK_ADDRESS + 0x1) = reinterpret_cast<uintptr_t>(RecvHandlerDetourThunk) - HOOK_ADDRESS - 0x5;

    // restore memory page protection
    VirtualProtect(reinterpret_cast<void*>(HOOK_ADDRESS), HOOK_SIZE, old_protection, &old_protection);
}
#include "ProxyClient.h"


static constexpr const char* HOST = "127.0.0.1";
static constexpr uint16_t RECV_PORT = 11111;
static constexpr uint16_t SEND_PORT = 22222;


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        gProxyClient = new ProxyClient();
        gProxyClient->CreateRecvSocket(HOST, RECV_PORT);
        gProxyClient->CreateSendSocket(HOST, SEND_PORT);
        gProxyClient->HookRecvHandler();
        gProxyClient->HookSendHandler();
        break;

    case DLL_PROCESS_DETACH:
        delete gProxyClient;
        break;
    }

    return TRUE;
}
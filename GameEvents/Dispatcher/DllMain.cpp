#include "Dispatcher.h"

static constexpr const char* HOST = "127.0.0.1";
static constexpr uint16_t PORT = 33333;

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
        gDispatcher = new Dispatcher();
        gDispatcher->CreateAnalyzerSocket(HOST, PORT);
        gDispatcher->HookProcessGameEvents();
        break;

    case DLL_PROCESS_DETACH:
        delete gDispatcher;
        break;
    }

    return TRUE;
}
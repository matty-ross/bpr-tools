#pragma once

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class Dispatcher
{
public:
    Dispatcher();
    ~Dispatcher();

public:
    void CreateAnalyzerSocket(const char* host, uint16_t port);
    void ForwardData(int32_t event_type, uint32_t event_size, const uint8_t* event_data);
    void HookProcessGameEvents();

private:
    SOCKET mAnalyzerSocket = INVALID_SOCKET;
};

inline Dispatcher* gDispatcher = nullptr;
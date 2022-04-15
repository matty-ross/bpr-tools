#pragma once

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class ProxyClient
{
public:
    ProxyClient();
    ~ProxyClient();

public:
    void CreateRecvSocket(const char* host, uint16_t port);
    void CreateSendSocket(const char* host, uint16_t port);
    void ForwardRecvData(const void* data, size_t size);
    void ForwardSendData(const void* data, size_t size);
    void HookSendHandler();
    void HookRecvHandler();

private:
    SOCKET mRecvSocket = INVALID_SOCKET;
    SOCKET mSendSocket = INVALID_SOCKET;
};

inline ProxyClient* gProxyClient = nullptr;
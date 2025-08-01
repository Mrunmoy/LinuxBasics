#pragma once

#include <cstdint>

class TCPServer
{
public:
    TCPServer(uint16_t port);
    ~TCPServer();

    void start();

private:
    int m_serverFD;
    uint16_t m_port;

    void setupSocket();
    void handleClient(int clientFD);
};

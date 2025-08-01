#pragma once

#include <cstdint>
#include <unordered_map>

class TcpServerEpoll
{
public:
    TcpServerEpoll(uint16_t port);
    ~TcpServerEpoll();

    void start();

private:
    int m_serverFD;
    int m_epollFD;
    uint16_t m_port;
    std::unordered_map<int, bool> m_clientFDs;

    void setupSocket();
    void handleNewConnection();
    void handleClientMessage(int clientFD);
};

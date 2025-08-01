#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <set>

class TcpServerSelect
{
public:
    TcpServerSelect(uint16_t port);
    ~TcpServerSelect();

    void start();

private:
    int m_serverFD;
    uint16_t m_port;
    std::set<int> m_clientFDs;

    void setupSocket();
    void handleNewConnection();
    void handleClientMessage(int clientFD);
    void broadcastMessage(int senderFD, const char* msg, size_t len);
};

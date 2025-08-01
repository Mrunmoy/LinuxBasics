#pragma once

#include <cstddef>
#include <cstdint>
#include <poll.h>
#include <vector>

class TcpServerPoll
{
public:
    TcpServerPoll(uint16_t port);
    ~TcpServerPoll();

    void start();

private:
    int m_serverFD;
    uint16_t m_port;
    std::vector<pollfd> m_pollFDs;

    void setupSocket();
    void handleNewConnection();
    void handleClientMessage(size_t index);
    void removeClientAt(size_t index);
};

#pragma once

#include <string>

class TCPClient
{
public:
    TCPClient(const std::string& serverIP, uint16_t port);
    ~TCPClient();

    void connectAndChat();

private:
    std::string m_serverIP;
    uint16_t m_port;
    int m_socketFD;

    void setupConnection();
};

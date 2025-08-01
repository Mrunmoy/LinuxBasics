#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "TCPClient.h"

namespace
{
    const std::string TAG = "TCPClient";
}

TCPClient::TCPClient(const std::string& serverIP, uint16_t port)
    : m_serverIP(serverIP)
    , m_port(port)
    , m_socketFD(-1)
{
    setupConnection();
}

TCPClient::~TCPClient()
{
    if (m_socketFD >= 0)
    {
        close(m_socketFD);
        std::cout << TAG << "🧹 Client socket closed.\n";
    }
}

void TCPClient::setupConnection()
{
    m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFD < 0)
    {
        throw std::runtime_error(TAG + "Socket creation failed");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_port);

    if (inet_pton(AF_INET, m_serverIP.c_str(), &serverAddr.sin_addr) <= 0)
    {
        throw std::runtime_error(TAG + "Invalid server address");
    }

    if (connect(m_socketFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error(TAG + "Connection to server failed");
    }

    std::cout << TAG << "🔗 Connected to server at " << m_serverIP << ":" << m_port << "\n";
}

void TCPClient::connectAndChat()
{
    // Receive server message
    char buffer[1024] = {0};
    int bytes = read(m_socketFD, buffer, sizeof(buffer) - 1);
    if (bytes > 0)
    {
        std::cout << TAG << "📥 Server says: " << buffer;
    }

    // Send message to server
    std::string msg = "Hi server, this is a C++ client!\n";
    write(m_socketFD, msg.c_str(), msg.size());
}

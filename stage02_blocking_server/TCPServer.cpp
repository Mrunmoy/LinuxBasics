#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "TCPServer.h"

namespace
{
    const std::string TAG = "TCPServer";
}

TCPServer::TCPServer(uint16_t port)
    : m_serverFD(-1)
    , m_port(port)
{
    setupSocket();
}

TCPServer::~TCPServer()
{
    if (m_serverFD >= 0)
    {
        close(m_serverFD);
        std::cout << TAG << "🧹 Server socket closed.\n";
    }
}

void TCPServer::setupSocket()
{
    m_serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverFD < 0)
    {
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(m_port);

    if (bind(m_serverFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        close(m_serverFD);
        throw std::runtime_error(TAG + "Bind failed");
    }

    if (listen(m_serverFD, 5) < 0)
    {
        close(m_serverFD);
        throw std::runtime_error(TAG + "Listen failed");
    }

    std::cout << TAG << "🟢 Server is listening on port " << m_port << "...\n";
}

void TCPServer::start()
{
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    int clientFD = accept(m_serverFD, (sockaddr*)&clientAddr, &clientLen);
    if (clientFD < 0)
    {
        std::cerr << TAG << "Accept failed.\n";
        return;
    }

    std::cout << TAG << "🔌 Client connected!\n";
    handleClient(clientFD);

    close(clientFD);
    std::cout << TAG << "👋 Client disconnected.\n";
}

void TCPServer::handleClient(int clientFD)
{
    // Send welcome message
    const char* welcome = "Hello from C++ server!\n";
    write(clientFD, welcome, strlen(welcome));

    // Receive message
    char buffer[1024] = {0};
    int bytes = read(clientFD, buffer, sizeof(buffer) - 1);
    if (bytes > 0)
    {
        std::cout << TAG << "📨 Received: " << buffer;
    }
}

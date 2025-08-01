#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdexcept>
#include <csignal>
#include <vector>

#include "TcpServerEpoll.h"

static volatile sig_atomic_t g_running = 1;

void signalHandler(int)
{
    g_running = 0;
    std::cout << "\n🛑 Caught SIGINT. Server will shut down...\n";
}

TcpServerEpoll::TcpServerEpoll(uint16_t port)
    : m_serverFD(-1), m_epollFD(-1), m_port(port)
{
    setupSocket();
}

TcpServerEpoll::~TcpServerEpoll()
{
    for (const auto& [fd, _] : m_clientFDs)
    {
        close(fd);
    }
    if (m_serverFD >= 0) close(m_serverFD);
    if (m_epollFD >= 0) close(m_epollFD);
    std::cout << "🧹 Server and all client sockets closed.\n";
}

void TcpServerEpoll::setupSocket()
{
    m_serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverFD < 0) throw std::runtime_error("Socket creation failed");

    int opt = 1;
    setsockopt(m_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(m_port);

    if (bind(m_serverFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Bind failed");

    if (listen(m_serverFD, 10) < 0)
        throw std::runtime_error("Listen failed");

    // Create epoll instance
    m_epollFD = epoll_create1(0);
    if (m_epollFD < 0)
        throw std::runtime_error("epoll_create1 failed");

    // Register the server socket with epoll
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = m_serverFD;

    if (epoll_ctl(m_epollFD, EPOLL_CTL_ADD, m_serverFD, &ev) < 0)
        throw std::runtime_error("epoll_ctl ADD serverFD failed");

    std::cout << "🟢 epoll server listening on port " << m_port << "...\n";
}

void TcpServerEpoll::start()
{
    signal(SIGINT, signalHandler);
    std::cout << "📡 Entering main server loop (epoll)...\n";

    const int MAX_EVENTS = 64;
    epoll_event events[MAX_EVENTS];

    while (g_running)
    {
        int nReady = epoll_wait(m_epollFD, events, MAX_EVENTS, -1);
        if (nReady < 0)
        {
            std::cerr << "epoll_wait() failed.\n";
            break;
        }

        for (int i = 0; i < nReady; ++i)
        {
            int fd = events[i].data.fd;

            if (fd == m_serverFD)
            {
                handleNewConnection();
            }
            else if (events[i].events & EPOLLIN)
            {
                handleClientMessage(fd);
            }
        }
    }
}

void TcpServerEpoll::handleNewConnection()
{
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    int clientFD = accept(m_serverFD, (sockaddr*)&clientAddr, &clientLen);
    if (clientFD < 0)
    {
        std::cerr << "Failed to accept new client.\n";
        return;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = clientFD;

    if (epoll_ctl(m_epollFD, EPOLL_CTL_ADD, clientFD, &ev) < 0)
    {
        std::cerr << "Failed to add client FD to epoll.\n";
        close(clientFD);
        return;
    }

    m_clientFDs[clientFD] = true;

    std::cout << "👥 New client connected: FD = " << clientFD << "\n";

    const char* welcome = "👋 Welcome to the epoll() chat server!\n";
    write(clientFD, welcome, strlen(welcome));
}

void TcpServerEpoll::handleClientMessage(int clientFD)
{
    char buffer[1024] = {0};

    int bytesRead = read(clientFD, buffer, sizeof(buffer) - 1);
    if (bytesRead <= 0)
    {
        std::cout << "Client FD " << clientFD << " disconnected or error.\n";
        epoll_ctl(m_epollFD, EPOLL_CTL_DEL, clientFD, nullptr);
        close(clientFD);
        m_clientFDs.erase(clientFD);
        return;
    }

    std::cout << "📨 Received from FD " << clientFD << ": " << buffer;

    std::vector<int> toRemove;

    for (const auto& [otherFD, _] : m_clientFDs)
    {
        if (otherFD == clientFD) continue;

        int sent = write(otherFD, buffer, bytesRead);
        if (sent < 0)
        {
            std::cerr << "⚠️ Failed to send to FD " << otherFD << "\n";
            toRemove.push_back(otherFD);
        }
    }

    // Clean up after loop
    for (int fd : toRemove)
    {
        epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, nullptr);
        close(fd);
        m_clientFDs.erase(fd);
    }

}

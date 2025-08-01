#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <csignal>

#include "TcpServerPoll.h"

static volatile sig_atomic_t g_running = 1;

void signalHandler(int)
{
    g_running = 0;
    std::cout << "\n🛑 Caught SIGINT. Server will shut down...\n";
}

TcpServerPoll::TcpServerPoll(uint16_t port)
    : m_serverFD(-1), m_port(port)
{
    setupSocket();
}

TcpServerPoll::~TcpServerPoll()
{
    for (auto& pfd : m_pollFDs)
    {
        if (pfd.fd >= 0)
            close(pfd.fd);
    }
    std::cout << "🧹 Server and all client sockets closed.\n";
}

void TcpServerPoll::setupSocket()
{
    m_serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverFD < 0)
        throw std::runtime_error("Socket creation failed");

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

    std::cout << "🟢 poll() server listening on port " << m_port << "...\n";

    // Add server socket to poll list
    m_pollFDs.push_back({m_serverFD, POLLIN, 0});
}

void TcpServerPoll::start()
{
    signal(SIGINT, signalHandler);
    std::cout << "📡 Entering main server loop (poll)...\n";

    while (g_running)
    {
        // How many sockets have something to do (like a new connection or incoming data)?
        int ready = poll(m_pollFDs.data(), m_pollFDs.size(), -1); // block forever
        if (ready < 0)
        {
            std::cerr << "poll() failed.\n";
            break;
        }

        // Check if server socket is ready (index 0)
        if (m_pollFDs[0].revents & POLLIN)
        {
            handleNewConnection();
            ready--;
        }

        // Check all client sockets which had something to do
        for (size_t i = 1; i < m_pollFDs.size() && ready > 0; ++i)
        {
            if (m_pollFDs[i].revents & POLLIN)
            {
                handleClientMessage(i);
                ready--;
            }
        }
    }
}

void TcpServerPoll::handleNewConnection()
{
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    int clientFD = accept(m_serverFD, (sockaddr*)&clientAddr, &clientLen);
    if (clientFD < 0)
    {
        std::cerr << "Failed to accept new client.\n";
        return;
    }

    pollfd clientPoll{};
    clientPoll.fd = clientFD;
    clientPoll.events = POLLIN;
    clientPoll.revents = 0;

    m_pollFDs.push_back(clientPoll);
    std::cout << "👥 New client connected: FD = " << clientFD << "\n";

    const char* welcome = "👋 Welcome to the poll() chat server!\n";
    write(clientFD, welcome, strlen(welcome));
}

void TcpServerPoll::handleClientMessage(size_t index)
{
    char buffer[1024] = {0};
    int clientFD = m_pollFDs[index].fd;

    int bytesRead = read(clientFD, buffer, sizeof(buffer) - 1);
    if (bytesRead <= 0)
    {
        std::cout << "Client FD " << clientFD << " disconnected or error.\n";
        removeClientAt(index);
        return;
    }

    std::cout << "📨 Received from FD " << clientFD << ": " << buffer;

    // Broadcast to all other clients
    for (size_t i = 1; i < m_pollFDs.size(); ++i)
    {
        if (i == index) continue;

        int otherFD = m_pollFDs[i].fd;
        if (otherFD >= 0)
        {
            int sent = write(otherFD, buffer, bytesRead);
            if (sent < 0)
            {
                std::cerr << "⚠️ Failed to send to FD " << otherFD << "\n";
            }
        }
    }
}

void TcpServerPoll::removeClientAt(size_t index)
{
    int fd = m_pollFDs[index].fd;

    if (fd >= 0)
    {
        close(fd);
        std::cout << "🧹 Closed and removed client FD " << fd << "\n";
    }

    // Remove by swapping with last element (if not last) and popping
    if (index != m_pollFDs.size() - 1)
    {
        std::swap(m_pollFDs[index], m_pollFDs.back());
    }

    m_pollFDs.pop_back();
    // Note:
    // If we don't remove the last element, the next poll() call will return
    // POLLIN for this client FD, which might cause a problem if the client
    // sends messages to other clients while the server is still trying
    // to read from this client FD.
    //
    // To avoid this, we could keep track of which FD are still in use
    // and only remove them from the poll list when they are closed.
    // But for simplicity, we just remove the client FD from the poll list.
    // This approach assumes that the client is always ready to be closed
    // when it sends a message, so it won't cause any issues.
    //
    // std::vector::erase() is slow — it shifts everything
    // So we swap with the last, then pop_back() — super fast
    // poll() doesn’t care about order, so we’re free to do this
}

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <csignal>

#include "TcpServerSelect.h"
#include "Debug.h"

static volatile sig_atomic_t g_running = 1;

void signalHandler(int)
{
    g_running = 0;
    std::cout << "\n🛑 Caught SIGINT. Server will shut down...\n";
}


TcpServerSelect::TcpServerSelect(uint16_t port)
    : m_serverFD(-1), m_port(port)
{
    setupSocket();
}

TcpServerSelect::~TcpServerSelect()
{
    for (int fd : m_clientFDs)
    {
        close(fd);
    }
    if (m_serverFD >= 0)
    {
        close(m_serverFD);
    }
    std::cout << "🧹 Server and clients closed.\n";
}

void TcpServerSelect::setupSocket()
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

    std::cout << "🟢 Server is listening on port " << m_port << "...\n";
}

void TcpServerSelect::start()
{
    // Register signal handler
    signal(SIGINT, signalHandler);
    std::cout << "📡 Entering main server loop...\n";

    fd_set readSet;
    int maxFD;

    DBG("📡 Entering main server loop...");

    while (g_running)
    {
        FD_ZERO(&readSet);
        FD_SET(m_serverFD, &readSet);
        maxFD = m_serverFD;

        for (int clientFD : m_clientFDs)
        {
            FD_SET(clientFD, &readSet);
            if (clientFD > maxFD) maxFD = clientFD;
        }

        DBG("⏳ Waiting on select() for activity...");

        // Let me know when any client sends data or when a new connection is incoming.
        int ready = select(maxFD + 1, &readSet, nullptr, nullptr, nullptr);
        if (ready < 0) // 0 = timeout (but we don’t use one)
        {
            std::cerr << "select() failed.\n";
            break;
        }

        // Was the server socket (m_serverFD) one of the FDs that became ready?
        if (FD_ISSET(m_serverFD, &readSet))
        {
            handleNewConnection();
        }

        // Create a snapshot copy of the FDs so we can iterate safely
        std::vector<int> activeClients(m_clientFDs.begin(), m_clientFDs.end());

        for (int clientFD : activeClients)
        {
            if (FD_ISSET(clientFD, &readSet))
            {
                DBG("📨 Data available from client FD " + std::to_string(clientFD) + "\n");
                handleClientMessage(clientFD);
            }
        }

    }
}

void TcpServerSelect::handleNewConnection()
{
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    int clientFD = accept(m_serverFD, (sockaddr*)&clientAddr, &clientLen);
    if (clientFD < 0)
    {
        std::cerr << "Failed to accept new client.\n";
        return;
    }

    m_clientFDs.insert(clientFD);
    std::cout << "👥 New client connected. FD = " << clientFD << "\n";

    // send a welcome message
    const char* welcome = "👋 Welcome to the chat server!\n";
    write(clientFD, welcome, strlen(welcome));
}

void TcpServerSelect::handleClientMessage(int clientFD)
{
    char buffer[1024] = {0};
    int bytesRead = read(clientFD, buffer, sizeof(buffer) - 1);
    if (bytesRead <= 0)
    {
        std::cout << "Client FD " << clientFD << " disconnected or error.\n";
        close(clientFD);
        m_clientFDs.erase(clientFD);
        return;
    }

    buffer[std::min(bytesRead, 1023)] = '\0'; // Force Null-terminate the string
    std::cout << "📨 From FD " << clientFD << ": " << buffer;

    broadcastMessage(clientFD, buffer, bytesRead);
}

void TcpServerSelect::broadcastMessage(int senderFD, const char* msg, size_t len)
{
    for (int clientFD : m_clientFDs)
    {
        if (clientFD == senderFD)
            continue; // Don't echo back to sender

        int sent = write(clientFD, msg, len);
        if (sent < 0)
        {
            std::cerr << "Failed to send to FD " << clientFD << "\n";
        }
    }
}


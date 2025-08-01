#include <stdexcept>
#include <iostream>

#include "TCPClient.h"

int main(int argc, char* argv[])
{
    try
    {
        TCPClient client("127.0.0.1", 12345);
        client.connectAndChat();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
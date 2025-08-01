#include <stdexcept>
#include <iostream>

#include "TCPServer.h"

int main()
{
    try
    {
        TCPServer server(12345);
        server.start();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}

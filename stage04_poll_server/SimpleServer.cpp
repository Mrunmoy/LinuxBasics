#include <iostream>
#include <exception>

#include "TcpServerPoll.h"

int main()
{
    try
    {
        TcpServerPoll server(12345);
        server.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}

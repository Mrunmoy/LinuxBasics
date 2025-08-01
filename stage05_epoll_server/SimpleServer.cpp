#include <iostream>
#include <exception>

#include "TcpServerEpoll.h"

int main()
{
    try
    {
        TcpServerEpoll server(12345);
        server.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}

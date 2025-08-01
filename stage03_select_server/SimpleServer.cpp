#include <iostream>
#include <exception>

#include "TcpServerSelect.h"

int main()
{
    try
    {
        TcpServerSelect server(12345);
        server.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}

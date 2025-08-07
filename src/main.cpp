#include <iostream>
#include <utility>
#include <boost/asio.hpp>
#include <AsioPDS/node.h>
int main()
{
    try
    {
        boost::asio::io_context io;
        Node node(io);
        io.run();            
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what();
    }
}
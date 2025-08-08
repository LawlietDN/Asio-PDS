#include <iostream>
#include <utility>
#include <boost/asio.hpp>
#include "AsioPDS/node.h"
#include "AsioPDS/logging.h"

int main(int argc,char* argv[])
{
    int verbosity = std::count_if(argv,argv+argc,[](auto* s){ return !std::strcmp(s,"-v");});
    pdslog::init(verbosity==0?pdslog::info:verbosity==1?pdslog::debug:pdslog::trace);
    try
    {
        boost::asio::io_context io;
        Node a(io, 9102, ".pds_A");
        Node b(io, 9202, ".pds_B");
        io.run();            
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what();
    }
}
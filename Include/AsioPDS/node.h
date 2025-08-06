#include <memory>
#include <boost/asio.hpp>
#include <AsioPDS/discoveryListener.h>
class Node
{
private:
    boost::asio::io_context& io;
    std::shared_ptr<DiscoveryListener> listener_; 

public:
    Node(boost::asio::io_context& ioCtx)
    :  io(ioCtx) 
    {
        listener_ = std::make_shared<DiscoveryListener>(io);
        listener_->start();
    }
};
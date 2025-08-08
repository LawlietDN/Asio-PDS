#include <iostream>
#include <sys/socket.h> 
#include "AsioPDS/discoveryListener.h"
#include "AsioPDS/discoveryPacket.h"

DiscoveryListener::DiscoveryListener(boost::asio::io_context& io, std::function<void(DiscoveryPacket, boost::asio::ip::udp::endpoint)> onPacketReceived_)
    : socket(io)                    
    , onPacketReceived(std::move(onPacketReceived_))
{
    socket.open(boost::asio::ip::udp::v4());
    socket.set_option(boost::asio::socket_base::reuse_address(true));
#if defined(SO_REUSEPORT)
    socket.set_option(boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>(true));
#endif
    socket.bind({boost::asio::ip::udp::v4(), proto::DiscoveryPort});

        std::cout << "[Discovery] Listening on "
        << socket.local_endpoint().address() << ':'
        << socket.local_endpoint().port() << '\n';
    }

void DiscoveryListener::start()
{
    startReceive();
}


void DiscoveryListener::startReceive()
{
    auto self = shared_from_this();
    socket.async_receive_from(boost::asio::buffer(buffer), sender,
    [self](boost::system::error_code const& e, size_t const& bytes)
    {
        if(e) {
            if(e != boost::asio::error::operation_aborted)
            {
                std::cerr << "UDP error: " << e.message() << '\n';
            }
            std::cerr << "Fatal UDP error: " << e.message() << '\n';
            return; 
        }

        if(bytes == sizeof(DiscoveryPacket))
        {
            if (!self->processPacket(self->buffer, bytes))
            {
                self->startReceive();       
                return;
            }
        }
       
        self->startReceive();                
    });
    
} 



bool DiscoveryListener::processPacket(std::array<char, sizeof(DiscoveryPacket)> const& buffer,  size_t const& bytes)
{
    DiscoveryPacket packet;
    if (decodeDiscovery(std::span<const char, sizeof(DiscoveryPacket)>(buffer.data(), sizeof(DiscoveryPacket)), packet))
    {
        onPacketReceived(packet, sender);
        return true;
    }

    return false;
}

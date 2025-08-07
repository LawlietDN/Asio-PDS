#include <iostream>
#include "AsioPDS/discoveryListener.h"
#include "AsioPDS/discoveryPacket.h"

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
        else
        {
            std::cout << bytes << '\n';
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

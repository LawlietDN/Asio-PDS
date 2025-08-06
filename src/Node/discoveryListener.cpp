#include <AsioPDS/discoveryListener.h>
#include <iostream>
void DiscoveryListener::start()
{
    startReceive();
}


void DiscoveryListener::startReceive()
{
    auto self = shared_from_this();
    socket.async_receive_from(boost::asio::buffer(buffer), sender,
    [self](boost::system::error_code const& e, std::size_t const& bytesReceived)
    {
        if(e)
        {
            std::cerr << "UDP error: " << e.message() << '\n';
        }
        if (!self->isPacketValid(self->buffer, bytesReceived)) {
            self->startReceive();       
            return;
        }

        self->processPacket(self->buffer, bytesReceived);

        self->startReceive();                
    });
    
} 



bool DiscoveryListener::isPacketValid(std::array<char, MaxPacketSize_> const& buffer,  std::size_t const& bytesReceived)
{
    

    if (bytesReceived < 23) return false;                        
    uint32_t receivedMagic;
    std::memcpy(&receivedMagic, buffer.data(), sizeof(uint32_t));
    return ntohl(receivedMagic) == MAGIC;

}
void DiscoveryListener::processPacket(std::array<char, MaxPacketSize_> const& buffer, std::size_t const& bytesReceived)
{

}
#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>  
#include "AsioPDS/node.h"
#include "AsioPDS/discoveryPacket.h" 

Node::Node(boost::asio::io_context& io_)
    : io(io_) 
{
    uuid_ = loadOrCreateNodeUuid();
    listener_ = std::make_shared<DiscoveryListener>(io, 
        [this](DiscoveryPacket const& packet, boost::asio::ip::udp::endpoint const& sender) 
        {
            this->handleDiscoveryPacket(packet, sender);
        }
    );

    listener_->start();
}

void Node::handleDiscoveryPacket(DiscoveryPacket const& packet, boost::asio::ip::udp::endpoint const& sender)
{

    boost::uuids::uuid peerUuid;
    std::memcpy(peerUuid.data, packet.uuid, 16);
    if(peerUuid == uuid_)
    {
        std::cout << "Self broadcast\n";
        return;
    }
    if(!peers.contains(peerUuid))
    {
        std::cout << "New Peer discovered\n";
    }
    std::cout << "Peer is already in the peer list\n";

        std::cout << "Magic: 0x" 
              << std::hex << std::setw(8) << std::setfill('0')
              << ntohl(packet.magic) << std::dec << "\n"
              
              << "Version: " << static_cast<int>(packet.version) << "\n"
              
              << "UUID: ";
    
    boost::uuids::uuid uuid;
    std::memcpy(uuid.data, packet.uuid, 16);
    std::cout << uuid << "\n"
              
              << "TCP Endpoint: " << sender.address() << ':' << ntohs(packet.tcpPort) << "\n";
}


std::filesystem::path Node::userConfigurationDirectory()
{
    #ifdef _WIN32
    const char* base = std::getenv("APPDATA");           
    if (!base) throw std::runtime_error("APPDATA not set");
    return std::filesystem::path(base) / "pds";                       
#else
    const char* base = std::getenv("HOME");              
    if (!base) throw std::runtime_error("HOME not set");
    return std::filesystem::path(base) / ".pds";                     
#endif

}
std::filesystem::path Node::uuidFilePath() { return userConfigurationDirectory() / "node.uuid"; }

boost::uuids::uuid Node::loadOrCreateNodeUuid()
{
    std::filesystem::path directory  = userConfigurationDirectory();
    std::filesystem::path file = uuidFilePath();
    
    if (!std::filesystem::exists(directory)) std::filesystem::create_directories(directory);

    if (std::filesystem::exists(file))
    {
        std::ifstream in(file);
        boost::uuids::uuid id;
        in >> id;                         
        if (in) return id;
    }

    boost::uuids::uuid id = boost::uuids::random_generator()();
    std::ofstream(file) << id;            
    return id;
}

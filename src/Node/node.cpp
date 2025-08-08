#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>  
#include <boost/uuid/uuid_hash.hpp>
#include "AsioPDS/node.h"
#include "AsioPDS/discoveryPacket.h" 
#include "AsioPDS/logging.h"

Node::Node(boost::asio::io_context& io_, uint16_t tcpPort, std::string const& dataTag)
    :   io(io_),
        uuid_(loadOrCreateNodeUuid(dataTag)),
        acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), tcpPort))

{
    LOG(info) << "Node: " << uuid_ << " Listening on TCP: " << tcpPort << '\n';
    listener = std::make_shared<DiscoveryListener>(io, 
        [this](DiscoveryPacket const& packet, boost::asio::ip::udp::endpoint const& sender) 
        {
            this->handleDiscoveryPacket(packet, sender);
        }
    );
    listener->start(); 
    broadcaster = std::make_shared<DiscoveryBroadcaster>(io_, uuid_, tcpPort);
    broadcaster->start();
    peers.reserve(256);
    startAccept();

}

void Node::handleDiscoveryPacket(DiscoveryPacket const& packet, boost::asio::ip::udp::endpoint const& sender)
{

    boost::uuids::uuid peerUUID_;
    std::memcpy(peerUUID_.data, packet.uuid, 16);
    if(peerUUID_ == uuid_)
    {
        LOG(debug) << "Ignored a self broadcast packet\n";
        return;
    }
    
    auto [position, isNew] = peers.try_emplace(
        peerUUID_,
        Peer
        {
            .id = peerUUID_,
            .PeerEndpoint =  {sender.address(), ntohs(packet.tcpPort)},
            .state = PeerState::Discovered,
            .lastSeen = std::chrono::steady_clock::now()
        }
        );

    auto& peer = position->second;
    peer.lastSeen = std::chrono::steady_clock::now();

    if (isNew)
    {
        LOG(info)<< "New peer discovered, peer list has been updated.\n";
    }
    else
    {
        LOG(info) << "Seen an already existing peer again, timestamp has been updated.\n";
    }
    
    if(peer.state==PeerState::Discovered && busy.insert(peerUUID_).second)
    {
        dialQueue.push(peerUUID_);
        pumpDial();
    }

}

void Node::pumpDial()
{
    if(dialQueue.empty()) return;
    auto id = dialQueue.front(); dialQueue.pop();
    auto& peer = peers.at(id);

    peer.state = PeerState::Connecting;
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io);
    socket->async_connect(peer.PeerEndpoint, [this,socket,id](auto e)
    {
        busy.erase(id);
        if(e)
        {
            peers.at(id).state = PeerState::Discovered;
            return pumpDial();
        }
        sendHello(socket);
        pumpDial();
    });
}

void Node::sendHello(std::shared_ptr<boost::asio::ip::tcp::socket> const& socket)
{
    std::array<uint8_t,23> buffer{};
    uint32_t* pm = reinterpret_cast<uint32_t*>(buffer.data());
    *pm = htonl(proto::Magic);
    buffer[4] = proto::Version;
    std::memcpy(&buffer[5], uuid_.data,16);
    boost::asio::async_write(*socket, boost::asio::buffer(buffer),
        [this,socket](auto e, size_t){
            if(e) return;
            readHello(socket);
        });
}

void Node::readHello(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
    auto buffer = std::make_shared<std::array<uint8_t,23>>();
    boost::asio::async_read(*socket, boost::asio::buffer(*buffer),
        [this, socket, buffer](auto e, size_t)
        {
            if (e) return;
            if (ntohl(*reinterpret_cast<uint32_t*>(buffer->data())) != proto::Magic) return;
            if (buffer->at(4) != proto::Version)                                      return;

            boost::uuids::uuid pid;
            std::memcpy(pid.data, &(*buffer)[5], 16);
            auto existing = peers.find(pid);
            if (existing != peers.end() && existing->second.state == PeerState::Connected)
            {
            socket->close();
            return;
            }

            auto [it, _] = peers.try_emplace(
                pid,
                Peer
                {
                      pid,
                      socket->remote_endpoint(),    
                      socket,
                      PeerState::Connected,
                      std::chrono::steady_clock::now() });

            auto& peer = it->second;
            peer.controlSocket = socket;
            peer.state         = PeerState::Connected;
            peer.lastSeen      = std::chrono::steady_clock::now();

            if (!peer.helloSent)
            {
                sendHello(socket);      
                peer.helloSent = true;
            }
            LOG(info) << "Peer " << pid << " connected";
        });
}

void Node::startAccept()
{
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io);
    acceptor.async_accept(*socket, [this,socket](auto e)
    {
        if(!e) readHello(socket);
        startAccept();
    });
}


std::filesystem::path Node::userConfigurationDirectory(std::string const& tag)
{
#ifdef _WIN32
    const char* base = std::getenv("APPDATA");
#else
    const char* base = std::getenv("HOME");
#endif
    if(!base) throw std::runtime_error("HOME/APPDATA not set");
    return std::filesystem::path(base) / tag; 
}



std::filesystem::path Node::uuidFilePath(std::string const& tag) { return userConfigurationDirectory(tag) / "node.uuid"; }

boost::uuids::uuid Node::loadOrCreateNodeUuid(std::string const& tag)
{
    std::filesystem::path directory  = userConfigurationDirectory(tag);
    std::filesystem::path file = uuidFilePath(tag);
    
    if (!std::filesystem::exists(directory))
    {
        LOG(info) << "User configuration directory does not exist.\n";
        LOG(info) <<  "Creating directory: " << directory << '\n';
        std::filesystem::create_directories(directory);
    }

    if (std::filesystem::exists(file))
    {
        LOG(info) << "UUID file exists at: " << file << '\n';
        LOG(info) << "Loading exisiting UUID..\n";
        std::ifstream in(file);
        boost::uuids::uuid id;
        in >> id;                         
        if (in) LOG(info)  << "Loading successful.\n";
        return id;
    }
    LOG(info) << "No existing UUID file found. Generating a new UUID...\n";
    boost::uuids::uuid id = boost::uuids::random_generator()();
    std::ofstream(file) << id;
    LOG(info) << "UUID generation has been successful. Stored in: " << file << '\n';
    return id;
}

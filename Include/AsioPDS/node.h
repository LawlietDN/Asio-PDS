#pragma once
#include <memory>
#include <string>
#include <filesystem>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include "AsioPDS/discoveryListener.h"
#include <AsioPDS/discoveryBroadcaster.h>
#include "AsioPDS/peer.h"


struct DiscoveryPacket; 

class Node
{
private:
    boost::asio::io_context& io;
    boost::uuids::uuid uuid_;

    boost::asio::ip::tcp::acceptor acceptor;
    std::shared_ptr<DiscoveryListener> listener;
    std::shared_ptr<DiscoveryBroadcaster> broadcaster;

    std::unordered_map<boost::uuids::uuid, Peer, boost::hash<boost::uuids::uuid>> peers;
    std::queue<boost::uuids::uuid> dialQueue;
    std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>> busy;

    void handleDiscoveryPacket(DiscoveryPacket const& packet, boost::asio::ip::udp::endpoint const& sender);
    void pumpDial();
    void sendHello(std::shared_ptr<boost::asio::ip::tcp::socket> const& socket);
    void readHello(std::shared_ptr<boost::asio::ip::tcp::socket> const& socket);
    void startAccept();

    static boost::uuids::uuid loadOrCreateNodeUuid(std::string const& tag);
    static std::filesystem::path userConfigurationDirectory(std::string const& tag);
    static std::filesystem::path uuidFilePath(std::string const& tag);



public:
    explicit Node(boost::asio::io_context& ioCtx, uint16_t tcpPort, std::string const& dataTag);
};
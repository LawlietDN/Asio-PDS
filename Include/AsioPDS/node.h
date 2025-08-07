#pragma once
#include <memory>
#include <string>
#include <filesystem>
#include <utility>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include "AsioPDS/discoveryListener.h"
#include "peer.h"


struct DiscoveryPacket; 

class Node
{
private:
    boost::asio::io_context& io;
    boost::uuids::uuid uuid_;
    boost::uuids::uuid loadOrCreateNodeUuid();
    void handleDiscoveryPacket(DiscoveryPacket const& packet, boost::asio::ip::udp::endpoint const& sender);
    static std::filesystem::path userConfigurationDirectory();
    static std::filesystem::path uuidFilePath();
    std::shared_ptr<DiscoveryListener> listener_; 



public:
    Node(boost::asio::io_context& ioCtx);
};
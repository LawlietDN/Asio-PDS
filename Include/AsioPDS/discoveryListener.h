#pragma once
#include <memory>
#include <iostream>
#include <utility>
#include <boost/asio.hpp>
#include "proto.h"
#include "AsioPDS/discoveryPacket.h"

class DiscoveryListener : public std::enable_shared_from_this<DiscoveryListener>
{
private:
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint sender;

    std::function<void(DiscoveryPacket, boost::asio::ip::udp::endpoint)> onPacketReceived;
    std::array<char, sizeof(DiscoveryPacket)> buffer;
    void startReceive();
    bool processPacket(std::array<char, sizeof(DiscoveryPacket)> const& buffer,  size_t const& bytes);


public:
    DiscoveryListener(boost::asio::io_context& io, std::function<void(DiscoveryPacket, boost::asio::ip::udp::endpoint)> onPacketReceived_);
    void start();
};
#pragma once
#include <span>
#include <boost/uuid/uuid.hpp>
#include "AsioPDS/proto.h"

#pragma pack(push,1)          
struct DiscoveryPacket {
    uint32_t magic;       
    uint8_t  version;
    uint8_t  uuid[16];
    uint16_t tcpPort;   
};
#pragma pack(pop)
static_assert(sizeof(DiscoveryPacket) == 23, "Packet must be 23 bytes");

inline DiscoveryPacket makeDiscovery(boost::uuids::uuid const& id, std::uint16_t portHostOrder)
{
    DiscoveryPacket packet{};
    packet.magic   = htonl(proto::Magic);
    packet.version = proto::Version;
    std::memcpy(packet.uuid, id.data, 16);
    packet.tcpPort = htons(portHostOrder);
    return packet;
}

inline bool decodeDiscovery(std::span<const char, sizeof(DiscoveryPacket)> buf, DiscoveryPacket& receivedPacket)
{
    std::memcpy(&receivedPacket, buf.data(), sizeof(receivedPacket));

    if (ntohl(receivedPacket.magic) != proto::Magic)   return false;
    if (receivedPacket.version      != proto::Version) return false;
    return true;
}

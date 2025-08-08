#include <utility>
#include "AsioPDS/discoveryBroadcaster.h"
#include "AsioPDS/logging.h"
#include "AsioPDS/discoveryPacket.h"

DiscoveryBroadcaster::DiscoveryBroadcaster(boost::asio::io_context& io, boost::uuids::uuid id, uint16_t tcpPort_)
    :   timer(io), socket(io, boost::asio::ip::udp::v4()),
        id(id), tcpPort(tcpPort_)
{
    socket.set_option(boost::asio::socket_base::broadcast(true));
}

void DiscoveryBroadcaster::tick()
{
    auto packet = makeDiscovery(id, tcpPort);
    socket.async_send_to(boost::asio::buffer(&packet,sizeof packet),
        { boost::asio::ip::address_v4::broadcast(), proto::DiscoveryPort },
        [](auto,auto){});
    timer.expires_after(std::chrono::seconds(5));
    timer.async_wait([self=shared_from_this()](auto){ self->tick(); });
}

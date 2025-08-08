#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>


class DiscoveryBroadcaster : public std::enable_shared_from_this<DiscoveryBroadcaster>
{
private:
    boost::asio::steady_timer timer;
    boost::asio::ip::udp::socket socket;
    boost::uuids::uuid id;
    uint16_t tcpPort;
    void tick();

public:
    explicit DiscoveryBroadcaster(boost::asio::io_context& io, boost::uuids::uuid id_, uint16_t tcpPort_);

    void start() {tick();}

};
#include <boost/uuid/uuid.hpp>
#include <boost/asio.hpp>

using TcpPort = decltype(
    std::declval<boost::asio::ip::tcp::endpoint>().port());

struct Peer
{
    boost::uuids::uuid PeerUUID;
    TcpPort PeerTcpPort;
    boost::asio::ip::tcp::endpoint PeerEndpoint;
    std::chrono::steady_clock::time_point lastSeen;

};
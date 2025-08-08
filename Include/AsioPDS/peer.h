#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/asio.hpp>

enum class PeerState { Discovered, Connecting, Connected };
struct Peer
{
    boost::uuids::uuid id;
    boost::asio::ip::tcp::endpoint PeerEndpoint;
    std::shared_ptr<boost::asio::ip::tcp::socket> controlSocket;
    PeerState state = PeerState::Discovered;
    std::chrono::steady_clock::time_point lastSeen;
    bool helloSent{false};  

};
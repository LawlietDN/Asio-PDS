#include <memory>
#include <iostream>
#include <boost/asio.hpp>

class DiscoveryListener : public std::enable_shared_from_this<DiscoveryListener>
{
private:
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint sender;
    uint16_t port;
    static constexpr uint32_t MAGIC = 0xDEADBEEF; //Temporary solution.
    static constexpr std::size_t MaxPacketSize_ = 1024; 
    std::array<char, MaxPacketSize_> buffer;
    void startReceive();
    void processPacket(std::array<char, MaxPacketSize_> const& buffer, std::size_t const& bytesReceived);
    bool isPacketValid(std::array<char, MaxPacketSize_> const& buffer,  std::size_t const& bytesReceived);


public:
    explicit DiscoveryListener(boost::asio::io_context& io)
    :   socket(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0)),
        port(socket.local_endpoint().port())
        {
            std::cout << "[Listener] Listening on "
            << socket.local_endpoint().address() << ':'
            << socket.local_endpoint().port() << '\n';
        }
    void start();
};
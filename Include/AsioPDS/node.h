#pragma once
#include <memory>
#include <string>
#include <filesystem>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>  
#include <AsioPDS/discoveryListener.h>

class Node
{
private:
    boost::asio::io_context& io;
    boost::uuids::uuid uuid_;
    static constexpr uint8_t version = 1;
    boost::uuids::uuid loadOrCreateNodeUuid();
    static std::filesystem::path userConfigurationDirectory();
    static std::filesystem::path uuidFilePath();
    std::shared_ptr<DiscoveryListener> listener_; 



public:
    Node(boost::asio::io_context& ioCtx)
    :  io(ioCtx) 
    {
        uuid_ = loadOrCreateNodeUuid();
        std::cout << "UUID: " << uuid_ << '\n';
        std::cout << "UUID File PAth: " << uuidFilePath() << '\n';

        listener_ = std::make_shared<DiscoveryListener>(io, version);
        listener_->start();
    }
};
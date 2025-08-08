#pragma once
#include <cstdint>

namespace proto
{
    constexpr uint32_t Magic           = 0xDEADBEEF; //This is only a temporary solution.
    constexpr uint8_t  Version         = 0x01;
    constexpr uint16_t DiscoveryPort   = 54321;   
    // constexpr uint8_t  MTHELLO        = 0x10;  
    // constexpr uint8_t  MTSUBSCRIBE    = 0x01;   
    // constexpr uint8_t  MTPUBLISH      = 0x03;  


    // enum class MsgType : std::uint8_t
    // {
    //     Subscribe          = 0x01,
    //     PropagateSubscribe = 0x02,
    //     Publish            = 0x03
    // };
} 

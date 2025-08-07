#pragma once
#include <cstdint>

namespace proto
{
    inline constexpr uint32_t Magic   = 0xDEADBEEF;
    inline constexpr uint8_t  Version = 0x01;
    inline constexpr uint16_t DiscoveryPort = 54321;

    enum class MsgType : std::uint8_t
    {
        Subscribe          = 0x01,
        PropagateSubscribe = 0x02,
        Publish            = 0x03
    };
} 

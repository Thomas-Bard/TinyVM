#ifndef CPU_COMMONS_HPP
#define CPU_COMMONS_HPP

#include <functional>
#include <cstdint>

namespace cpu
{
    typedef std::function<void(uint16_t addr, uint16_t data)> MemoryWriteCallback;
    typedef std::function<uint16_t(uint16_t addr)> MemoryReadCallback;

    typedef std::function<void(uint8_t port, uint16_t addr, uint16_t data)> PortWriteCallback;
    typedef std::function<uint16_t(uint8_t port, uint16_t addr)> PortReadCallback;
}

#endif

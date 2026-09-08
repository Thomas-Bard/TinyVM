#ifndef RAM_HPP
#define RAM_HPP

#include <cstdint>
#include <vector>
#include <functional>

namespace RAM
{
    /* Prototypes for RAM Callbacks
    typedef std::function<void(uint16_t addr, uint16_t data)> MemoryWriteCallback;
    typedef std::function<uint16_t(uint16_t addr)> MemoryReadCallback;

    typedef std::function<void(uint8_t port, uint16_t addr, uint16_t data)> PortWriteCallback;
    typedef std::function<uint16_t(uint8_t port, uint16_t addr)> PortReadCallback;
    */

    class RAM
    {
        public:
            RAM(uint16_t size) noexcept;
            void write(uint16_t addr, uint16_t data) noexcept;
            uint16_t read(uint16_t addr) noexcept;
        private:
            std::vector<uint16_t> m_memory;
    };
}

#endif

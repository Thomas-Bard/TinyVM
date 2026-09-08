#include "ram.hpp"


namespace RAM
{
    RAM::RAM(uint16_t size) noexcept
    {
        m_memory.reserve(size);
        m_memory.assign(size, 0);
    }

    void RAM::write(uint16_t addr, uint16_t data) noexcept
    {
        if (addr >= m_memory.size())
        {
            return;
        }
        m_memory[addr] = data;
    }

    uint16_t RAM::read(uint16_t addr) noexcept
    {
        if (addr >= m_memory.size())
        {
            return 0;
        }
        return m_memory[addr];
    }
}

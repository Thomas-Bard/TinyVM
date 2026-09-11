#include "vm.hpp"
#include "cpu.hpp"
#include "ram.hpp"

namespace TinyVM
{
    VM::VM(uint16_t ram_size)
    : m_ram(ram_size),
      m_io_manager(),
      m_cpu(
            [this](uint16_t addr, uint16_t data) { m_ram.write(addr, data); },
            [this](uint16_t addr) { return m_ram.read(addr); },
            [this](uint16_t port, uint16_t addr, uint16_t data) { m_io_manager.write(port, addr, data); },
            [this](uint16_t port, uint16_t addr) { return m_io_manager.read(port, addr); }
        )
    {}

    void VM::add_device(std::unique_ptr<IO::IODevice> device)
    {
        m_io_manager.addDevice(std::move(device));
    }
}

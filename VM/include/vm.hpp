#ifndef VM_HPP
#define VM_HPP

#include <cstdint>
#include <cpu.hpp>
#include <ram.hpp>
#include <io.hpp>

namespace TinyVM
{
    class VM
    {
    public:
        VM(uint16_t ram_size);
        void run();
        void load_program(const std::vector<uint16_t>& program);
        void reset_cpu();
        void set_breakpoint(uint16_t address);
        void remove_breakpoint(uint16_t address);
        void step();
        uint64_t get_current_pc(void);
        std::array<uint16_t, 8>& get_registers();
        void add_device(std::unique_ptr<IO::IODevice> device);
        IO::IOManager& get_io_manager();
        std::vector<uint16_t>& get_ram();
    private:
        RAM::RAM m_ram;
        IO::IOManager m_io_manager;
        cpu::CPU m_cpu;
    };
}


#endif // VM_HPP

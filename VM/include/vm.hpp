#ifndef VM_HPP
#define VM_HPP

#include <cstdint>
#include <cpu.hpp>
#include <ram.hpp>

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
    };
}


#endif // VM_HPP

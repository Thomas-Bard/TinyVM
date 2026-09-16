#ifndef VM_HPP
#define VM_HPP

#include <cstdint>
#include <cpu.hpp>
#include <memory>
#include <optional>
#include <ram.hpp>
#include <io.hpp>
#include <unordered_map>

namespace TinyVM
{

    typedef struct BreakPoint
    {
        uint16_t address;
        bool enabled;
        std::string name;
    } BreakPoint;

    enum class VM_StopReason
    {
        BreakpointHit,
        CPU_Exception,
        CPU_Halted,
        VM_Paused,
        NoStop
    };

    typedef struct VM_ReturnState
    {
        VM_StopReason reason;
        uint16_t pc;
    } VM_ReturnState;

    class VM
    {
    public:
        VM(uint16_t ram_size, IO::ExternalEventHandler hndl);
        VM_ReturnState run();
        void load_program(const std::vector<uint16_t>& program);
        void reset_cpu();
        void set_breakpoint(BreakPoint bp);
        void remove_breakpoint(uint16_t address);
        VM_ReturnState step();
        VM_ReturnState resume();
        std::optional<BreakPoint> get_last_hit_bp() const;
        void pause_execution(void);
        void unpause_execution(void);

        uint64_t get_current_ir(void);
        const std::array<uint16_t, 8>& get_registers() const;
        void add_device(std::unique_ptr<IO::IODevice> device);
        IO::IOManager& get_io_manager();
        std::vector<uint16_t>& get_ram();
    private:
        VM_ReturnState m_process();

        RAM::RAM m_ram;
        IO::IOManager m_io_manager;
        cpu::CPU m_cpu;
        std::unordered_map<uint16_t, BreakPoint> m_breakpoints;
        std::optional<BreakPoint> m_last_hit_bp;
        bool should_pause;
    };
}


#endif // VM_HPP

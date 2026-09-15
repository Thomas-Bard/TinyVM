#include "vm.hpp"
#include "cpu.hpp"
#include "io.hpp"
#include "ram.hpp"

namespace TinyVM
{
    VM::VM(uint16_t ram_size, IO::ExternalEventHandler hndl)
    : m_ram(ram_size),
      m_io_manager(hndl),
      m_cpu(
            [this](uint16_t addr, uint16_t data) { m_ram.write(addr, data); },
            [this](uint16_t addr) { return m_ram.read(addr); },
            [this](uint8_t port, uint16_t addr, uint16_t data) { m_io_manager.write(port, addr, data); },
            [this](uint16_t port, uint16_t addr) { return m_io_manager.read(port, addr); }
        ),
        m_last_hit_bp(std::nullopt),
        should_pause(false)
    {}

    IO::IOManager& VM::get_io_manager(void)
    {
        return m_io_manager;
    }

    void VM::add_device(std::unique_ptr<IO::IODevice> device)
    {
        m_io_manager.addDevice(std::move(device));
    }

    void VM::reset_cpu()
    {
        m_cpu.reset();
    }

    void VM::set_breakpoint(BreakPoint bp)
    {
        m_breakpoints[bp.address] = bp;
    }

    void VM::remove_breakpoint(uint16_t address)
    {
        m_breakpoints.erase(address);
    }

    VM_ReturnState VM::step()
    {
        return m_process();
    }

    VM_ReturnState VM::resume()
    {
        should_pause = false;
        while (m_process().reason == VM_StopReason::NoStop);
        return m_process();
    }

    VM_ReturnState VM::run()
    {
        m_cpu.reset();
        while (m_process().reason == VM_StopReason::NoStop);
        return m_process();
    }

    uint64_t VM::get_current_ir()
    {
        return m_cpu.get_instruction_register();
    }

    VM_ReturnState VM::m_process()
    {
        VM_ReturnState result;
        if (should_pause)
        {
            result.pc = m_cpu.get_program_counter();
            result.reason = VM_StopReason::VM_Paused;
            return result;
        }

        m_cpu.tick();
        result.pc = m_cpu.get_program_counter();
        if (m_cpu.has_exception())
        {
            result.reason = VM_StopReason::CPU_Exception;
        }
        else if (m_cpu.is_halted())
        {
            result.reason = VM_StopReason::CPU_Halted;
        }
        else
        {
            result.reason = VM_StopReason::NoStop;
        }
        if (m_breakpoints.contains(result.pc))
        {
            m_last_hit_bp = m_breakpoints[result.pc];
            result.reason = VM_StopReason::BreakpointHit;
        }
        m_io_manager.update_devices();
        return result;
    }

    std::optional<BreakPoint> VM::get_last_hit_bp() const
    {
        return m_last_hit_bp;
    }

    void VM::load_program(const std::vector<uint16_t>& program)
    {
        for (uint64_t i = 0; i < program.size(); ++i)
        {
            m_ram.write((uint16_t)i, program[i]);
        }
    }

    void VM::pause_execution()
    {
        should_pause = true;
    }

    void VM::unpause_execution()
    {
        should_pause = false;
    }
}

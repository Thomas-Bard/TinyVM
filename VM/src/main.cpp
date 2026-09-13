#include <ios>
#include <iostream>
#include <cstdint>
#include <fstream>
#include "conout.hpp"
#include "vm.hpp"
#include "console_sdl.hpp"


#define RAM_SZ 0xFFFF
#define CONOUT_DEVICE_ADDR 0xBABA

// Temporary, this whole file is temporary. Absolutely AWFUL design. We'll get around it somehow, not now, but we'll get around it.

static void handler(uint16_t addr);

static TinyVM::VM* g_vm = nullptr;

void handler(uint16_t addr)
{
    TinyVM::IO::IODevice* device = g_vm->get_io_manager().getDevice(addr);
    if (addr == CONOUT_DEVICE_ADDR)
    {
        TinyVM::IO::ConOut* conout_device = reinterpret_cast<TinyVM::IO::ConOut*>(device);
        console::ConsoleRendererSDL* renderer = reinterpret_cast<console::ConsoleRendererSDL*>(conout_device->getRenderer());
        if (renderer->should_quit())
        {
            g_vm->pause_execution();
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <program>  <path-to-console-ttf>" << std::endl;
        return 1;
    }

    TinyVM::VM vm(RAM_SZ, handler);
    g_vm = &vm;

    std::ifstream program(argv[1], std::ios::binary | std::ios::in | std::ios::ate);
    if (!program.is_open())
    {
        std::cerr << "Unable to open program binary" << std::endl;
        return 1;
    }

    std::shared_ptr<console::ConsoleRendererSDL> console_renderer = std::make_shared<console::ConsoleRendererSDL>(80, 60, std::string(argv[2]));

    vm.add_device(std::make_unique<TinyVM::IO::ConOut>(CONOUT_DEVICE_ADDR, 80, 60, console_renderer));

    // Program image is too large
    ssize_t img_size = 0;
    if ((img_size = program.tellg()) > 0x20000)
    {
        std::cerr << "Program too large !" << std::endl;
        return 1;
    }

    if (img_size % 2 != 0)
    {
        std::cerr << "Program format invalid. Contains byte-addressable data." << std::endl;
        return 1;
    }

    program.seekg(std::ios::beg);

    uint8_t* data = new uint8_t[static_cast<size_t>(img_size)];
    if (!data)
    {
        std::cerr << "Unable to load program in memory (Host allocation failure)" << std::endl;
        return 1;
    }
    // I fucking hate clang for making a tantrum as signed char* is not the same as char*
    program.read(reinterpret_cast<char*>(data), img_size);
    {
        std::vector<uint16_t> temp_prg_vector(reinterpret_cast<uint16_t*>(data), reinterpret_cast<uint16_t*>(data) + img_size / 2);
        vm.load_program(temp_prg_vector);
    }
    delete[] data;
    data = nullptr;
    program.close();

    vm.reset_cpu();
    TinyVM::VM_ReturnState return_state = vm.run();
    switch (return_state.reason)
    {
        case TinyVM::VM_StopReason::BreakpointHit:
            std::cout << "[WIP] Breakpoint hit. Execution aborted. PC=0x" << std::hex << return_state.pc << std::endl;
            return 0;
        case TinyVM::VM_StopReason::CPU_Halted:
            std::cout << "CPU Halted. Execution terminated. PC=0x" << std::hex << return_state.pc << std::endl;
            return 0;
        case TinyVM::VM_StopReason::CPU_Exception:
            std::cout << "CPU Exception. Execution terminated. PC=0x" << std::hex << return_state.pc << std::endl;
            return 0;
        case TinyVM::VM_StopReason::VM_Paused:
            std::cout << "Execution aborted externally. PC=0x" << std::hex << return_state.pc << std::endl;
            return 0;
        default:
            std::cout << "Unsupported VM Stop encountered. Execution terminated. PC=0x" << std::ios_base::hex << return_state.pc << std::endl;
            return 1;
    }
}

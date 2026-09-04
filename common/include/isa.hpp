#ifndef ISA_HPP
#define ISA_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace isa
{
    struct Opcode
    {
        std::string name;
        uint16_t opcode;
    };

    extern const std::unordered_map<std::string, Opcode> OPCODES;
}

#endif

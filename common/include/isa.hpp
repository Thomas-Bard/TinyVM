#ifndef ISA_HPP
#define ISA_HPP

#include <cstdint>
#include <string>

#define OPCODE(name, opcode, operand_count) \
    (isa::Opcode{ name, opcode, operand_count })

#define HALT_OPCODE OPCODE("halt", 0x00, 0)
#define ADD_OPCODE OPCODE("add", 0x01, 3)
#define ADDI_OPCODE OPCODE("addi", 0x02, 3)
#define SUB_OPCODE OPCODE("sub", 0x03, 3)
#define SUBI_OPCODE OPCODE("subi", 0x04, 3)
#define ADDS_OPCODE OPCODE("adds", 0x05, 3)
#define ADDSI_OPCODE OPCODE("addsi", 0x06, 3)
#define SUBS_OPCODE OPCODE("subs", 0x07, 3)
#define SUBSI_OPCODE OPCODE("subsi", 0x08, 3)
#define DIV_OPCODE OPCODE("div", 0x09, 3)
#define DIVI_OPCODE OPCODE("divi", 0x0A, 3)
#define DIVS_OPCODE OPCODE("divs", 0x0B, 3)
#define DIVSI_OPCODE OPCODE("divsi", 0x0C, 3)
#define MUL_OPCODE OPCODE("mul", 0x0D, 3)
#define MULI_OPCODE OPCODE("muli", 0x0E, 3)
#define MULS_OPCODE OPCODE("muls", 0x0F, 3)
#define MULSI_OPCODE OPCODE("mulsi", 0x10, 3)
#define INC_OPCODE OPCODE("inc", 0x11, 1)
#define DEC_OPCODE OPCODE("dec", 0x12, 1)

#define MOV_OPCODE OPCODE("mov", 0x13, 2)
#define MOVI_OPCODE OPCODE("movi", 0x14, 2)
#define STR_OPCODE OPCODE("str", 0x15, 2)
#define STRI_OPCODE OPCODE("stri", 0x16, 2)
#define LD_OPCODE OPCODE("ld", 0x17, 2)
#define PUSH_OPCODE OPCODE("push", 0x18, 1)
#define POP_OPCODE OPCODE("pop", 0x19, 1)
#define OUT_OPCODE OPCODE("out", 0x1A, 3)
#define IN_OPCODE OPCODE("in", 0x1B, 3)

#define JMP_OPCODE OPCODE("jmp", 0x1C, 1)
#define JMPI_OPCODE OPCODE("jmpi", 0x1D, 1)
#define JC_OPCODE OPCODE("jc", 0x1E, 1)
#define JNC_OPCODE OPCODE("jnc", 0x1F, 1)
#define JZ_OPCODE OPCODE("jz", 0x20, 1)
#define JNZ_OPCODE OPCODE("jnz", 0x21, 1)
#define JA_OPCODE OPCODE("ja", 0x22, 1)
#define JAE_OPCODE OPCODE("jae", 0x23, 1)
#define JB_OPCODE OPCODE("jb", 0x24, 1)
#define JBE_OPCODE OPCODE("jbe", 0x25, 1)
#define JNE_OPCODE OPCODE("jne", 0x26, 1)
#define JE_OPCODE OPCODE("je", 0x27, 1)

#define AND_OPCODE OPCODE("and", 0x28, 3)
#define OR_OPCODE OPCODE("or", 0x29, 3)
#define XOR_OPCODE OPCODE("xor", 0x2A, 3)
#define NOT_OPCODE OPCODE("not", 0x2B, 2)
#define SHR_OPCODE OPCODE("shr", 0x2C, 3)
#define SHL_OPCODE OPCODE("shl", 0x2D, 3)

#define CMP_OPCODE OPCODE("cmp", 0x2E, 2)
#define TEST_OPCODE OPCODE("test", 0x2F, 2)
#define CLF_OPCODE OPCODE("clf", 0x30, 1)
#define LDI_OPCODE OPCODE("ldi", 0x31, 2)




namespace isa
{
    struct Opcode
    {
        std::string name;
        uint16_t opcode;
        uint8_t operand_count;
    };
}

#endif

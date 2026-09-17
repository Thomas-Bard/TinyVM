#include "tokens.hpp"
#include <string_view>
#include <unordered_map>

namespace ASM::Tokens
{
    static std::unordered_map<std::string, Instruction> _inst;
    static std::unordered_map<std::string, AddressableRegister> _reg;

    static void op(std::string mnemonic, uint8_t opcode, uint8_t operands, OperandTypes op0, OperandTypes op1, OperandTypes op2)
    {
        _inst[mnemonic] = {
            .mnemonic = mnemonic,
            .opcode = opcode,
            .operands = operands,
            .operand_types = {op0, op1, op2}
        };
    }

    void reg(std::string mnemonic, uint8_t code, bool is_grp = true)
    {
        _reg[mnemonic] = {
            .mnemonic = mnemonic,
            .identifier = code,
            .is_grp = is_grp
        };
    }

    static void opn(std::string mnemonic, uint8_t opcode)
    {
        op(mnemonic, opcode, 0, OperandTypes::NONE, OperandTypes::NONE, OperandTypes::NONE);
    }

    static void oprrr(std::string mnemonic, uint8_t opcode)
    {
        op(mnemonic, opcode, 3, OperandTypes::REG, OperandTypes::REG, OperandTypes::REG);
    }

    static void oprri(std::string mnemonic, uint8_t opcode)
    {
        op(mnemonic, opcode, 3, OperandTypes::REG, OperandTypes::REG, OperandTypes::IMM);
    }

    static void oprr(std::string mnemnonic, uint8_t opcode)
    {
        op(mnemnonic, opcode, 2, OperandTypes::REG, OperandTypes::REG, OperandTypes::NONE);
    }

    static void opr(std::string mnemonic, uint8_t opcode)
    {
        op(mnemonic, opcode, 1, OperandTypes::REG, OperandTypes::NONE, OperandTypes::NONE);
    }

    static void opi(std::string mnemonic, uint8_t opcode)
    {
        op(mnemonic, opcode, 1, OperandTypes::IMM, OperandTypes::NONE, OperandTypes::NONE);
    }

    static void opri(std::string mnemonic, uint8_t opcode)
    {
        op(mnemonic, opcode, 2, OperandTypes::REG, OperandTypes::IMM, OperandTypes::NONE);
    }

    void init_instructions(void)
    {
        opn("halt", 0);
        oprrr("add", 1);
        oprri("addi", 2);
        oprrr("sub", 3);
        oprri("subi", 4);
        oprrr("adds", 5);
        oprri("addsi", 6);
        oprrr("subs", 7);
        oprri("subsi", 8);
        oprrr("div", 9);
        oprri("divi", 0xa);
        oprrr("divs", 0xb);
        oprri("divsi", 0xc);
        oprrr("mul", 0xd);
        oprri("muli", 0xe);
        oprrr("muls", 0xf);
        oprri("mulsi", 0x10);
        opr("inc", 0x11);
        opr("dec", 0x12);
        op("mov", 0x13, 2, OperandTypes::REGSP, OperandTypes::REGSP, OperandTypes::NONE);
        opri("movi", 0x14);
        oprr("str", 0x15);
        opri("stri", 0x16);
        oprr("ld", 0x17);
        opr("push", 0x18);
        opr("pop", 0x19);
        oprrr("out", 0x1A);
        oprrr("in", 0x1B);
        opr("jmp", 0x1C);
        opi("jmpi", 0x1D);
        opr("jc", 0x1E);
        opr("jnc", 0x1F);
        opr("jz", 0x20);
        opr("jnz", 0x21);
        opr("ja", 0x22);
        opr("jae", 0x23);
        opr("jb", 0x24);
        opr("jbe", 0x25);
        opr("jne", 0x26);
        opr("je", 0x27);
        oprrr("and", 0x28);
        oprrr("or", 0x29);
        oprrr("xor", 0x2A);
        opr("not", 0x2B);
        oprrr("shr", 0x2C);
        oprrr("shl", 0x2D);
        oprr("cmp", 0x2E);
        oprr("test", 0x2F);
        opn("clf", 0x30);
        opri("ldi", 0x31);
        oprr("msb", 0x32);
        oprr("lsb", 0x33);
        op("outi", 0x34, 3, OperandTypes::IMM, OperandTypes::IMM, OperandTypes::REG);
        opi("jei", 0x35);
        opi("jnei", 0x36);
        opi("jci", 0x37);
        opi("jnci", 0x38);
        opi("jzi", 0x39);
        opi("jnzi", 0x3A);
        opi("jai", 0x3B);
        opi("jaei", 0x3C);
        opi("jbi", 0x3D);
        opi("jbei", 0x3E);
        op("ini", 0x3F, 3, OperandTypes::REG, OperandTypes::IMM, OperandTypes::IMM);
        opr("liommu", 0x40);
    }

    std::unordered_map<std::string, Instruction> const* get_instructions(void)
    {
        if (_inst.empty())
            return nullptr;
        return &_inst;
    }

    uint8_t get_opcode(std::string const& mnemonic)
    {
        if (!_inst.contains(mnemonic))
            return 0xFF;
        return _inst[mnemonic].opcode;
    }

    uint8_t get_operand(std::string const& mnemonic)
    {
        if (!_inst.contains(mnemonic))
            return 0xFF;
        return _inst[mnemonic].operands;
    }

    Instruction const* get_instruction(std::string const& mnemonic)
    {
        if (!_inst.contains(mnemonic))
            return nullptr;
        return &_inst[mnemonic];
    }

    void init_registers(void)
    {
        reg("ra", 0x00);
        reg("rb", 0x01);
        reg("rc", 0x02);
        reg("rd", 0x03);
        reg("re", 0x04);
        reg("rf", 0x05);
        reg("rg", 0x06);
        reg("rh", 0x07);
        reg("sp", 0x08, false);
    }

    std::unordered_map<std::string, AddressableRegister> const* get_registers(void)
    {
        if (_reg.empty())
            return nullptr;
        return &_reg;
    }
    uint8_t get_register_number(std::string const& mnemonic)
    {
        if (!_reg.contains(mnemonic))
            return 0xFF;
        return _reg[mnemonic].identifier;
    }
    AddressableRegister const* get_register(std::string const& mnemonic)
    {
        if (!_reg.contains(mnemonic))
            return nullptr;
        return &_reg[mnemonic];
    }
}

#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <unordered_map>
#include <string>
#include <cstdint>
#include <variant>

/** TODO REDO ALL OF THE TOKENS AND LEXER **/

namespace ASM::Tokens
{
    enum class TokenKind
    {
        INDENTIFIER,
        INSTRUCTION,
        REGISTER,
        DECL,
        STRING,
        NUMBER,
        COMMA,
        COLON,
        NEW_LINE,
        END_FILE,
    };



    enum class OperandTypes
    {
        REG,
        REGSP,
        IMM,
        NONE
    };

    struct Instruction
    {
        std::string mnemonic;
        uint8_t opcode;
        uint8_t operands;
        OperandTypes operand_types[3];
    };

    struct AddressableRegister
    {
        std::string mnemonic;
        uint8_t identifier;
        bool is_grp;
    };

    struct Token
    {
        TokenKind kind;
        std::variant<

            std::string,
            uint16_t,
            Instruction,
            AddressableRegister
        > value;
        uint64_t line;
        uint64_t col;
    };

    void init_instructions(void);
    std::unordered_map<std::string, Instruction> const* get_instructions(void);
    uint8_t get_opcode(std::string const& mnemonic);
    uint8_t get_operands(std::string const& mnemonic);
    Instruction const* get_instruction(std::string const& mnemonic);

    void init_registers(void);
    std::unordered_map<std::string, AddressableRegister> const* get_registers(void);
    uint8_t get_register_number(std::string const& mnemonic);
    AddressableRegister const* get_register(std::string const& mnemonic);
}

#endif

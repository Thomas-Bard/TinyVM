#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <unordered_map>
#include <string>
#include <cstdint>

/** TODO REDO ALL OF THE TOKENS AND LEXER **/

namespace ASM::Tokens
{
    struct Token
    {
        enum class TokenType
        {
            IDENTIFIER,
            INSTRUCTION,
            REGISTER,
            NUMBER,
            NUMBER_HEX,
            COMMA,
            DECLARE_WORD,
            DECLARE_STRING,
            STRING,
            LABEL_DECL,
            EOL,
            EoF,
        };
        uint64_t line_number;
        uint64_t column_number;
        std::string value;
        TokenType type;
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

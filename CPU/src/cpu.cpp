#include "cpu.hpp"

#define CF_MASK 0b0000001
#define ZF_MASK 0b0000010
#define OF_MASK 0b0000100
#define SF_MASK 0b0001000
#define DZ_MASK 0b0010000
#define II_MASK 0b0100000

#include "isa.hpp"

namespace helper {
    uint16_t extract_operand(uint64_t instruction, int index)
    {
        return (instruction >> (index * 16)) & 0xFFFF;
    }
}

namespace cpu {
    CPU::CPU(MemoryWriteCallback memory_write_callback,
        MemoryReadCallback memory_read_callback,
        PortWriteCallback port_write_callback,
        PortReadCallback port_read_callback) noexcept
    : m_memory_write_callback(memory_write_callback),
      m_memory_read_callback(memory_read_callback),
      m_port_write_callback(port_write_callback),
      m_port_read_callback(port_read_callback)
    {
        reset();
    }

    void CPU::reset(void) noexcept
    {
        m_program_counter = RESET_VECTOR;
        m_stack_pointer = 0;
        m_instruction_register = 0;
        m_halted = false;
        m_flags = 0;
        for (auto& reg : m_general_purpose_registers)
        {
            reg = 0;
        }
    }

    void CPU::tick(void) noexcept
    {
        if (m_halted)
            return;
        m_fetch();
        m_execute();
        m_program_counter += 8;
    }

    bool CPU::has_exception(void) const noexcept
    {
        return (m_instruction_register & II_MASK || m_instruction_register & DZ_MASK) != 0;
    }

    void CPU::m_fetch(void) noexcept
    {
        if (m_halted)
            return;
        m_instruction_register = m_memory_read_callback(m_program_counter);
    }

    void CPU::m_execute(void) noexcept
    {
        if (m_halted)
            return;
        uint8_t opcode = (m_instruction_register & 0xFF00000000000000) >> 56;
        switch (opcode)
        {
            case HALT_OPCODE.opcode:
                m_halt();
                break;
            case ADD_OPCODE.opcode:
                m_add(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case ADDI_OPCODE.opcode:
                m_addi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case SUB_OPCODE.opcode:
                m_sub(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case SUBI_OPCODE.opcode:
                m_subi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case ADDS_OPCODE.opcode:
                m_adds(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case ADDSI_OPCODE.opcode:
                m_addsi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                        static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                        static_cast<int16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case SUBS_OPCODE.opcode:
                m_subs(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case SUBSI_OPCODE.opcode:
                m_subsi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                        static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                        static_cast<int16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case DIV_OPCODE.opcode:
                m_div(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case DIVI_OPCODE.opcode:
                m_divi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case MUL_OPCODE.opcode:
                m_mul(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case MULI_OPCODE.opcode:
                m_muli(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case DIVS_OPCODE.opcode:
                m_divs(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case DIVSI_OPCODE.opcode:
                m_divsi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                        static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                        static_cast<int16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case MULS_OPCODE.opcode:
                m_muls(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case MULSI_OPCODE.opcode:
                m_mulsi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                        static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                        static_cast<int16_t>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case INC_OPCODE.opcode:
                m_inc(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case DEC_OPCODE.opcode:
                m_dec(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case MOV_OPCODE.opcode:
                m_mov(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case MOVI_OPCODE.opcode:
                m_movi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case STR_OPCODE.opcode:
                m_str(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case STRI_OPCODE.opcode:
                m_stri(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case LD_OPCODE.opcode:
                m_ld(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                     static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case LDI_OPCODE.opcode:
                m_ldi(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case PUSH_OPCODE.opcode:
                m_push(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case POP_OPCODE.opcode:
                m_pop(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case OUT_OPCODE.opcode:
                m_out(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case IN_OPCODE.opcode:
                m_in(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                     static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                     static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;

            case JMP_OPCODE.opcode:
                m_jmp(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JMPI_OPCODE.opcode:
                m_jmpi(static_cast<uint16_t>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JC_OPCODE.opcode:
                m_jc(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JNC_OPCODE.opcode:
                m_jnc(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JZ_OPCODE.opcode:
                m_jz(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JNZ_OPCODE.opcode:
                m_jnz(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JA_OPCODE.opcode:
                m_ja(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JAE_OPCODE.opcode:
                m_jae(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JB_OPCODE.opcode:
                m_jb(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JBE_OPCODE.opcode:
                m_jbe(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JNE_OPCODE.opcode:
                m_jne(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case JE_OPCODE.opcode:
                m_je(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;

            case AND_OPCODE.opcode:
                m_and(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case OR_OPCODE.opcode:
                m_or(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                     static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                     static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case XOR_OPCODE.opcode:
                m_xor(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case NOT_OPCODE.opcode:
                m_not(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)));
                break;
            case SHR_OPCODE.opcode:
                m_shr(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;
            case SHL_OPCODE.opcode:
                m_shl(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 2)));
                break;

            case CMP_OPCODE.opcode:
                m_cmp(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                      static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case TEST_OPCODE.opcode:
                m_test(static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 0)),
                       static_cast<RegisterNumber>(helper::extract_operand(m_instruction_register, 1)));
                break;
            case CLF_OPCODE.opcode:
                m_clf();
                break;
        }
    }
}

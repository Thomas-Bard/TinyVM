#include "cpu.hpp"

#define CF_MASK 0b0000001
#define ZF_MASK 0b0000010
#define OF_MASK 0b0000100
#define SF_MASK 0b0001000
#define DZ_MASK 0b0010000
#define II_MASK 0b0100000

#define RA 0x00
#define RB 0x01
#define RC 0x02
#define RD 0x03
#define RE 0x04
#define RF 0x05
#define RG 0x06
#define RH 0x07

#define SP_NBR 0x08


namespace cpu
{
    void CPU::m_halt(void) noexcept
    {
        m_halted = true;
    }

    void CPU::m_add(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept
    {
        if (reg1 >= m_general_purpose_registers.size() ||
            reg2 >= m_general_purpose_registers.size() ||
            reg3 >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        uint16_t a = m_general_purpose_registers[reg1];
        uint16_t b = m_general_purpose_registers[reg2];
        uint16_t result = a + b;

        // CF: unsigned wrap-around
        if (result < a)
        {
            m_flags = m_flags | CF_MASK;
        }
        // OF: signed overflow — both operands have the same sign but result differs
        if (!((a ^ b) & 0x8000) && ((result ^ a) & 0x8000))
        {
            m_flags = m_flags | OF_MASK;
        }
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        m_general_purpose_registers[reg3] = result;
    }

    void CPU::m_addi(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            rs >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        uint16_t a = m_general_purpose_registers[rs];
        uint16_t result = a + imm;

        // CF: unsigned wrap-around
        if (result < a)
        {
            m_flags = m_flags | CF_MASK;
        }
        // OF: signed overflow — both operands share the same sign but result sign differs
        if (!((a ^ imm) & 0x8000) && ((result ^ a) & 0x8000))
        {
            m_flags = m_flags | OF_MASK;
        }
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        m_general_purpose_registers[rd] = result;
    }

    void CPU::m_sub(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept
    {
        if (reg1 >= m_general_purpose_registers.size() ||
            reg2 >= m_general_purpose_registers.size() ||
            reg3 >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        uint16_t a = m_general_purpose_registers[reg1];
        uint16_t b = m_general_purpose_registers[reg2];
        uint16_t result = a - b;

        // CF: unsigned borrow
        if (b > a)
        {
            m_flags = m_flags | CF_MASK;
        }
        // OF: signed overflow — operands have different signs and result sign differs from minuend
        if (((a ^ b) & 0x8000) && ((result ^ a) & 0x8000))
        {
            m_flags = m_flags | OF_MASK;
        }
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        m_general_purpose_registers[reg3] = result;
    }

    void CPU::m_subi(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            rs >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        uint16_t a = m_general_purpose_registers[rs];
        uint16_t result = a - imm;

        // CF: unsigned borrow
        if (imm > a)
        {
            m_flags = m_flags | CF_MASK;
        }
        // OF: signed overflow — operands have different signs and result sign differs from minuend
        if (((a ^ imm) & 0x8000) && ((result ^ a) & 0x8000))
        {
            m_flags = m_flags | OF_MASK;
        }
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        m_general_purpose_registers[rd] = result;
    }

    void CPU::m_adds(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept
    {
        m_add(reg1, reg2, reg3);
        // SF: set if the result is negative (MSB set)
        if (m_general_purpose_registers[reg3] & 0x8000)
        {
            m_flags = m_flags | SF_MASK;
        }
    }

    void CPU::m_subs(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept
    {
        m_sub(reg1, reg2, reg3);
        // SF: set if the result is negative (MSB set)
        if (m_general_purpose_registers[reg3] & 0x8000)
        {
            m_flags = m_flags | SF_MASK;
        }
    }

    void CPU::m_addsi(RegisterNumber rd, RegisterNumber rs, int16_t imm) noexcept
    {
        m_addi(rd, rs, static_cast<uint16_t>(imm));
        // SF: set if the result is negative (MSB set)
        if (m_general_purpose_registers[rd] & 0x8000)
        {
            m_flags = m_flags | SF_MASK;
        }
    }

    void CPU::m_subsi(RegisterNumber rd, RegisterNumber rs, int16_t imm) noexcept
    {
        m_subi(rd, rs, static_cast<uint16_t>(imm));
        // SF: set if the result is negative (MSB set)
        if (m_general_purpose_registers[rd] & 0x8000)
        {
            m_flags = m_flags | SF_MASK;
        }
    }

    void CPU::m_div(RegisterNumber rd, RegisterNumber rs, RegisterNumber rm) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            rs >= m_general_purpose_registers.size() ||
            rm >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        if (m_general_purpose_registers[rm] == 0)
        {
            m_flags = m_flags | DZ_MASK;
            m_halt();
            return;
        }

        uint16_t quotient = m_general_purpose_registers[rs] / m_general_purpose_registers[rm];
        uint16_t remainder = m_general_purpose_registers[rs] % m_general_purpose_registers[rm];
        m_general_purpose_registers[rd] = quotient;
        m_general_purpose_registers[RH] = remainder;

        if (quotient == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
    }

    void CPU::m_divi(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            rs >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        if (imm == 0)
        {
            m_flags = m_flags | DZ_MASK;
            m_halt();
            return;
        }

        uint16_t quotient = m_general_purpose_registers[rs] / imm;
        uint16_t remainder = m_general_purpose_registers[rs] % imm;
        m_general_purpose_registers[rd] = quotient;
        m_general_purpose_registers[RH] = remainder;

        if (quotient == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
    }

    void CPU::m_mul(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept
    {
        if (reg1 >= m_general_purpose_registers.size() ||
            reg2 >= m_general_purpose_registers.size() ||
            reg3 >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        uint32_t result = static_cast<uint32_t>(m_general_purpose_registers[reg1]) * static_cast<uint32_t>(m_general_purpose_registers[reg2]);
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        // OF + CF: upper 16 bits are non-zero, result does not fit in 16 bits
        if (result > 0xFFFF)
        {
            m_flags = m_flags | OF_MASK | CF_MASK;
        }
        // truncate result to 16 bits
        m_general_purpose_registers[reg3] = static_cast<uint16_t>(result);
    }

    void CPU::m_muli(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            rs >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        uint32_t result = static_cast<uint32_t>(m_general_purpose_registers[rs]) * static_cast<uint32_t>(imm);
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        // OF + CF: upper 16 bits are non-zero, result does not fit in 16 bits
        if (result > 0xFFFF)
        {
            m_flags = m_flags | OF_MASK | CF_MASK;
        }
        // truncate result to 16 bits
        m_general_purpose_registers[rd] = static_cast<uint16_t>(result);
    }

    void CPU::m_divs(RegisterNumber rd, RegisterNumber r1, RegisterNumber r2) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            r1 >= m_general_purpose_registers.size() ||
            r2 >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        if (m_general_purpose_registers[r2] == 0)
        {
            m_flags = m_flags | DZ_MASK;
            m_halt();
            return;
        }

        // Signed division
        int16_t result = static_cast<int16_t>(m_general_purpose_registers[r1]) / static_cast<int16_t>(m_general_purpose_registers[r2]);
        int16_t remainder = static_cast<int16_t>(m_general_purpose_registers[r1]) % static_cast<int16_t>(m_general_purpose_registers[r2]);

        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        if (result < 0)
        {
            m_flags = m_flags | SF_MASK;
        }

        m_general_purpose_registers[rd] = static_cast<uint16_t>(result);
        m_general_purpose_registers[RH] = static_cast<uint16_t>(remainder);
    }

    void CPU::m_divsi(RegisterNumber rd, RegisterNumber r1, int16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            r1 >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        if (imm == 0)
        {
            m_flags = m_flags | DZ_MASK;
            m_halt();
            return;
        }
        int16_t result = static_cast<int16_t>(m_general_purpose_registers[r1]) / imm;
        int16_t remainder = static_cast<int16_t>(m_general_purpose_registers[r1]) % imm;

        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        if (result < 0)
        {
            m_flags = m_flags | SF_MASK;
        }

        m_general_purpose_registers[rd] = static_cast<uint16_t>(result);
        m_general_purpose_registers[RH] = static_cast<uint16_t>(remainder);
    }

    void CPU::m_muls(RegisterNumber rd, RegisterNumber r1, RegisterNumber r2) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            r1 >= m_general_purpose_registers.size() ||
            r2 >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        int32_t result = static_cast<int32_t>(static_cast<int16_t>(m_general_purpose_registers[r1])) * static_cast<int32_t>(static_cast<int16_t>(m_general_purpose_registers[r2]));
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        // SF: result is negative
        if (result < 0)
        {
            m_flags = m_flags | SF_MASK;
        }
        // OF + CF: result does not fit in a signed 16-bit value
        if (result < -32768 || result > 32767)
        {
            m_flags = m_flags | OF_MASK | CF_MASK;
        }
        m_general_purpose_registers[rd] = static_cast<uint16_t>(result);
    }
    void CPU::m_mulsi(RegisterNumber rd, RegisterNumber rs, int16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            rs >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        int32_t result = static_cast<int32_t>(static_cast<int16_t>(m_general_purpose_registers[rs])) * static_cast<int32_t>(imm);
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
        // SF: result is negative
        if (result < 0)
        {
            m_flags = m_flags | SF_MASK;
        }
        // OF + CF: result does not fit in a signed 16-bit value
        if (result < -32768 || result > 32767)
        {
            m_flags = m_flags | OF_MASK | CF_MASK;
        }
        m_general_purpose_registers[rd] = static_cast<uint16_t>(result);
    }
    void CPU::m_inc(RegisterNumber reg) noexcept
    {
        if (reg >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        uint16_t current = m_general_purpose_registers[reg];
        uint16_t result = current + 1;
        m_general_purpose_registers[reg] = result;

        // CF: unsigned wrap-around (0xFFFF + 1)
        if (result < current)
        {
            m_flags = m_flags | CF_MASK;
        }
        // OF: signed overflow (0x7FFF + 1 wraps to negative)
        if (current == 0x7FFF)
        {
            m_flags = m_flags | OF_MASK;
        }
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
    }
    void CPU::m_dec(RegisterNumber reg) noexcept
    {
        if (reg >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        uint16_t current = m_general_purpose_registers[reg];
        uint16_t result = current - 1;
        m_general_purpose_registers[reg] = result;

        // CF: unsigned borrow (0x0000 - 1)
        if (current == 0)
        {
            m_flags = m_flags | CF_MASK;
        }
        // OF: signed overflow (0x8000 - 1 wraps to positive)
        if (current == 0x8000)
        {
            m_flags = m_flags | OF_MASK;
        }
        // ZF
        if (result == 0)
        {
            m_flags = m_flags | ZF_MASK;
        }
    }
    void CPU::m_mov(RegisterNumber rd, RegisterNumber rs) noexcept
    {
        if ((rd >= m_general_purpose_registers.size() && rd != SP_NBR) ||
            (rs >= m_general_purpose_registers.size() && rs != SP_NBR))
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }

        rd != SP_NBR ? m_general_purpose_registers[rd] : m_stack_pointer = m_general_purpose_registers[rs];
    }
    void CPU::m_movi(RegisterNumber rd, uint16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_general_purpose_registers[rd] = imm;
    }
    void CPU::m_str(RegisterNumber rs, RegisterNumber ra) noexcept
    {
        if (rs >= m_general_purpose_registers.size() ||
            ra >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_memory_write_callback(m_general_purpose_registers[rs], m_general_purpose_registers[ra]);
    }
    void CPU::m_ld(RegisterNumber rd, RegisterNumber ra) noexcept
    {
        if (rd >= m_general_purpose_registers.size() ||
            ra >= m_general_purpose_registers.size()
        )
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_general_purpose_registers[rd] = m_memory_read_callback(m_general_purpose_registers[ra]);
    }
    void CPU::m_ldi(RegisterNumber rd, uint16_t imm) noexcept
    {
        if (rd >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_general_purpose_registers[rd] = m_memory_read_callback(imm);
    }
    void CPU::m_push(RegisterNumber reg) noexcept
    {
        if (reg >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_memory_write_callback(m_general_purpose_registers[reg], m_stack_pointer - 1);
        m_stack_pointer--;
    }
    void CPU::m_pop(RegisterNumber reg) noexcept
    {
        if (reg >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_general_purpose_registers[reg] = m_memory_read_callback(m_stack_pointer);
        m_stack_pointer++;
    }
    void CPU::m_out(RegisterNumber rp, RegisterNumber ra, RegisterNumber rb) noexcept
    {
        if (rp >= m_general_purpose_registers.size() ||
            ra >= m_general_purpose_registers.size() ||
            rb >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_port_write_callback(static_cast<uint8_t>(m_general_purpose_registers[rp]),
            m_general_purpose_registers[ra],
            m_general_purpose_registers[rb]
        );
    }
    void CPU::m_in(RegisterNumber rp, RegisterNumber ra, RegisterNumber rb) noexcept
    {
        if (rp >= m_general_purpose_registers.size() ||
            ra >= m_general_purpose_registers.size() ||
            rb >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_general_purpose_registers[rb] = m_port_read_callback(static_cast<uint8_t>(m_general_purpose_registers[rp]),
            static_cast<uint16_t>(m_general_purpose_registers[ra])
        );
    }
    void CPU::m_jmp(RegisterNumber reg) noexcept
    {
        if (reg >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        m_program_counter = m_general_purpose_registers[reg];
    }
    void CPU::m_jmpi(uint16_t imm) noexcept
    {
        m_program_counter = imm;
    }
    void CPU::m_jz(RegisterNumber reg) noexcept
    {
        if (reg >= m_general_purpose_registers.size())
        {
            m_flags = m_flags | II_MASK;
            m_halt();
            return;
        }
        if (m_flags & ZF_MASK)
        {
            m_program_counter = m_general_purpose_registers[reg];
        }
    }
}

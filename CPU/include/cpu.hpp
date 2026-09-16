#ifndef CPU_HPP
#define CPU_HPP

#include <SDL3/SDL_events.h>
#include <cstdint>
#include <array>
#include <functional>

namespace cpu
{
    typedef std::function<void(uint16_t addr, uint16_t data)> MemoryWriteCallback;
    typedef std::function<uint16_t(uint16_t addr)> MemoryReadCallback;

    typedef std::function<void(uint8_t port, uint16_t addr, uint16_t data)> PortWriteCallback;
    typedef std::function<uint16_t(uint8_t port, uint16_t addr)> PortReadCallback;

    typedef uint8_t RegisterNumber;

    constexpr uint16_t RESET_VECTOR = 0xFFFC;

    class CPU
    {
        public:
            CPU(MemoryWriteCallback memory_write_callback,
                MemoryReadCallback memory_read_callback,
                PortWriteCallback port_write_callback,
                PortReadCallback port_read_callback) noexcept;
            // Loop handled by caller
            void tick(void) noexcept;
            bool has_exception(void) const noexcept;
            bool is_halted(void) const noexcept;
            void reset(void) noexcept;
            uint16_t get_program_counter(void) const noexcept;
            const std::array<uint16_t, 8>& get_registers(void) const noexcept;
            uint64_t get_instruction_register(void) const noexcept;
        private:
            std::array<uint16_t, 8> m_general_purpose_registers;
            uint16_t m_program_counter;
            uint16_t m_stack_pointer;
            uint64_t m_instruction_register;
            uint16_t m_flags;

            MemoryWriteCallback m_memory_write_callback;
            MemoryReadCallback m_memory_read_callback;

            PortWriteCallback m_port_write_callback;
            PortReadCallback m_port_read_callback;

            bool m_halted;
            bool m_override_inc;

            // == Instructions implementations ==

            void m_halt(void) noexcept;
            void m_add(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_addi(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept;
            void m_sub(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_subi(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept;
            void m_adds(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_subs(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_addsi(RegisterNumber rd, RegisterNumber rs, int16_t imm) noexcept;
            void m_subsi(RegisterNumber rd, RegisterNumber rs, int16_t imm) noexcept;
            void m_div(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_divi(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept;
            void m_mul(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_muli(RegisterNumber rd, RegisterNumber rs, uint16_t imm) noexcept;
            void m_divs(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_divsi(RegisterNumber rd, RegisterNumber rs, int16_t imm) noexcept;
            void m_muls(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_mulsi(RegisterNumber rd, RegisterNumber rs, int16_t imm) noexcept;
            void m_inc(RegisterNumber reg) noexcept;
            void m_dec(RegisterNumber reg) noexcept;

            void m_mov(RegisterNumber rd, RegisterNumber rs) noexcept;
            void m_movi(RegisterNumber rd, uint16_t imm) noexcept;
            void m_str(RegisterNumber rd, RegisterNumber ra) noexcept;
            void m_stri(RegisterNumber rd, uint16_t imm) noexcept;
            void m_ld(RegisterNumber rd, RegisterNumber ra) noexcept;
            void m_ldi(RegisterNumber rd, uint16_t imm) noexcept;
            void m_push(RegisterNumber reg) noexcept;
            void m_pop(RegisterNumber reg) noexcept;
            void m_out(RegisterNumber rp, RegisterNumber ra, RegisterNumber rb) noexcept;
            void m_in(RegisterNumber rp, RegisterNumber ra, RegisterNumber rb) noexcept;

            void m_jmp(RegisterNumber reg) noexcept;
            void m_jmpi(uint16_t imm) noexcept;
            void m_jc(RegisterNumber reg) noexcept;
            void m_jnc(RegisterNumber reg) noexcept;
            void m_jz(RegisterNumber reg) noexcept;
            void m_jnz(RegisterNumber reg) noexcept;
            void m_ja(RegisterNumber reg) noexcept;
            void m_jae(RegisterNumber reg) noexcept;
            void m_jb(RegisterNumber reg) noexcept;
            void m_jbe(RegisterNumber reg) noexcept;
            void m_jne(RegisterNumber reg) noexcept;
            void m_je(RegisterNumber reg) noexcept;

            void m_and(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_or(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_xor(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_not(RegisterNumber reg) noexcept;
            void m_shr(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;
            void m_shl(RegisterNumber reg1, RegisterNumber reg2, RegisterNumber reg3) noexcept;

            void m_cmp(RegisterNumber reg1, RegisterNumber reg2) noexcept;
            void m_test(RegisterNumber reg1, RegisterNumber reg2) noexcept;
            void m_clf() noexcept;

            void m_msb(RegisterNumber dest, RegisterNumber src) noexcept;
            void m_lsb(RegisterNumber dest, RegisterNumber src) noexcept;

            void m_outi(uint16_t port, uint16_t addr, RegisterNumber data) noexcept;
            void m_jei(uint16_t addr) noexcept;
            void m_jnei(uint16_t addr) noexcept;
            void m_jci(uint16_t addr) noexcept;
            void m_jnci(uint16_t addr) noexcept;
            void m_jzi(uint16_t addr) noexcept;
            void m_jnzi(uint16_t addr) noexcept;
            void m_jai(uint16_t addr) noexcept;
            void m_jaei(uint16_t addr) noexcept;
            void m_jbi(uint16_t addr) noexcept;
            void m_jbei(uint16_t addr) noexcept;
            void m_ini(RegisterNumber dest, uint16_t port, uint16_t addr) noexcept;

            // == Helper functions ==
            void m_reset(void) noexcept;
            void m_fetch(void) noexcept;
            void m_execute(void) noexcept;
            void m_tick(void) noexcept;
    };
}

#endif

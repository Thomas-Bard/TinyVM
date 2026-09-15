# TinyVM — Instruction Set Architecture Reference

> **Source files:** `common/include/isa.hpp` · `CPU/src/cpu_inst.cpp` · `CPU/src/cpu.cpp`

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Registers](#2-registers)
3. [Flags Register](#3-flags-register)
4. [Memory Model](#4-memory-model)
5. [Instruction Encoding](#5-instruction-encoding)
6. [CPU Lifecycle](#6-cpu-lifecycle)
7. [Exceptions & Halting](#7-exceptions--halting)
8. [Instruction Reference](#8-instruction-reference)
   - [Arithmetic — Unsigned](#81-arithmetic--unsigned)
   - [Arithmetic — Signed](#82-arithmetic--signed)
   - [Increment / Decrement](#83-increment--decrement)
   - [Data Transfer](#84-data-transfer)
   - [Stack](#85-stack)
   - [Port I/O](#86-port-io)
   - [Bitwise Operations](#87-bitwise-operations)
   - [Comparison & Test](#88-comparison--test)
   - [Byte Extraction](#89-byte-extraction)
   - [Control Flow](#810-control-flow)
   - [System](#811-system)
9. [Opcode Table](#9-opcode-table)
10. [Flag Summary by Instruction Group](#10-flag-summary-by-instruction-group)

---

## 1. Architecture Overview

TinyVM is a **16-bit, word-addressable** CPU with a fixed-width instruction set.

| Property | Value |
|---|---|
| Native word size | 16 bits |
| Address space | 16-bit (65 536 word-addressable locations) |
| Instruction width | Fixed — 4 words (64 bits) |
| General-purpose registers | 8 × 16-bit (RA – RH) |
| Special registers | PC (16-bit), SP (16-bit), IR (64-bit), FLAGS (16-bit) |
| Reset vector | `0xFFFC` |
| Byte addressability | **None** — every address selects a 16-bit word |

---

## 2. Registers

### General-Purpose Registers

| Name | Number | Notes |
|------|--------|-------|
| RA | `0x00` | General purpose |
| RB | `0x01` | General purpose |
| RC | `0x02` | General purpose |
| RD | `0x03` | General purpose |
| RE | `0x04` | General purpose |
| RF | `0x05` | General purpose |
| RG | `0x06` | General purpose |
| RH | `0x07` | General purpose; **implicitly written by division** (holds remainder) |

> **RH is special:** every `div`, `divi`, `divs`, and `divsi` instruction stores the remainder of the division into RH, regardless of which registers are used for the quotient. Programs that rely on RH should be aware of this side effect.

### Special Registers

| Register | Width | Description |
|----------|-------|-------------|
| PC | 16-bit | Program Counter — points to the first word of the currently-executing instruction |
| SP | 16-bit | Stack Pointer — accessible via `mov` (register number `0x08`); initialized to `0` on reset |
| IR | 64-bit | Instruction Register — holds the raw 64-bit instruction being executed (read-only to programs) |
| FLAGS | 16-bit | Status flags set by instructions (see §3) |

---

## 3. Flags Register

The FLAGS register is a 16-bit bitmask. Individual flags are set by instructions and remain set until explicitly cleared (by `clf`) or overwritten by a subsequent instruction.

| Bit | Mask | Abbreviation | Name | Set when… |
|-----|------|---|---|---|
| 0 | `0b000001` | **CF** | Carry Flag | Unsigned result wrapped around, an unsigned borrow occurred, or a multiplication overflowed 16 bits |
| 1 | `0b000010` | **ZF** | Zero Flag | Result of an operation equals zero |
| 2 | `0b000100` | **OF** | Overflow Flag | Signed arithmetic overflowed (result cannot be represented as a signed 16-bit value), or multiplication overflowed |
| 3 | `0b001000` | **SF** | Sign Flag | Result of a *signed* operation is negative (bit 15 is set) |
| 4 | `0b010000` | **DZ** | Division by Zero | A division instruction was issued with a zero divisor |
| 5 | `0b100000` | **II** | Illegal Instruction | An invalid register number was used, or an unknown opcode was fetched |

> **Important:** Flags are **cumulative** — they are ORed in, never cleared automatically. Use `clf` to reset all flags between operations that test flags independently.

---

## 4. Memory Model

- Memory is **word-addressable**: address `N` refers to the **N-th 16-bit word**, not a byte.
- The full addressable space is `0x0000`–`0xFFFF` (65 536 words = 128 KB).
- Programs are typically loaded starting at address `0x0000`.
- The reset vector is at `0xFFFC` — on startup the CPU reads and executes the instruction at that address (which should contain a `jmpi` to the actual entry point, or the entry point code itself).
- Out-of-bounds memory accesses are silently ignored (writes are discarded, reads return `0`).

### Stack Convention

The stack is a **downward-growing** structure managed by SP:

- **PUSH** pre-decrements SP: stores the register value at `mem[SP − 1]`, then SP becomes `SP − 1`.
- **POP** post-increments SP: reads from `mem[SP]` into the register, then SP becomes `SP + 1`.
- SP is initialized to `0x0000` on reset; the first push targets `mem[0xFFFF]`.

---

## 5. Instruction Encoding

Every instruction occupies exactly **4 consecutive words** (64 bits) in memory. The CPU always advances the program counter by 4 after each instruction (unless a jump is taken).

### Bit Layout (little-endian)

```
Bits [63:48]  Bits [47:32]  Bits [31:16]  Bits [15:0]
─────────────────────────────────────────────────────────
  Operand 2    Operand 1     Operand 0    Opcode (8-bit)
                                          + 8 unused bits
```

| Field | Bits | Width | Description |
|-------|------|-------|-------------|
| Opcode | `[7:0]` | 8 bits | Instruction identifier |
| *(unused)* | `[15:8]` | 8 bits | Reserved, should be zero |
| Operand 0 | `[31:16]` | 16 bits | First operand (register number or immediate) |
| Operand 1 | `[47:32]` | 16 bits | Second operand (register number or immediate) |
| Operand 2 | `[63:48]` | 16 bits | Third operand (register number or immediate) |

- For instructions with fewer than 3 operands, the unused operand slots are ignored.
- Register operands are encoded as their register number (0–7, or 8 for SP) in the low byte of the 16-bit operand word; the high byte is unused.
- Immediate operands occupy the full 16 bits of the operand word.

---

## 6. CPU Lifecycle

### Reset

On `reset()`, the CPU is placed into a known clean state:

| Register | Reset Value |
|----------|-------------|
| PC | `0xFFFC` (RESET_VECTOR) |
| SP | `0x0000` |
| RA – RH | `0x0000` |
| FLAGS | `0x0000` |
| IR | `0x0000_0000_0000_0000` |
| Halted | `false` |

### Tick (Fetch–Execute Cycle)

Each call to `tick()` performs one full fetch–execute cycle:

```
if halted → return immediately

1. FETCH:   Read 4 consecutive words from [PC, PC+1, PC+2, PC+3]
            → assemble them into the 64-bit IR (little-endian)

2. EXECUTE: Decode opcode from IR[7:0]
            → dispatch to the matching instruction handler

3. ADVANCE: if no jump was taken → PC = PC + 4
            if a jump was taken  → PC was already set by the instruction
```

---

## 7. Exceptions & Halting

The CPU stops execution (sets the internal `halted` flag) in the following situations:

| Condition | Flag Set | Cause |
|-----------|----------|-------|
| `halt` instruction executed | — | Normal program termination |
| Invalid register number used | **II** | A register operand value ≥ 8 (and not SP where allowed) |
| Unknown opcode fetched | **II** | The opcode field does not match any defined instruction |
| Division by zero | **DZ** | A `div`/`divi`/`divs`/`divsi` with a zero divisor |

`has_exception()` returns `true` when either **II** or **DZ** is set. A halted CPU without an exception represents a clean program termination.

---

## 8. Instruction Reference

### Notation

| Symbol | Meaning |
|--------|---------|
| `Rd` | Destination register (RA–RH) |
| `Rs`, `Ra`, `Rb` | Source registers (RA–RH) |
| `Rp` | Port register |
| `imm` | 16-bit unsigned immediate |
| `simm` | 16-bit signed immediate (two's complement) |
| `mem[X]` | Memory word at address X |
| `→` | Stores result into |
| *flags* | Lists flags potentially modified |

---

### 8.1 Arithmetic — Unsigned

---

#### `add` — Add (register)

| | |
|--|--|
| **Opcode** | `0x01` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = Ra + Rb` |
| **Flags** | CF, OF, ZF |

Performs unsigned 16-bit addition of `Ra` and `Rb`, storing the result in `Rd`.

- **CF** is set if the addition produces unsigned wrap-around (i.e., `result < Ra`).
- **OF** is set if signed overflow occurs: both operands share the same sign but the result sign differs.
- **ZF** is set if the result is `0`.

---

#### `addi` — Add Immediate (unsigned)

| | |
|--|--|
| **Opcode** | `0x02` |
| **Operands** | `Rd, Rs, imm` |
| **Operation** | `Rd = Rs + imm` |
| **Flags** | CF, OF, ZF |

Unsigned addition of register `Rs` and a 16-bit immediate. Same flag logic as `add`.

---

#### `sub` — Subtract (register)

| | |
|--|--|
| **Opcode** | `0x03` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = Ra - Rb` |
| **Flags** | CF, OF, ZF |

Performs unsigned 16-bit subtraction `Ra − Rb`, storing into `Rd`.

- **CF** is set if a borrow was needed (`Rb > Ra` unsigned).
- **OF** is set if signed overflow occurs: operands have different signs and the result sign differs from `Ra`.
- **ZF** is set if the result is `0`.

---

#### `subi` — Subtract Immediate (unsigned)

| | |
|--|--|
| **Opcode** | `0x04` |
| **Operands** | `Rd, Rs, imm` |
| **Operation** | `Rd = Rs - imm` |
| **Flags** | CF, OF, ZF |

Unsigned subtraction of an immediate from `Rs`. Same flag logic as `sub`.

---

#### `div` — Unsigned Divide (register)

| | |
|--|--|
| **Opcode** | `0x09` |
| **Operands** | `Rd, Rs, Rm` |
| **Operation** | `Rd = Rs / Rm` · `RH = Rs % Rm` |
| **Flags** | ZF, DZ (halt) |

Performs unsigned integer division of `Rs` by divisor `Rm`.

- The **quotient** is stored in `Rd`.
- The **remainder** is stored implicitly in **RH** (regardless of what Rd is).
- If `Rm` is `0`: **DZ** is set and the CPU halts immediately.
- **ZF** is set if the quotient is `0`.

---

#### `divi` — Unsigned Divide Immediate

| | |
|--|--|
| **Opcode** | `0x0A` |
| **Operands** | `Rd, Rs, imm` |
| **Operation** | `Rd = Rs / imm` · `RH = Rs % imm` |
| **Flags** | ZF, DZ (halt) |

Unsigned division of `Rs` by a 16-bit immediate divisor. Same behavior as `div`, except the divisor is an immediate value. Halts with **DZ** if `imm` is `0`.

---

#### `mul` — Unsigned Multiply (register)

| | |
|--|--|
| **Opcode** | `0x0D` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = (Ra × Rb)[15:0]` |
| **Flags** | CF, OF, ZF |

Multiplies `Ra` and `Rb` as unsigned 16-bit values using a 32-bit intermediate result. The low 16 bits are stored in `Rd`.

- **CF** and **OF** are both set if the full 32-bit result exceeds `0xFFFF` (product does not fit in 16 bits).
- **ZF** is set if the result is `0`.

> The upper 16 bits of the product are **discarded**. Use OF/CF to detect overflow.

---

#### `muli` — Unsigned Multiply Immediate

| | |
|--|--|
| **Opcode** | `0x0E` |
| **Operands** | `Rd, Rs, imm` |
| **Operation** | `Rd = (Rs × imm)[15:0]` |
| **Flags** | CF, OF, ZF |

Unsigned multiplication of `Rs` by a 16-bit immediate. Same flag behavior as `mul`.

---

### 8.2 Arithmetic — Signed

Signed variants interpret register values and immediates as **two's complement signed 16-bit integers**. They additionally set the **SF** (Sign Flag) when the result is negative (bit 15 set).

---

#### `adds` — Signed Add (register)

| | |
|--|--|
| **Opcode** | `0x05` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = Ra + Rb` (signed) |
| **Flags** | CF, OF, ZF, SF |

Identical arithmetic to `add`. Additionally sets **SF** if the result's MSB (bit 15) is `1`, indicating a negative result in two's complement.

---

#### `addsi` — Signed Add Immediate

| | |
|--|--|
| **Opcode** | `0x06` |
| **Operands** | `Rd, Rs, simm` |
| **Operation** | `Rd = Rs + simm` (signed) |
| **Flags** | CF, OF, ZF, SF |

Signed addition with a 16-bit signed immediate. Sets SF additionally on a negative result.

---

#### `subs` — Signed Subtract (register)

| | |
|--|--|
| **Opcode** | `0x07` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = Ra - Rb` (signed) |
| **Flags** | CF, OF, ZF, SF |

Identical arithmetic to `sub`. Additionally sets **SF** if the result is negative.

---

#### `subsi` — Signed Subtract Immediate

| | |
|--|--|
| **Opcode** | `0x08` |
| **Operands** | `Rd, Rs, simm` |
| **Operation** | `Rd = Rs - simm` (signed) |
| **Flags** | CF, OF, ZF, SF |

Signed subtraction with a 16-bit signed immediate. Sets SF on a negative result.

---

#### `divs` — Signed Divide (register)

| | |
|--|--|
| **Opcode** | `0x0B` |
| **Operands** | `Rd, R1, R2` |
| **Operation** | `Rd = (int16)R1 / (int16)R2` · `RH = (int16)R1 % (int16)R2` |
| **Flags** | ZF, SF, DZ (halt) |

Signed integer division. Both operands are reinterpreted as signed 16-bit values.

- Quotient → `Rd`; signed remainder → **RH**.
- Halts with **DZ** if `R2` is `0`.
- **ZF** is set if the quotient is `0`.
- **SF** is set if the quotient is negative.

---

#### `divsi` — Signed Divide Immediate

| | |
|--|--|
| **Opcode** | `0x0C` |
| **Operands** | `Rd, R1, simm` |
| **Operation** | `Rd = (int16)R1 / simm` · `RH = (int16)R1 % simm` |
| **Flags** | ZF, SF, DZ (halt) |

Signed division with a signed 16-bit immediate divisor. Halts with **DZ** if `simm` is `0`.

---

#### `muls` — Signed Multiply (register)

| | |
|--|--|
| **Opcode** | `0x0F` |
| **Operands** | `Rd, R1, R2` |
| **Operation** | `Rd = ((int16)R1 × (int16)R2)[15:0]` |
| **Flags** | CF, OF, ZF, SF |

Signed multiplication using a 32-bit signed intermediate. Low 16 bits of result stored in `Rd`.

- **SF** is set if the full 32-bit result is negative.
- **CF** and **OF** are both set if the result falls outside the signed 16-bit range `[−32768, 32767]`.
- **ZF** is set if the result is `0`.

---

#### `mulsi` — Signed Multiply Immediate

| | |
|--|--|
| **Opcode** | `0x10` |
| **Operands** | `Rd, Rs, simm` |
| **Operation** | `Rd = ((int16)Rs × simm)[15:0]` |
| **Flags** | CF, OF, ZF, SF |

Signed multiplication with a signed 16-bit immediate. Same flag behavior as `muls`.

---

### 8.3 Increment / Decrement

---

#### `inc` — Increment

| | |
|--|--|
| **Opcode** | `0x11` |
| **Operands** | `Rn` |
| **Operation** | `Rn = Rn + 1` |
| **Flags** | CF, OF, ZF |

Increments `Rn` by 1 in place.

- **CF** is set on unsigned wrap-around (`0xFFFF` → `0x0000`).
- **OF** is set on signed overflow (`0x7FFF` → `0x8000`, i.e., max positive to min negative).
- **ZF** is set if the result is `0` (i.e., was `0xFFFF`).

---

#### `dec` — Decrement

| | |
|--|--|
| **Opcode** | `0x12` |
| **Operands** | `Rn` |
| **Operation** | `Rn = Rn - 1` |
| **Flags** | CF, OF, ZF |

Decrements `Rn` by 1 in place.

- **CF** is set on unsigned borrow (`0x0000` → `0xFFFF`).
- **OF** is set on signed overflow (`0x8000` → `0x7FFF`, i.e., min negative to max positive).
- **ZF** is set if the result is `0` (i.e., was `0x0001`).

---

### 8.4 Data Transfer

---

#### `mov` — Move Register to Register

| | |
|--|--|
| **Opcode** | `0x13` |
| **Operands** | `Rd, Rs` |
| **Operation** | `Rd = Rs` |
| **Flags** | None |

Copies the value of `Rs` into `Rd`. `Rd` may be **SP** (register number `0x08`), allowing the stack pointer to be set from a general-purpose register.

---

#### `movi` — Move Immediate

| | |
|--|--|
| **Opcode** | `0x14` |
| **Operands** | `Rd, imm` |
| **Operation** | `Rd = imm` |
| **Flags** | None |

Loads a 16-bit immediate value directly into `Rd`. This is the primary way to load a constant into a register.

---

#### `str` — Store Register to Memory

| | |
|--|--|
| **Opcode** | `0x15` |
| **Operands** | `Ra, Rb` |
| **Operation** | `mem[Ra] = Rb` |
| **Flags** | None |

Stores the value of `Rb` into the memory word at the address held in `Ra`. `Ra` provides the address; `Rb` provides the data.

---

#### `stri` — Store Immediate to Memory

| | |
|--|--|
| **Opcode** | `0x16` |
| **Operands** | `Ra, imm` |
| **Operation** | `mem[Ra] = imm` |
| **Flags** | None |

Stores a 16-bit immediate value into the memory word at the address held in `Ra`.

---

#### `ld` — Load from Memory (register address)

| | |
|--|--|
| **Opcode** | `0x17` |
| **Operands** | `Rd, Ra` |
| **Operation** | `Rd = mem[Ra]` |
| **Flags** | None |

Loads the memory word at the address held in `Ra` into `Rd`.

---

#### `ldi` — Load from Memory (immediate address)

| | |
|--|--|
| **Opcode** | `0x31` |
| **Operands** | `Rd, imm` |
| **Operation** | `Rd = mem[imm]` |
| **Flags** | None |

Loads the memory word at the immediate address `imm` into `Rd`.

---

### 8.5 Stack

The stack grows **downward**. SP points to the next available (free) slot.

---

#### `push` — Push Register onto Stack

| | |
|--|--|
| **Opcode** | `0x18` |
| **Operands** | `Rn` |
| **Operation** | `mem[SP − 1] = Rn` · `SP = SP − 1` |
| **Flags** | None |

Pushes the value in `Rn` onto the stack. SP is decremented after the write.

---

#### `pop` — Pop from Stack into Register

| | |
|--|--|
| **Opcode** | `0x19` |
| **Operands** | `Rn` |
| **Operation** | `Rn = mem[SP]` · `SP = SP + 1` |
| **Flags** | None |

Pops the top value from the stack into `Rn`. SP is incremented after the read.

---

### 8.6 Port I/O

The CPU communicates with peripherals through a port-based I/O system. Each port is identified by an **8-bit port number**. Each port exposes a **16-bit address space** (the address selects a register or location within the device). Data transferred is always **16 bits**.

---

#### `out` — Output to Port (register operands)

| | |
|--|--|
| **Opcode** | `0x1A` |
| **Operands** | `Rp, Ra, Rb` |
| **Operation** | `port[Rp][Ra] ← Rb` |
| **Flags** | None |

Sends the value of `Rb` to the device on port `Rp` at device address `Ra`. All three operands are registers.

---

#### `outi` — Output to Port (immediate port and address)

| | |
|--|--|
| **Opcode** | `0x34` |
| **Operands** | `port_imm, addr_imm, Rdata` |
| **Operation** | `port[port_imm][addr_imm] ← Rdata` |
| **Flags** | None |

> **Operand order is unusual:** Operand 0 = port (immediate), Operand 1 = address (immediate), Operand 2 = data (register).

Sends the value of register `Rdata` to the device on the immediate port at the immediate address. Port and address are both encoded as 16-bit immediates; the port number uses only the low 8 bits.

---

#### `in` — Input from Port (register operands)

| | |
|--|--|
| **Opcode** | `0x1B` |
| **Operands** | `Rp, Ra, Rb` |
| **Operation** | `Rb = port[Rp][Ra]` |
| **Flags** | None |

Reads a 16-bit value from the device on port `Rp` at device address `Ra`, storing the result in `Rb`. Rp and Ra provide the port and address; Rb is the destination.

---

#### `ini` — Input from Port (immediate port and address)

| | |
|--|--|
| **Opcode** | `0x3F` |
| **Operands** | `Rdest, port_imm, addr_imm` |
| **Operation** | `Rdest = port[port_imm][addr_imm]` |
| **Flags** | None |

> **Operand order is unusual:** Operand 0 = destination (register), Operand 1 = port (immediate), Operand 2 = address (immediate).

Reads from the device on the immediate port at the immediate address, storing the result in `Rdest`.

---

### 8.7 Bitwise Operations

---

#### `and` — Bitwise AND

| | |
|--|--|
| **Opcode** | `0x28` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = Ra & Rb` |
| **Flags** | ZF |

Bitwise AND of `Ra` and `Rb`. **ZF** is set if the result is `0`.

---

#### `or` — Bitwise OR

| | |
|--|--|
| **Opcode** | `0x29` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = Ra \| Rb` |
| **Flags** | ZF |

Bitwise OR of `Ra` and `Rb`. **ZF** is set if the result is `0`.

---

#### `xor` — Bitwise XOR

| | |
|--|--|
| **Opcode** | `0x2A` |
| **Operands** | `Rd, Ra, Rb` |
| **Operation** | `Rd = Ra ^ Rb` |
| **Flags** | ZF |

Bitwise XOR of `Ra` and `Rb`. **ZF** is set if the result is `0`.

> **Tip:** `xor Rn, Rn, Rn` zeroes `Rn` and sets ZF.

---

#### `not` — Bitwise NOT

| | |
|--|--|
| **Opcode** | `0x2B` |
| **Operands** | `Rn` |
| **Operation** | `Rn = ~Rn` |
| **Flags** | ZF |

Inverts all 16 bits of `Rn` in place. **ZF** is set if the result is `0` (only when `Rn` was `0xFFFF`).

---

#### `shl` — Shift Left

| | |
|--|--|
| **Opcode** | `0x2D` |
| **Operands** | `Rd, Rs, Rshift` |
| **Operation** | `Rd = Rs << Rshift` |
| **Flags** | ZF, OF |

Logically shifts `Rs` left by the number of positions in `Rshift`. The shift is computed through a 32-bit intermediate; the low 16 bits are stored in `Rd`.

- **OF** is set if the 32-bit result exceeds `0xFFFF` (bits were shifted out).
- **ZF** is set if the result is `0`.

---

#### `shr` — Shift Right

| | |
|--|--|
| **Opcode** | `0x2C` |
| **Operands** | `Rd, Rs, Rshift` |
| **Operation** | `Rd = Rs >> Rshift` |
| **Flags** | ZF, OF |

Logically shifts `Rs` right by the number of positions in `Rshift`. The result is always truncated to 16 bits.

- **ZF** is set if the result is `0`.
- **OF** is set if the 32-bit intermediate result exceeds `0xFFFF` (in practice this cannot happen for a right shift of a 16-bit value).

---

### 8.8 Comparison & Test

These instructions update flags **without modifying any general-purpose register**.

---

#### `cmp` — Compare

| | |
|--|--|
| **Opcode** | `0x2E` |
| **Operands** | `Ra, Rb` |
| **Operation** | *(flags only, no result stored)* |
| **Flags** | ZF, CF |

Compares `Ra` and `Rb` as unsigned values:

- **ZF** is set if `Ra == Rb`.
- **CF** is set if `Ra < Rb` (unsigned).

`cmp` is the primary instruction for driving the conditional jump family (`jb`, `jbe`, `ja`, `jae`, `je`, `jne`).

---

#### `test` — Bit Test

| | |
|--|--|
| **Opcode** | `0x2F` |
| **Operands** | `Ra, Rb` |
| **Operation** | *(flags only, no result stored)* |
| **Flags** | ZF |

Computes `Ra & Rb` and discards the result. **ZF** is set if the result is `0` (i.e., the two values share no set bits). Useful for testing individual bits: `test Rn, Rn` sets ZF if `Rn` is zero.

---

#### `clf` — Clear Flags

| | |
|--|--|
| **Opcode** | `0x30` |
| **Operands** | *(none)* |
| **Operation** | `FLAGS = 0` |
| **Flags** | All cleared |

Resets every flag to `0`. Use this before a sequence of instructions whose flags must not be contaminated by earlier operations.

---

### 8.9 Byte Extraction

These instructions extract individual bytes from a 16-bit register into the **low byte** of the destination, zero-extending the result.

---

#### `msb` — Extract Most-Significant Byte

| | |
|--|--|
| **Opcode** | `0x32` |
| **Operands** | `Rd, Rs` |
| **Operation** | `Rd = (Rs >> 8) & 0x00FF` |
| **Flags** | None |

Copies the high byte of `Rs` into the low byte of `Rd`. The high byte of `Rd` is set to `0`.

---

#### `lsb` — Extract Least-Significant Byte

| | |
|--|--|
| **Opcode** | `0x33` |
| **Operands** | `Rd, Rs` |
| **Operation** | `Rd = Rs & 0x00FF` |
| **Flags** | None |

Copies the low byte of `Rs` into the low byte of `Rd`. The high byte of `Rd` is set to `0`.

---

### 8.10 Control Flow

All jump instructions load a new value into PC and **suppress** the normal PC+4 advancement. If the jump condition is not met, execution continues sequentially at the next instruction (PC+4).

Each conditional jump exists in two forms:
- **Register variant** — the jump target is read from a register.
- **Immediate variant** (`i` suffix) — the jump target is encoded as a 16-bit immediate in the instruction.

---

#### `jmp` / `jmpi` — Unconditional Jump

| Mnemonic | Opcode | Target |
|----------|--------|--------|
| `jmp Rn` | `0x1C` | Register |
| `jmpi imm` | `0x1D` | Immediate |

Sets PC to the target address unconditionally.

---

#### Conditional Jumps

All conditional jumps test the FLAGS register. The jump is taken only when the stated condition holds.

| Register form | Immediate form | Opcode (reg / imm) | Condition | Meaning after `cmp Ra, Rb` |
|---|---|---|---|---|
| `jz Rn` | `jzi imm` | `0x20` / `0x39` | ZF = 1 | Ra == Rb (or result was 0) |
| `jnz Rn` | `jnzi imm` | `0x21` / `0x3A` | ZF = 0 | Ra ≠ Rb (or result non-zero) |
| `je Rn` | `jei imm` | `0x27` / `0x35` | ZF = 1 | Ra == Rb *(alias for `jz`/`jzi`)* |
| `jne Rn` | `jnei imm` | `0x26` / `0x36` | ZF = 0 | Ra ≠ Rb *(alias for `jnz`/`jnzi`)* |
| `jc Rn` | `jci imm` | `0x1E` / `0x37` | CF = 1 | Carry / unsigned borrow |
| `jnc Rn` | `jnci imm` | `0x1F` / `0x38` | CF = 0 | No carry / no borrow |
| `ja Rn` | `jai imm` | `0x22` / `0x3B` | CF = 0 AND ZF = 0 | Ra > Rb (unsigned, strictly above) |
| `jae Rn` | `jaei imm` | `0x23` / `0x3C` | CF = 0 | Ra ≥ Rb (unsigned, above or equal) |
| `jb Rn` | `jbi imm` | `0x24` / `0x3D` | CF = 1 AND ZF = 0 | Ra < Rb (unsigned, strictly below) |
| `jbe Rn` | `jbei imm` | `0x25` / `0x3E` | CF = 1 | Ra < Rb (unsigned below; see note) |

> **Note on `jbe`/`jbei`:** These instructions fire when CF is set. After a `cmp Ra, Rb`, CF is set only when `Ra < Rb`, and ZF is set only when `Ra == Rb` (in which case CF is clear). Therefore, `jbe` effectively behaves as a strict "below" branch in a CMP context — it does **not** fire on equality. Use `jb` or `jae`+`je` combinations if you need true below-or-equal semantics.

---

### 8.11 System

#### `halt` — Halt CPU

| | |
|--|--|
| **Opcode** | `0x00` |
| **Operands** | *(none)* |
| **Operation** | CPU halts, no further ticks are processed |
| **Flags** | None |

Cleanly stops the CPU. The halted state is not an error; `has_exception()` returns `false` after a normal halt. Execution can only resume via an external `reset()`.

---

## 9. Opcode Table

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| `0x00` | `halt` | — | Halt CPU |
| `0x01` | `add` | Rd, Ra, Rb | Unsigned add (register) |
| `0x02` | `addi` | Rd, Rs, imm | Unsigned add (immediate) |
| `0x03` | `sub` | Rd, Ra, Rb | Unsigned subtract (register) |
| `0x04` | `subi` | Rd, Rs, imm | Unsigned subtract (immediate) |
| `0x05` | `adds` | Rd, Ra, Rb | Signed add (register) |
| `0x06` | `addsi` | Rd, Rs, simm | Signed add (immediate) |
| `0x07` | `subs` | Rd, Ra, Rb | Signed subtract (register) |
| `0x08` | `subsi` | Rd, Rs, simm | Signed subtract (immediate) |
| `0x09` | `div` | Rd, Rs, Rm | Unsigned divide; RH ← remainder |
| `0x0A` | `divi` | Rd, Rs, imm | Unsigned divide immediate; RH ← remainder |
| `0x0B` | `divs` | Rd, R1, R2 | Signed divide; RH ← remainder |
| `0x0C` | `divsi` | Rd, R1, simm | Signed divide immediate; RH ← remainder |
| `0x0D` | `mul` | Rd, Ra, Rb | Unsigned multiply (register) |
| `0x0E` | `muli` | Rd, Rs, imm | Unsigned multiply (immediate) |
| `0x0F` | `muls` | Rd, R1, R2 | Signed multiply (register) |
| `0x10` | `mulsi` | Rd, Rs, simm | Signed multiply (immediate) |
| `0x11` | `inc` | Rn | Increment register |
| `0x12` | `dec` | Rn | Decrement register |
| `0x13` | `mov` | Rd, Rs | Register to register copy (Rd may be SP) |
| `0x14` | `movi` | Rd, imm | Load immediate into register |
| `0x15` | `str` | Ra, Rb | Store Rb → mem[Ra] |
| `0x16` | `stri` | Ra, imm | Store imm → mem[Ra] |
| `0x17` | `ld` | Rd, Ra | Load mem[Ra] → Rd |
| `0x18` | `push` | Rn | Push Rn onto stack |
| `0x19` | `pop` | Rn | Pop from stack into Rn |
| `0x1A` | `out` | Rp, Ra, Rb | Write Rb to port Rp at address Ra |
| `0x1B` | `in` | Rp, Ra, Rb | Read from port Rp at address Ra → Rb |
| `0x1C` | `jmp` | Rn | Unconditional jump to address in Rn |
| `0x1D` | `jmpi` | imm | Unconditional jump to immediate address |
| `0x1E` | `jc` | Rn | Jump if CF |
| `0x1F` | `jnc` | Rn | Jump if !CF |
| `0x20` | `jz` | Rn | Jump if ZF |
| `0x21` | `jnz` | Rn | Jump if !ZF |
| `0x22` | `ja` | Rn | Jump if !CF && !ZF (above) |
| `0x23` | `jae` | Rn | Jump if !CF (above or equal) |
| `0x24` | `jb` | Rn | Jump if CF && !ZF (below) |
| `0x25` | `jbe` | Rn | Jump if CF (see note in §8.10) |
| `0x26` | `jne` | Rn | Jump if !ZF (not equal, alias jnz) |
| `0x27` | `je` | Rn | Jump if ZF (equal, alias jz) |
| `0x28` | `and` | Rd, Ra, Rb | Bitwise AND |
| `0x29` | `or` | Rd, Ra, Rb | Bitwise OR |
| `0x2A` | `xor` | Rd, Ra, Rb | Bitwise XOR |
| `0x2B` | `not` | Rn | Bitwise NOT |
| `0x2C` | `shr` | Rd, Rs, Rshift | Logical shift right |
| `0x2D` | `shl` | Rd, Rs, Rshift | Logical shift left |
| `0x2E` | `cmp` | Ra, Rb | Compare (sets ZF/CF, no result) |
| `0x2F` | `test` | Ra, Rb | Bit test AND (sets ZF, no result) |
| `0x30` | `clf` | — | Clear all flags |
| `0x31` | `ldi` | Rd, imm | Load from immediate memory address |
| `0x32` | `msb` | Rd, Rs | Extract high byte of Rs into Rd |
| `0x33` | `lsb` | Rd, Rs | Extract low byte of Rs into Rd |
| `0x34` | `outi` | port_imm, addr_imm, Rdata | Write to port (imm port & addr, reg data) |
| `0x35` | `jei` | imm | Jump if ZF (equal, immediate target) |
| `0x36` | `jnei` | imm | Jump if !ZF (not equal, immediate target) |
| `0x37` | `jci` | imm | Jump if CF (carry, immediate target) |
| `0x38` | `jnci` | imm | Jump if !CF (no carry, immediate target) |
| `0x39` | `jzi` | imm | Jump if ZF (zero, immediate target) |
| `0x3A` | `jnzi` | imm | Jump if !ZF (not zero, immediate target) |
| `0x3B` | `jai` | imm | Jump if !CF && !ZF (above, immediate) |
| `0x3C` | `jaei` | imm | Jump if !CF (above or equal, immediate) |
| `0x3D` | `jbi` | imm | Jump if CF && !ZF (below, immediate) |
| `0x3E` | `jbei` | imm | Jump if CF (immediate, see note §8.10) |
| `0x3F` | `ini` | Rdest, port_imm, addr_imm | Read from port (imm port & addr, reg dest) |

---

## 10. Flag Summary by Instruction Group

| Instruction(s) | CF | ZF | OF | SF | DZ | II |
|---|:---:|:---:|:---:|:---:|:---:|:---:|
| `add`, `addi` | ✓ | ✓ | ✓ | — | — | ✓ |
| `sub`, `subi` | ✓ | ✓ | ✓ | — | — | ✓ |
| `adds`, `addsi` | ✓ | ✓ | ✓ | ✓ | — | ✓ |
| `subs`, `subsi` | ✓ | ✓ | ✓ | ✓ | — | ✓ |
| `mul`, `muli` | ✓ | ✓ | ✓ | — | — | ✓ |
| `muls`, `mulsi` | ✓ | ✓ | ✓ | ✓ | — | ✓ |
| `div`, `divi` | — | ✓ | — | — | ✓ | ✓ |
| `divs`, `divsi` | — | ✓ | — | ✓ | ✓ | ✓ |
| `inc`, `dec` | ✓ | ✓ | ✓ | — | — | ✓ |
| `and`, `or`, `xor`, `not` | — | ✓ | — | — | — | ✓ |
| `shl`, `shr` | — | ✓ | ✓ | — | — | ✓ |
| `cmp` | ✓ | ✓ | — | — | — | ✓ |
| `test` | — | ✓ | — | — | — | ✓ |
| `clf` | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ |
| `mov`, `movi`, `str`, `stri`, `ld`, `ldi` | — | — | — | — | — | ✓ |
| `push`, `pop` | — | — | — | — | — | ✓ |
| `out`, `outi`, `in`, `ini` | — | — | — | — | — | ✓ |
| `jmp`, `jmpi`, all conditional jumps | — | — | — | — | — | ✓* |
| `halt` | — | — | — | — | — | — |
| `msb`, `lsb` | — | — | — | — | — | ✓ |

**Legend:**
- ✓ = may be set by this instruction
- ✗ = explicitly cleared (only `clf`)
- — = not affected
- ✓\* = II is set and CPU halts only if a register-form jump uses an invalid register number; immediate-form jumps never set II

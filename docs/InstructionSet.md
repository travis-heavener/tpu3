# Instruction Set

## About

This file shows the complete instruction set for the 32-bit TPU3 architecture.

## Terminology

- reg8/reg16/reg32: 8/16/32-bit register
- imm8/imm16/imm32: 8/16/32-bit immediate value
- label: a named label corresponding to a 32-bit memory address
- addr: a 32-bit memory address

## Instruction Memory Diagram

Refer to the following diagram for how the specific bits of each instruction are laid out:

| Bits    | Field     |
|---------|-----------|
| 0..7    | Opcode    |
| 8..10   | MOD       |
| 11      | S (sign)  |
| 12..15  | Reserved  |

**Visual Diagram:**
```
0             7 8    10 11 12          15
[   OPCODE    ] [ MOD ] [S] [ Reserved ]

Total: 2 bytes (16 bits) + arguments
```

## General Instructions

\* Unimplemented

| Instruction | Op. A | Op. B | OpCode | MOD | Signed? | Description |
|-------------|-------|-------|--------|-----|---------|-------------|
| nop         |    -- |    -- |   0x00 |  -- |      -- | No operation. |
| hlt         |    -- |    -- |   0x01 |  -- |      -- | Stops CPU clock. |
| syscall*    |  imm8 |    -- |   0x02 |  -- |      -- | Triggers a syscall, entering kernel mode for protected instructions. See `call`. |
| sysret*     |       |    -- |   0x03 |  -- |      -- | Returns from a syscall, exiting kernel mode and returning to user mode. See `ret`. |
| call        | label |    -- |   0x04 |  -- |      -- | Moves the IP to the label's address, storing the current IP in RP. |
| ret         |    -- |    -- |   0x05 |  -- |      -- | Reverts the IP to the previous memory address stored in RP. |
| jmp         | label |    -- |   0x06 |   0 |      -- | Moves the IP to the label's address. |
| jz          | label |    -- |   0x06 |   1 |      -- | Moves the IP to the label's address, if the zero flag (ZF) is set. |
| jnz         | label |    -- |   0x06 |   2 |      -- | Moves the IP to the label's address, if the zero flag (ZF) is cleared. |
| jc          | label |    -- |   0x06 |   3 |      -- | Moves the IP to the label's address, if the carry flag (CF) is set. |
| jnc         | label |    -- |   0x06 |   4 |      -- | Moves the IP to the label's address, if the carry flag (CF) is cleared. |
| jo          | label |    -- |   0x06 |   5 |      -- | Moves the IP to the label's address, if the overflow flag (OF) is set. |
| jno         | label |    -- |   0x06 |   6 |      -- | Moves the IP to the label's address, if the overflow flag (OF) is cleared. |
| mov         |  reg8 |  imm8 |   0x07 |   0 |      -- | Moves an imm8 value into a reg8. |
| movw        | reg16 | imm16 |   0x07 |   1 |      -- | Moves an imm16 value into a reg16. |
| movdw       | reg32 | imm32 |   0x07 |   2 |      -- | Moves an imm32 value into a reg32. |
| mov         |  reg8 |  reg8 |   0x07 |   3 |      -- | Moves an 8-bit value between two reg8. |
| movw        | reg16 | reg16 |   0x07 |   4 |      -- | Moves an 16-bit value between two reg16. |
| movdw       | reg32 | reg32 |   0x07 |   5 |      -- | Moves an 32-bit value between two reg32. |
| lb          |  reg8 |  addr |   0x08 |   0 |      -- | Loads a byte from a memory address into a reg8. |
| lw          | reg16 |  addr |   0x08 |   1 |      -- | Loads a word starting at a memory address into a reg16. |
| ldw         | reg32 |  addr |   0x08 |   2 |      -- | Loads a dword starting at a memory address into a reg32. |
| sb          |  reg8 |  addr |   0x09 |   0 |      -- | Saves a byte from a reg8 to an address. |
| sw          | reg16 |  addr |   0x09 |   1 |      -- | Saves a word from a reg16 to an address. |
| sdw         | reg32 |  addr |   0x09 |   2 |      -- | Saves a dword from a reg32 to an address. |
| push        |  reg8 |    -- |   0x0A |   0 |      -- | Pushes the value of a reg8 onto the top of the stack. |
|          -- |  imm8 |    -- |   0x0A |   1 |      -- | Pushes an imm8 value onto the top of the stack. |
| pushw       | reg16 |    -- |   0x0A |   2 |      -- | Pushes the value of a reg16 onto the top of the stack. |
|          -- | imm16 |    -- |   0x0A |   3 |      -- | Pushes an imm16 value onto the top of the stack. |
| pushdw      | reg32 |    -- |   0x0A |   4 |      -- | Pushes the value of a reg32 the top of the stack. |
|          -- | imm32 |    -- |   0x0A |   5 |      -- | Pushes an imm32 value onto the top of the stack. |
| pop         |  reg8 |    -- |   0x0B |   0 |      -- | Pops the top byte of the stack into a reg8. |
|          -- |    -- |    -- |   0x0B |   1 |      -- | Discards the top byte of the stack. |
| popw        | reg16 |    -- |   0x0B |   2 |      -- | Pops the top word of the stack into a reg16. |
|          -- |    -- |    -- |   0x0B |   3 |      -- | Discards the top word of the stack. |
| popdw       | reg32 |    -- |   0x0B |   4 |      -- | Pops the top dword of the stack into a reg32. |
|          -- |    -- |    -- |   0x0B |   5 |      -- | Discards the top dword of the stack. |

## Bitwise & Arithmetic Instructions

| Instruction | Op. A | Op. B | OpCode | MOD | Signed? | Description |
|-------------|-------|-------|--------|-----|---------|-------------|
| cmp         |  reg8 |  imm8 |   0x21 |   0 |  No (0) | Compares a reg8 and imm8. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg16 | imm16 |   0x21 |   1 |  No (0) | Compares a reg16 and imm16. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg32 | imm32 |   0x21 |   2 |  No (0) | Compares a reg32 and imm32. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- |  reg8 |  reg8 |   0x21 |   3 |  No (0) | Compares two reg8. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg16 | reg16 |   0x21 |   4 |  No (0) | Compares two reg16. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg32 | reg32 |   0x21 |   5 |  No (0) | Compares two reg32. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| scmp        |  reg8 |  imm8 |   0x21 |   0 | Yes (1) | Compares a reg8 and imm8. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg16 | imm16 |   0x21 |   1 | Yes (1) | Compares a reg16 and imm16. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg32 | imm32 |   0x21 |   2 | Yes (1) | Compares a reg32 and imm32. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- |  reg8 |  reg8 |   0x21 |   3 | Yes (1) | Compares two reg8. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg16 | reg16 |   0x21 |   4 | Yes (1) | Compares two reg16. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
|          -- | reg32 | reg32 |   0x21 |   5 | Yes (1) | Compares two reg32. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| and         |  reg8 |  imm8 |   0x22 |   0 |      -- | Bitwise AND on a reg8 and imm8, stored in first reg8. |
|          -- | reg16 | imm16 |   0x22 |   1 |      -- | Bitwise AND on a reg16 and imm16, stored in first reg16. |
|          -- | reg32 | imm32 |   0x22 |   2 |      -- | Bitwise AND on a reg32 and imm32, stored in first reg32. |
|          -- |  reg8 |  reg8 |   0x22 |   3 |      -- | Bitwise AND on two reg8, stored in first reg8. |
|          -- | reg16 | reg16 |   0x22 |   4 |      -- | Bitwise AND on two reg16, stored in first reg16. |
|          -- | reg32 | reg32 |   0x22 |   5 |      -- | Bitwise AND on two reg32, stored in first reg32. |
| or          |  reg8 |  imm8 |   0x23 |   0 |      -- | Bitwise OR on a reg8 and imm8, stored in first reg8. |
|          -- | reg16 | imm16 |   0x23 |   1 |      -- | Bitwise OR on a reg16 and imm16, stored in first reg16. |
|          -- | reg32 | imm32 |   0x23 |   2 |      -- | Bitwise OR on a reg32 and imm32, stored in first reg32. |
|          -- |  reg8 |  reg8 |   0x23 |   3 |      -- | Bitwise OR on two reg8, stored in first reg8. |
|          -- | reg16 | reg16 |   0x23 |   4 |      -- | Bitwise OR on two reg16, stored in first reg16. |
|          -- | reg32 | reg32 |   0x23 |   5 |      -- | Bitwise OR on two reg32, stored in first reg32. |
| xor         |  reg8 |  imm8 |   0x24 |   0 |      -- | Bitwise XOR on a reg8 and imm8, stored in first reg8. |
|          -- | reg16 | imm16 |   0x24 |   1 |      -- | Bitwise XOR on a reg16 and imm16, stored in first reg16. |
|          -- | reg32 | imm32 |   0x24 |   2 |      -- | Bitwise XOR on a reg32 and imm32, stored in first reg32. |
|          -- |  reg8 |  reg8 |   0x24 |   3 |      -- | Bitwise XOR on two reg8, stored in first reg8. |
|          -- | reg16 | reg16 |   0x24 |   4 |      -- | Bitwise XOR on two reg16, stored in first reg16. |
|          -- | reg32 | reg32 |   0x24 |   5 |      -- | Bitwise XOR on two reg32, stored in first reg32. |
| not         |  reg8 |    -- |   0x25 |   0 |      -- | Bitwise NOT on a reg8, stored in place. |
|          -- | reg16 |    -- |   0x25 |   1 |      -- | Bitwise NOT on a reg16, stored in place. |
|          -- | reg32 |    -- |   0x25 |   2 |      -- | Bitwise NOT on a reg32, stored in place. |
| add         |  reg8 |  imm8 |   0x2A |   0 |  No (0) | Adds a reg8 and imm8, stored in first reg8. |
|          -- | reg16 | imm16 |   0x2A |   1 |  No (0) | Adds a reg16 and imm16, stored in first reg16. |
|          -- | reg32 | imm32 |   0x2A |   2 |  No (0) | Adds a reg32 and imm32, stored in first reg32. |
|          -- |  reg8 |  reg8 |   0x2A |   3 |  No (0) | Adds two reg8, stored in first reg8. |
|          -- | reg16 | reg16 |   0x2A |   4 |  No (0) | Adds two reg16, stored in first reg16. |
|          -- | reg32 | reg32 |   0x2A |   5 |  No (0) | Adds two reg32, stored in first reg32. |
| sadd        |  reg8 |  imm8 |   0x2A |   0 | Yes (1) | Adds a reg8 and imm8, stored in first reg8. |
|          -- | reg16 | imm16 |   0x2A |   1 | Yes (1) | Adds a reg16 and imm16, stored in first reg16. |
|          -- | reg32 | imm32 |   0x2A |   2 | Yes (1) | Adds a reg32 and imm32, stored in first reg32. |
|          -- |  reg8 |  reg8 |   0x2A |   3 | Yes (1) | Adds two reg8, stored in first reg8. |
|          -- | reg16 | reg16 |   0x2A |   4 | Yes (1) | Adds two reg16, stored in first reg16. |
|          -- | reg32 | reg32 |   0x2A |   5 | Yes (1) | Adds two reg32, stored in first reg32. |
| sub         |  reg8 |  imm8 |   0x2B |   0 |  No (0) | Subtracts an imm8 from a reg8, stored in first reg8. |
|          -- | reg16 | imm16 |   0x2B |   1 |  No (0) | Subtracts an imm16 from a reg16, stored in first reg16. |
|          -- | reg32 | imm32 |   0x2B |   2 |  No (0) | Subtracts an imm32 from a reg32, stored in first reg32. |
|          -- |  reg8 |  reg8 |   0x2B |   3 |  No (0) | Subtracts the second reg8 from the first, stored in first reg8. |
|          -- | reg16 | reg16 |   0x2B |   4 |  No (0) | Subtracts the second reg16 from the first, stored in first reg16. |
|          -- | reg32 | reg32 |   0x2B |   5 |  No (0) | Subtracts the second reg32 from the first, stored in first reg32. |
| ssub        |  reg8 |  imm8 |   0x2B |   0 | Yes (1) | Subtracts an imm8 from a reg8, stored in first reg8. |
|          -- | reg16 | imm16 |   0x2B |   1 | Yes (1) | Subtracts an imm16 from a reg16, stored in first reg16. |
|          -- | reg32 | imm32 |   0x2B |   2 | Yes (1) | Subtracts an imm32 from a reg32, stored in first reg32. |
|          -- |  reg8 |  reg8 |   0x2B |   3 | Yes (1) | Subtracts the second reg8 from the first, stored in first reg8. |
|          -- | reg16 | reg16 |   0x2B |   4 | Yes (1) | Subtracts the second reg16 from the first, stored in first reg16. |
|          -- | reg32 | reg32 |   0x2B |   5 | Yes (1) | Subtracts the second reg32 from the first, stored in first reg32. |
| mul         |  imm8 |    -- |   0x2C |   0 |  No (0) | Multiplies AL by an imm8, storing the result in AX. |
|          -- | imm16 |    -- |   0x2C |   1 |  No (0) | Multiplies AX by an imm16, storing the result in EAX. |
|          -- | imm32 |    -- |   0x2C |   2 |  No (0) | Multiplies EAX by an imm32, storing the lower half of the result in EAX and the upper half in EDX. |
|          -- |  reg8 |    -- |   0x2C |   3 |  No (0) | Multiplies AL by a reg8, storing the result in AX. |
|          -- | reg16 |    -- |   0x2C |   4 |  No (0) | Multiplies AX by a reg6, storing the result in EAX. |
|          -- | reg32 |    -- |   0x2C |   5 |  No (0) | Multiplies EAX by a reg32, storing the lower half of the result in EAX and the upper half in EDX. |
| smul        |  imm8 |    -- |   0x2C |   0 | Yes (1) | Multiplies AL by an imm8, storing the result in AX. |
|          -- | imm16 |    -- |   0x2C |   1 | Yes (1) | Multiplies AX by an imm16, storing the result in EAX. |
|          -- | imm32 |    -- |   0x2C |   2 | Yes (1) | Multiplies EAX by an imm32, storing the lower half of the result in EAX and the upper half in EDX. |
|          -- |  reg8 |    -- |   0x2C |   3 | Yes (1) | Multiplies AL by a reg8, storing the result in AX. |
|          -- | reg16 |    -- |   0x2C |   4 | Yes (1) | Multiplies AX by a reg6, storing the result in EAX. |
|          -- | reg32 |    -- |   0x2C |   5 | Yes (1) | Multiplies EAX by a reg32, storing the lower half of the result in EAX and the upper half in EDX. |

# Instruction Set

## About

This file shows the complete instruction set for the 32-bit TPU3 architecture.

## Terminology

- reg8/reg16/reg32: 8/16/32-bit register
    - Ex: AX, ESP, SI
- imm8/imm16/imm32: 8/16/32-bit unsigned immediate value
    - Ex: 1024, 0xBEEF
- simm8/simm16/simm32: 8/16/32-bit signed immediate value (MUST have a sign, +/-)
    - Ex: -1024, +0xBEEF
- ximm8/ximm16/ximm32: refers to either signed OR unsigned immediate values
- label: a symbolic location resolved by the assembler to a rel32 offset of the IP
    - Ex: _start, my_label8
- rel32: a simm32 offset from a reg32
    - If relative to IP, IP's value is the address AFTER the current instruction
    - Stores the reg32 code in a byte, then the 4-byte simm32 immediately after
    - Ex: [IP + 0x12], [EAX - 99]
- addr: an absolute 32-bit HEXADECIMAL memory address
    - Ex: @0xDEADBEEF, @0x1234ABCD

## Instruction Memory Diagram

Refer to the following diagram for how the specific bits of each instruction are laid out:

| Bits    | Field          |
|---------|----------------|
| 0..7    | Opcode         |
| 8..10   | Modifider bits |
| 11      | Sign [S]       |
| 12      | Addr. Mode [M] |
| 13..15  | Reserved       |

**Visual Diagram:**
```
0             7 8    10 11  12  14        15
[   OPCODE    ] [ MOD ] [S] [M] [ Reserved ]

Total: 1 byte (8 bits) + arguments if NO modifier fields are present, 2 bytes (16 bits) + arguments otherwise
```

## Control Instructions

| Inst.   | Op. A | Op. B | OpCode | MOD | Addr. Mode | Description                                                                         |
|---------|-------|-------|--------|-----|------------|-------------------------------------------------------------------------------------|
| nop     |    -- |    -- |   0x00 |  -- |         -- | No operation.                                                                       |
| hlt     |    -- |    -- |   0x01 |  -- |         -- | Stops CPU clock.                                                                    |
| syscall |    -- |    -- |   0x02 |  -- |         -- | Triggers a syscall based on the value in EAX, storing the return address in SRP.    |
| sysret  |       |    -- |   0x03 |  -- |         -- | Returns from a syscall to user mode via the return address in SRP.                  |
| call    | rel32 |    -- |   0x04 |   0 |   Rel. (0) | Moves the IP to a new memory address, storing the current IP in RP.                 |
| call    |  addr |    -- |   0x04 |   0 |   Abs. (1) | Moves the IP to a new memory address, storing the current IP in RP.                 |
| call    | reg32 |    -- |   0x04 |   1 |         -- | Moves the IP to a memory address from a reg32, storing the current IP in RP.        |
| ret     |    -- |    -- |   0x05 |  -- |         -- | Reverts the IP to the previous memory address stored in RP.                         |
| jmp     | rel32 |    -- |   0x06 |   0 |   Rel. (0) | Moves the IP to a new memory address.                                               |
| jmp     |  addr |    -- |   0x06 |   0 |   Abs. (1) | Moves the IP to a new memory address.                                               |
| jmp     | reg32 |    -- |   0x06 |   1 |         -- | Moves the IP to a memory address from a reg32.                                      |
| jz      | rel32 |    -- |   0x07 |   0 |   Rel. (0) | If the zero flag (ZF) is set, moves the IP to a new memory address.                 |
| jz      |  addr |    -- |   0x07 |   0 |   Abs. (1) | If the zero flag (ZF) is set, moves the IP to a new memory address.                 |
| jz      | reg32 |    -- |   0x07 |   1 |         -- | If the zero flag (ZF) is set, moves the IP to a memory address from a reg32.        |
| jnz     | rel32 |    -- |   0x07 |   2 |   Rel. (0) | If the zero flag (ZF) is clear, moves the IP to a new memory address.               |
| jnz     |  addr |    -- |   0x07 |   2 |   Abs. (1) | If the zero flag (ZF) is clear, moves the IP to a new memory address.               |
| jnz     | reg32 |    -- |   0x07 |   3 |         -- | If the zero flag (ZF) is clear, moves the IP to a memory address from a reg32.      |
| jc      | rel32 |    -- |   0x08 |   0 |   Rel. (0) | If the carry flag (CF) is set, moves the IP to a new memory address.                |
| jc      |  addr |    -- |   0x08 |   0 |   Abs. (1) | If the carry flag (CF) is set, moves the IP to a new memory address.                |
| jc      | reg32 |    -- |   0x08 |   1 |         -- | If the carry flag (CF) is set, moves the IP to a memory address from a reg32.       |
| jnc     | rel32 |    -- |   0x08 |   2 |   Rel. (0) | If the carry flag (CF) is clear, moves the IP to a new memory address.              |
| jnc     |  addr |    -- |   0x08 |   2 |   Abs. (1) | If the carry flag (CF) is clear, moves the IP to a new memory address.              |
| jnc     | reg32 |    -- |   0x08 |   3 |         -- | If the carry flag (CF) is clear, moves the IP to a memory address from a reg32.     |
| jo      | rel32 |    -- |   0x09 |   0 |   Rel. (0) | If the overflow flag (OF) is set, moves the IP to a new memory address.             |
| jo      |  addr |    -- |   0x09 |   0 |   Abs. (1) | If the overflow flag (OF) is set, moves the IP to a new memory address.             |
| jo      | reg32 |    -- |   0x09 |   1 |         -- | If the overflow flag (OF) is set, moves the IP to a memory address from a reg32.    |
| jno     | rel32 |    -- |   0x09 |   2 |   Rel. (0) | If the overflow flag (OF) is clear, moves the IP to a new memory address.           |
| jno     |  addr |    -- |   0x09 |   2 |   Abs. (1) | If the overflow flag (OF) is clear, moves the IP to a new memory address.           |
| jno     | reg32 |    -- |   0x09 |   3 |         -- | If the overflow flag (OF) is clear, moves the IP to a memory address from a reg32.  |
| js      | rel32 |    -- |   0x0A |   0 |   Rel. (0) | If the sign flag (SF) is set, moves the IP to a new memory address.                 |
| js      |  addr |    -- |   0x0A |   0 |   Abs. (1) | If the sign flag (SF) is set, moves the IP to a new memory address.                 |
| js      | reg32 |    -- |   0x0A |   1 |         -- | If the sign flag (SF) is set, moves the IP to a memory address from a reg32.        |
| jns     | rel32 |    -- |   0x0A |   2 |   Rel. (0) | If the sign flag (SF) is clear, moves the IP to a new memory address.               |
| jns     |  addr |    -- |   0x0A |   2 |   Abs. (1) | If the sign flag (SF) is clear, moves the IP to a new memory address.               |
| jns     | reg32 |    -- |   0x0A |   3 |         -- | If the sign flag (SF) is clear, moves the IP to a memory address from a reg32.      |
| jp      | rel32 |    -- |   0x0B |   0 |   Rel. (0) | If the parity flag (PF) is set, moves the IP to a new memory address.               |
| jp      |  addr |    -- |   0x0B |   0 |   Abs. (1) | If the parity flag (PF) is set, moves the IP to a new memory address.               |
| jp      | reg32 |    -- |   0x0B |   1 |         -- | If the parity flag (PF) is set, moves the IP to a memory address from a reg32.      |
| jnp     | rel32 |    -- |   0x0B |   2 |   Rel. (0) | If the parity flag (PF) is clear, moves the IP to a new memory address.             |
| jnp     |  addr |    -- |   0x0B |   2 |   Abs. (1) | If the parity flag (PF) is clear, moves the IP to a new memory address.             |
| jnp     | reg32 |    -- |   0x0B |   3 |         -- | If the parity flag (PF) is clear, moves the IP to a memory address from a reg32.    |

## Register & Memory Instructions

| Inst.  | Op. A  | Op. B  | OpCode | MOD | Description                                                |
|--------|--------|--------|--------|-----|------------------------------------------------------------|
| mov    |   reg8 |  ximm8 |   0x10 |   0 | Moves an imm8 or simm8 value into a reg8.                  |
| --     |  reg16 | ximm16 |   0x10 |   1 | Moves an imm16 or simm16 value into a reg16.               |
| --     |  reg32 | ximm32 |   0x10 |   2 | Moves an imm32 or simm32 value into a reg32.               |
| --     |   reg8 |   reg8 |   0x10 |   3 | Moves an 8-bit value between two reg8.                     |
| --     |  reg16 |  reg16 |   0x10 |   4 | Moves an 16-bit value between two reg16.                   |
| --     |  reg32 |  reg32 |   0x10 |   5 | Moves an 32-bit value between two reg32.                   |
| lb     |   reg8 |   addr |   0x11 |   0 | Loads a byte from a memory address into a reg8.            |
| lw     |  reg16 |   addr |   0x11 |   1 | Loads a word starting at a memory address into a reg16.    |
| ldw    |  reg32 |   addr |   0x11 |   2 | Loads a dword starting at a memory address into a reg32.   |
| sb     |   reg8 |   addr |   0x12 |   0 | Saves a byte from a reg8 to an address.                    |
| sw     |  reg16 |   addr |   0x12 |   1 | Saves a word from a reg16 to an address.                   |
| sdw    |  reg32 |   addr |   0x12 |   2 | Saves a dword from a reg32 to an address.                  |
| push   |   reg8 |     -- |   0x13 |   0 | Pushes the value of a reg8 onto the top of the stack.      |
| --     |  ximm8 |     -- |   0x13 |   1 | Pushes an imm8 OR simm8 value onto the top of the stack.   |
| pushw  |  reg16 |     -- |   0x13 |   2 | Pushes the value of a reg16 onto the top of the stack.     |
| --     | ximm16 |     -- |   0x13 |   3 | Pushes an imm16 OR simm16 value onto the top of the stack. |
| pushdw |  reg32 |     -- |   0x13 |   4 | Pushes the value of a reg32 the top of the stack.          |
| --     | ximm32 |     -- |   0x13 |   5 | Pushes an imm32 OR simm32 value onto the top of the stack. |
| pop    |   reg8 |     -- |   0x14 |   0 | Pops the top byte of the stack into a reg8.                |
| --     |     -- |     -- |   0x14 |   1 | Discards the top byte of the stack.                        |
| popw   |  reg16 |     -- |   0x14 |   2 | Pops the top word of the stack into a reg16.               |
| --     |     -- |     -- |   0x14 |   3 | Discards the top word of the stack.                        |
| popdw  |  reg32 |     -- |   0x14 |   4 | Pops the top dword of the stack into a reg32.              |
| --     |     -- |     -- |   0x14 |   5 | Discards the top dword of the stack.                       |

## Bitwise & Arithmetic Instructions

| Inst. | Op. A  | Op. B  | OpCode | MOD | Signed? | Description                                                        | Flags? |
|-------|--------|--------|--------|-----|---------|--------------------------------------------------------------------|--------|
| cmp   |   reg8 |   imm8 |   0x21 |   0 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg16 |  imm16 |   0x21 |   1 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg32 |  imm32 |   0x21 |   2 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |   reg8 |   reg8 |   0x21 |   3 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg16 |  reg16 |   0x21 |   4 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg32 |  reg32 |   0x21 |   5 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| scmp  |   reg8 |  simm8 |   0x21 |   0 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg16 | simm16 |   0x21 |   1 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg32 | simm32 |   0x21 |   2 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |   reg8 |   reg8 |   0x21 |   3 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg16 |  reg16 |   0x21 |   4 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg32 |  reg32 |   0x21 |   5 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| and   |   reg8 |   imm8 |   0x22 |   0 |      -- | Bitwise AND on a reg8 and imm8, stored in first reg8.              | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x22 |   1 |      -- | Bitwise AND on a reg16 and imm16, stored in first reg16.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x22 |   2 |      -- | Bitwise AND on a reg32 and imm32, stored in first reg32.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x22 |   3 |      -- | Bitwise AND on two reg8, stored in first reg8.                     | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x22 |   4 |      -- | Bitwise AND on two reg16, stored in first reg16.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x22 |   5 |      -- | Bitwise AND on two reg32, stored in first reg32.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| or    |   reg8 |   imm8 |   0x23 |   0 |      -- | Bitwise OR on a reg8 and imm8, stored in first reg8.               | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x23 |   1 |      -- | Bitwise OR on a reg16 and imm16, stored in first reg16.            | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x23 |   2 |      -- | Bitwise OR on a reg32 and imm32, stored in first reg32.            | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x23 |   3 |      -- | Bitwise OR on two reg8, stored in first reg8.                      | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x23 |   4 |      -- | Bitwise OR on two reg16, stored in first reg16.                    | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x23 |   5 |      -- | Bitwise OR on two reg32, stored in first reg32.                    | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| xor   |   reg8 |   imm8 |   0x24 |   0 |      -- | Bitwise XOR on a reg8 and imm8, stored in first reg8.              | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x24 |   1 |      -- | Bitwise XOR on a reg16 and imm16, stored in first reg16.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x24 |   2 |      -- | Bitwise XOR on a reg32 and imm32, stored in first reg32.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x24 |   3 |      -- | Bitwise XOR on two reg8, stored in first reg8.                     | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x24 |   4 |      -- | Bitwise XOR on two reg16, stored in first reg16.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x24 |   5 |      -- | Bitwise XOR on two reg32, stored in first reg32.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| not   |   reg8 |     -- |   0x25 |   0 |      -- | Bitwise NOT on a reg8, stored in place.                            | --     |
| --    |  reg16 |     -- |   0x25 |   1 |      -- | Bitwise NOT on a reg16, stored in place.                           | --     |
| --    |  reg32 |     -- |   0x25 |   2 |      -- | Bitwise NOT on a reg32, stored in place.                           | --     |
| add   |   reg8 |   imm8 |   0x2A |   0 |  No (0) | Adds a reg8 and imm8, stored in first reg8.                        | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x2A |   1 |  No (0) | Adds a reg16 and imm16, stored in first reg16.                     | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x2A |   2 |  No (0) | Adds a reg32 and imm32, stored in first reg32.                     | Flags affected: CF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x2A |   3 |  No (0) | Adds two reg8, stored in first reg8.                               | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x2A |   4 |  No (0) | Adds two reg16, stored in first reg16.                             | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x2A |   5 |  No (0) | Adds two reg32, stored in first reg32.                             | Flags affected: CF, PF, ZF, SF. |
| sadd  |   reg8 |  simm8 |   0x2A |   0 | Yes (1) | Adds a reg8 and simm8, stored in first reg8.                       | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 | simm16 |   0x2A |   1 | Yes (1) | Adds a reg16 and simm16, stored in first reg16.                    | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 | simm32 |   0x2A |   2 | Yes (1) | Adds a reg32 and simm32, stored in first reg32.                    | Flags affected: OF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x2A |   3 | Yes (1) | Adds two reg8, stored in first reg8.                               | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x2A |   4 | Yes (1) | Adds two reg16, stored in first reg16.                             | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x2A |   5 | Yes (1) | Adds two reg32, stored in first reg32.                             | Flags affected: OF, PF, ZF, SF. |
| sub   |   reg8 |   imm8 |   0x2B |   0 |  No (0) | Subtracts an imm8 from a reg8, stored in first reg8.               | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x2B |   1 |  No (0) | Subtracts an imm16 from a reg16, stored in first reg16.            | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x2B |   2 |  No (0) | Subtracts an imm32 from a reg32, stored in first reg32.            | Flags affected: CF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x2B |   3 |  No (0) | Subtracts the second reg8 from the first, stored in first reg8.    | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x2B |   4 |  No (0) | Subtracts the second reg16 from the first, stored in first reg16.  | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x2B |   5 |  No (0) | Subtracts the second reg32 from the first, stored in first reg32.  | Flags affected: CF, PF, ZF, SF. |
| ssub  |   reg8 |  simm8 |   0x2B |   0 | Yes (1) | Subtracts a simm8 from a reg8, stored in first reg8.               | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 | simm16 |   0x2B |   1 | Yes (1) | Subtracts a simm16 from a reg16, stored in first reg16.            | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 | simm32 |   0x2B |   2 | Yes (1) | Subtracts a simm32 from a reg32, stored in first reg32.            | Flags affected: OF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x2B |   3 | Yes (1) | Subtracts the second reg8 from the first, stored in first reg8.    | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x2B |   4 | Yes (1) | Subtracts the second reg16 from the first, stored in first reg16.  | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x2B |   5 | Yes (1) | Subtracts the second reg32 from the first, stored in first reg32.  | Flags affected: OF, PF, ZF, SF. |
| mul   |   imm8 |     -- |   0x2C |   0 |  No (0) | Multiplies AL by an imm8, storing the result in AX.                                    | Flags affected: OF, CF (both equal). |
| --    |  imm16 |     -- |   0x2C |   1 |  No (0) | Multiplies AX by an imm16, storing the result in EAX.                                  | Flags affected: OF, CF (both equal). |
| --    |  imm32 |     -- |   0x2C |   2 |  No (0) | Multiplies EAX by an imm32, storing the lower half in EAX and the upper half in EDX.   | Flags affected: OF, CF (both equal). |
| --    |   reg8 |     -- |   0x2C |   3 |  No (0) | Multiplies AL by a reg8, storing the result in AX.                                     | Flags affected: OF, CF (both equal). |
| --    |  reg16 |     -- |   0x2C |   4 |  No (0) | Multiplies AX by a reg6, storing the result in EAX.                                    | Flags affected: OF, CF (both equal). |
| --    |  reg32 |     -- |   0x2C |   5 |  No (0) | Multiplies EAX by a reg32, storing the lower half in EAX and the upper half in EDX.    | Flags affected: OF, CF (both equal). |
| smul  |  simm8 |     -- |   0x2C |   0 | Yes (1) | Multiplies AL by a simm8, storing the result in AX.                                    | Flags affected: OF, CF (both equal). |
| --    | simm16 |     -- |   0x2C |   1 | Yes (1) | Multiplies AX by a simm16, storing the result in EAX.                                  | Flags affected: OF, CF (both equal). |
| --    | simm32 |     -- |   0x2C |   2 | Yes (1) | Multiplies EAX by a simm32, storing the lower half in EAX and the upper half in EDX.   | Flags affected: OF, CF (both equal). |
| --    |   reg8 |     -- |   0x2C |   3 | Yes (1) | Multiplies AL by a reg8, storing the result in AX.                                     | Flags affected: OF, CF (both equal). |
| --    |  reg16 |     -- |   0x2C |   4 | Yes (1) | Multiplies AX by a reg6, storing the result in EAX.                                    | Flags affected: OF, CF (both equal). |
| --    |  reg32 |     -- |   0x2C |   5 | Yes (1) | Multiplies EAX by a reg32, storing the lower half in EAX and the upper half in EDX.    | Flags affected: OF, CF (both equal). |

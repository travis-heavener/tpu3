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
| syscall |    -- |    -- |   0x01 |  -- |         -- | Triggers a syscall based on the value in EAX, storing the return address in SRP.    |
| sysret  |       |    -- |   0x02 |  -- |         -- | Returns from a syscall to user mode via the return address in SRP.                  |
| call    | rel32 |    -- |   0x03 |   0 |   Rel. (0) | Moves the IP to a new memory address, storing the current IP in RP.                 |
| call    |  addr |    -- |   0x03 |   0 |   Abs. (1) | Moves the IP to a new memory address, storing the current IP in RP.                 |
| call    | reg32 |    -- |   0x03 |   1 |         -- | Moves the IP to a memory address from a reg32, storing the current IP in RP.        |
| ret     |    -- |    -- |   0x04 |  -- |         -- | Reverts the IP to the previous memory address stored in RP.                         |
| jmp     | rel32 |    -- |   0x05 |   0 |   Rel. (0) | Moves the IP to a new memory address.                                               |
| jmp     |  addr |    -- |   0x05 |   0 |   Abs. (1) | Moves the IP to a new memory address.                                               |
| jmp     | reg32 |    -- |   0x05 |   1 |         -- | Moves the IP to a memory address from a reg32.                                      |
| jz      | rel32 |    -- |   0x06 |   0 |   Rel. (0) | If the zero flag (ZF) is set, moves the IP to a new memory address.                 |
| jz      |  addr |    -- |   0x06 |   0 |   Abs. (1) | If the zero flag (ZF) is set, moves the IP to a new memory address.                 |
| jz      | reg32 |    -- |   0x06 |   1 |         -- | If the zero flag (ZF) is set, moves the IP to a memory address from a reg32.        |
| jnz     | rel32 |    -- |   0x06 |   2 |   Rel. (0) | If the zero flag (ZF) is clear, moves the IP to a new memory address.               |
| jnz     |  addr |    -- |   0x06 |   2 |   Abs. (1) | If the zero flag (ZF) is clear, moves the IP to a new memory address.               |
| jnz     | reg32 |    -- |   0x06 |   3 |         -- | If the zero flag (ZF) is clear, moves the IP to a memory address from a reg32.      |
| jc      | rel32 |    -- |   0x07 |   0 |   Rel. (0) | If the carry flag (CF) is set, moves the IP to a new memory address.                |
| jc      |  addr |    -- |   0x07 |   0 |   Abs. (1) | If the carry flag (CF) is set, moves the IP to a new memory address.                |
| jc      | reg32 |    -- |   0x07 |   1 |         -- | If the carry flag (CF) is set, moves the IP to a memory address from a reg32.       |
| jnc     | rel32 |    -- |   0x07 |   2 |   Rel. (0) | If the carry flag (CF) is clear, moves the IP to a new memory address.              |
| jnc     |  addr |    -- |   0x07 |   2 |   Abs. (1) | If the carry flag (CF) is clear, moves the IP to a new memory address.              |
| jnc     | reg32 |    -- |   0x07 |   3 |         -- | If the carry flag (CF) is clear, moves the IP to a memory address from a reg32.     |
| jo      | rel32 |    -- |   0x08 |   0 |   Rel. (0) | If the overflow flag (OF) is set, moves the IP to a new memory address.             |
| jo      |  addr |    -- |   0x08 |   0 |   Abs. (1) | If the overflow flag (OF) is set, moves the IP to a new memory address.             |
| jo      | reg32 |    -- |   0x08 |   1 |         -- | If the overflow flag (OF) is set, moves the IP to a memory address from a reg32.    |
| jno     | rel32 |    -- |   0x08 |   2 |   Rel. (0) | If the overflow flag (OF) is clear, moves the IP to a new memory address.           |
| jno     |  addr |    -- |   0x08 |   2 |   Abs. (1) | If the overflow flag (OF) is clear, moves the IP to a new memory address.           |
| jno     | reg32 |    -- |   0x08 |   3 |         -- | If the overflow flag (OF) is clear, moves the IP to a memory address from a reg32.  |
| js      | rel32 |    -- |   0x09 |   0 |   Rel. (0) | If the sign flag (SF) is set, moves the IP to a new memory address.                 |
| js      |  addr |    -- |   0x09 |   0 |   Abs. (1) | If the sign flag (SF) is set, moves the IP to a new memory address.                 |
| js      | reg32 |    -- |   0x09 |   1 |         -- | If the sign flag (SF) is set, moves the IP to a memory address from a reg32.        |
| jns     | rel32 |    -- |   0x09 |   2 |   Rel. (0) | If the sign flag (SF) is clear, moves the IP to a new memory address.               |
| jns     |  addr |    -- |   0x09 |   2 |   Abs. (1) | If the sign flag (SF) is clear, moves the IP to a new memory address.               |
| jns     | reg32 |    -- |   0x09 |   3 |         -- | If the sign flag (SF) is clear, moves the IP to a memory address from a reg32.      |
| jp      | rel32 |    -- |   0x0A |   0 |   Rel. (0) | If the parity flag (PF) is set, moves the IP to a new memory address.               |
| jp      |  addr |    -- |   0x0A |   0 |   Abs. (1) | If the parity flag (PF) is set, moves the IP to a new memory address.               |
| jp      | reg32 |    -- |   0x0A |   1 |         -- | If the parity flag (PF) is set, moves the IP to a memory address from a reg32.      |
| jnp     | rel32 |    -- |   0x0A |   2 |   Rel. (0) | If the parity flag (PF) is clear, moves the IP to a new memory address.             |
| jnp     |  addr |    -- |   0x0A |   2 |   Abs. (1) | If the parity flag (PF) is clear, moves the IP to a new memory address.             |
| jnp     | reg32 |    -- |   0x0A |   3 |         -- | If the parity flag (PF) is clear, moves the IP to a memory address from a reg32.    |

## Kernel Protected Instructions
| Inst.      | Op. A | Op. B | OpCode | MOD | Description                                                                         |
|------------|-------|-------|--------|-----|-------------------------------------------------------------------------------------|
| hlt        |    -- |    -- |   0x15 |  -- | Stops CPU clock.                                                                    |
| uret       |  addr |  addr |   0x16 |  -- | Enters user mode after kernel initialization, setting the IP and ESP, respectively. |
| setsyscall |  imm8 | rel32 |   0x16 |  -- | Binds a syscall handler for the syscall number in imm8 using a rel32.               |

## Register & Memory Instructions

| Inst.  | Op. A  | Op. B  | OpCode | MOD | Addr. Mode | Description                                                |
|--------|--------|--------|--------|-----|------------|------------------------------------------------------------|
| mov    |   reg8 |  ximm8 |   0x30 |   0 |         -- | Moves an imm8 or simm8 value into a reg8.                  |
| --     |  reg16 | ximm16 |   0x30 |   1 |         -- | Moves an imm16 or simm16 value into a reg16.               |
| --     |  reg32 | ximm32 |   0x30 |   2 |         -- | Moves an imm32 or simm32 value into a reg32.               |
| --     |   reg8 |   reg8 |   0x30 |   3 |         -- | Moves an 8-bit value between two reg8.                     |
| --     |  reg16 |  reg16 |   0x30 |   4 |         -- | Moves an 16-bit value between two reg16.                   |
| --     |  reg32 |  reg32 |   0x30 |   5 |         -- | Moves an 32-bit value between two reg32.                   |
| --     |  reg32 |  rel32 |   0x30 |   6 |         -- | Resolves a rel32 address into a reg32.                     |
| lb     |   reg8 |  rel32 |   0x31 |   0 |   Rel. (0) | Loads a byte from a memory address into a reg8.            |
| --     |   reg8 |   addr |   0x31 |   0 |   Abs. (1) | Loads a byte from a memory address into a reg8.            |
| --     |   reg8 |  reg32 |   0x31 |   1 |         -- | Loads a byte from a memory address in a reg32 to a reg8.   |
| lw     |  reg16 |  rel32 |   0x31 |   2 |   Rel. (0) | Loads a word from a memory address into a reg16.           |
| --     |  reg16 |   addr |   0x31 |   2 |   Abs. (1) | Loads a word from a memory address into a reg16.           |
| --     |  reg16 |  reg32 |   0x31 |   3 |         -- | Loads a word from a memory address in a reg32 to a reg16.  |
| ldw    |  reg32 |  rel32 |   0x31 |   4 |   Rel. (0) | Loads a dword from a memory address into a reg32.          |
| --     |  reg32 |   addr |   0x31 |   4 |   Abs. (1) | Loads a dword from a memory address into a reg32.          |
| --     |  reg32 |  reg32 |   0x31 |   5 |         -- | Loads a dword from a memory address in a reg32 to a reg32. |
| sb     |   reg8 |  rel32 |   0x32 |   0 |   Rel. (0) | Saves a byte from a reg8 to an address.                    |
| --     |   reg8 |   addr |   0x32 |   0 |   Abs. (1) | Saves a byte from a reg8 to an address.                    |
| --     |   reg8 |  reg32 |   0x32 |   1 |         -- | Saves a byte from a reg8 to an address in a reg32.         |
| sw     |  reg16 |  rel32 |   0x32 |   2 |   Rel. (0) | Saves a word from a reg16 to an address.                   |
| --     |  reg16 |   addr |   0x32 |   2 |   Abs. (1) | Saves a word from a reg16 to an address.                   |
| --     |  reg16 |  reg32 |   0x32 |   3 |         -- | Saves a word from a reg16 to an address in a reg32.        |
| sdw    |  reg32 |  rel32 |   0x32 |   4 |   Rel. (0) | Saves a dword from a reg32 to an address.                  |
| --     |  reg32 |   addr |   0x32 |   4 |   Abs. (1) | Saves a dword from a reg32 to an address.                  |
| --     |  reg32 |  reg32 |   0x32 |   5 |         -- | Saves a dword from a reg32 to an address in a reg32.       |
| push   |   reg8 |     -- |   0x33 |   0 |            | Pushes the value of a reg8 onto the top of the stack.      |
| --     |  ximm8 |     -- |   0x33 |   1 |            | Pushes an imm8 OR simm8 value onto the top of the stack.   |
| pushw  |  reg16 |     -- |   0x33 |   2 |            | Pushes the value of a reg16 onto the top of the stack.     |
| --     | ximm16 |     -- |   0x33 |   3 |            | Pushes an imm16 OR simm16 value onto the top of the stack. |
| pushdw |  reg32 |     -- |   0x33 |   4 |            | Pushes the value of a reg32 the top of the stack.          |
| --     | ximm32 |     -- |   0x33 |   5 |            | Pushes an imm32 OR simm32 value onto the top of the stack. |
| pop    |   reg8 |     -- |   0x34 |   0 |            | Pops the top byte of the stack into a reg8.                |
| --     |     -- |     -- |   0x34 |   1 |            | Discards the top byte of the stack.                        |
| popw   |  reg16 |     -- |   0x34 |   2 |            | Pops the top word of the stack into a reg16.               |
| --     |     -- |     -- |   0x34 |   3 |            | Discards the top word of the stack.                        |
| popdw  |  reg32 |     -- |   0x34 |   4 |            | Pops the top dword of the stack into a reg32.              |
| --     |     -- |     -- |   0x34 |   5 |            | Discards the top dword of the stack.                       |

## Bitwise & Arithmetic Instructions

| Inst. | Op. A  | Op. B  | OpCode | MOD | Signed? | Description                                                        | Flags? |
|-------|--------|--------|--------|-----|---------|--------------------------------------------------------------------|--------|
| cmp   |   reg8 |   imm8 |   0x61 |   0 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg16 |  imm16 |   0x61 |   1 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg32 |  imm32 |   0x61 |   2 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |   reg8 |   reg8 |   0x61 |   3 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg16 |  reg16 |   0x61 |   4 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| --    |  reg32 |  reg32 |   0x61 |   5 |  No (0) | Same as sub, but doesn't store the result.                         | --     |
| scmp  |   reg8 |  simm8 |   0x61 |   0 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg16 | simm16 |   0x61 |   1 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg32 | simm32 |   0x61 |   2 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |   reg8 |   reg8 |   0x61 |   3 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg16 |  reg16 |   0x61 |   4 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| --    |  reg32 |  reg32 |   0x61 |   5 | Yes (1) | Same as ssub, but doesn't store the result.                        | --     |
| and   |   reg8 |   imm8 |   0x62 |   0 |      -- | Bitwise AND on a reg8 and imm8, stored in first reg8.              | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x62 |   1 |      -- | Bitwise AND on a reg16 and imm16, stored in first reg16.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x62 |   2 |      -- | Bitwise AND on a reg32 and imm32, stored in first reg32.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x62 |   3 |      -- | Bitwise AND on two reg8, stored in first reg8.                     | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x62 |   4 |      -- | Bitwise AND on two reg16, stored in first reg16.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x62 |   5 |      -- | Bitwise AND on two reg32, stored in first reg32.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| or    |   reg8 |   imm8 |   0x63 |   0 |      -- | Bitwise OR on a reg8 and imm8, stored in first reg8.               | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x63 |   1 |      -- | Bitwise OR on a reg16 and imm16, stored in first reg16.            | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x63 |   2 |      -- | Bitwise OR on a reg32 and imm32, stored in first reg32.            | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x63 |   3 |      -- | Bitwise OR on two reg8, stored in first reg8.                      | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x63 |   4 |      -- | Bitwise OR on two reg16, stored in first reg16.                    | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x63 |   5 |      -- | Bitwise OR on two reg32, stored in first reg32.                    | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| xor   |   reg8 |   imm8 |   0x64 |   0 |      -- | Bitwise XOR on a reg8 and imm8, stored in first reg8.              | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x64 |   1 |      -- | Bitwise XOR on a reg16 and imm16, stored in first reg16.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x64 |   2 |      -- | Bitwise XOR on a reg32 and imm32, stored in first reg32.           | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x64 |   3 |      -- | Bitwise XOR on two reg8, stored in first reg8.                     | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x64 |   4 |      -- | Bitwise XOR on two reg16, stored in first reg16.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x64 |   5 |      -- | Bitwise XOR on two reg32, stored in first reg32.                   | Flags cleared: OF, CF; flags affected: PF, ZF, SF. |
| not   |   reg8 |     -- |   0x65 |   0 |      -- | Bitwise NOT on a reg8, stored in place.                            | --     |
| --    |  reg16 |     -- |   0x65 |   1 |      -- | Bitwise NOT on a reg16, stored in place.                           | --     |
| --    |  reg32 |     -- |   0x65 |   2 |      -- | Bitwise NOT on a reg32, stored in place.                           | --     |
| add   |   reg8 |   imm8 |   0x6A |   0 |  No (0) | Adds a reg8 and imm8, stored in first reg8.                        | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x6A |   1 |  No (0) | Adds a reg16 and imm16, stored in first reg16.                     | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x6A |   2 |  No (0) | Adds a reg32 and imm32, stored in first reg32.                     | Flags affected: CF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x6A |   3 |  No (0) | Adds two reg8, stored in first reg8.                               | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x6A |   4 |  No (0) | Adds two reg16, stored in first reg16.                             | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x6A |   5 |  No (0) | Adds two reg32, stored in first reg32.                             | Flags affected: CF, PF, ZF, SF. |
| sadd  |   reg8 |  simm8 |   0x6A |   0 | Yes (1) | Adds a reg8 and simm8, stored in first reg8.                       | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 | simm16 |   0x6A |   1 | Yes (1) | Adds a reg16 and simm16, stored in first reg16.                    | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 | simm32 |   0x6A |   2 | Yes (1) | Adds a reg32 and simm32, stored in first reg32.                    | Flags affected: OF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x6A |   3 | Yes (1) | Adds two reg8, stored in first reg8.                               | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x6A |   4 | Yes (1) | Adds two reg16, stored in first reg16.                             | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x6A |   5 | Yes (1) | Adds two reg32, stored in first reg32.                             | Flags affected: OF, PF, ZF, SF. |
| sub   |   reg8 |   imm8 |   0x6B |   0 |  No (0) | Subtracts an imm8 from a reg8, stored in first reg8.               | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  imm16 |   0x6B |   1 |  No (0) | Subtracts an imm16 from a reg16, stored in first reg16.            | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  imm32 |   0x6B |   2 |  No (0) | Subtracts an imm32 from a reg32, stored in first reg32.            | Flags affected: CF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x6B |   3 |  No (0) | Subtracts the second reg8 from the first, stored in first reg8.    | Flags affected: CF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x6B |   4 |  No (0) | Subtracts the second reg16 from the first, stored in first reg16.  | Flags affected: CF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x6B |   5 |  No (0) | Subtracts the second reg32 from the first, stored in first reg32.  | Flags affected: CF, PF, ZF, SF. |
| ssub  |   reg8 |  simm8 |   0x6B |   0 | Yes (1) | Subtracts a simm8 from a reg8, stored in first reg8.               | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 | simm16 |   0x6B |   1 | Yes (1) | Subtracts a simm16 from a reg16, stored in first reg16.            | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 | simm32 |   0x6B |   2 | Yes (1) | Subtracts a simm32 from a reg32, stored in first reg32.            | Flags affected: OF, PF, ZF, SF. |
| --    |   reg8 |   reg8 |   0x6B |   3 | Yes (1) | Subtracts the second reg8 from the first, stored in first reg8.    | Flags affected: OF, PF, ZF, SF. |
| --    |  reg16 |  reg16 |   0x6B |   4 | Yes (1) | Subtracts the second reg16 from the first, stored in first reg16.  | Flags affected: OF, PF, ZF, SF. |
| --    |  reg32 |  reg32 |   0x6B |   5 | Yes (1) | Subtracts the second reg32 from the first, stored in first reg32.  | Flags affected: OF, PF, ZF, SF. |
| mul   |   imm8 |     -- |   0x6C |   0 |  No (0) | Multiplies AL by an imm8, storing the result in AX.                                    | Flags affected: OF, CF (both equal). |
| --    |  imm16 |     -- |   0x6C |   1 |  No (0) | Multiplies AX by an imm16, storing the result in EAX.                                  | Flags affected: OF, CF (both equal). |
| --    |  imm32 |     -- |   0x6C |   2 |  No (0) | Multiplies EAX by an imm32, storing the lower half in EAX and the upper half in EDX.   | Flags affected: OF, CF (both equal). |
| --    |   reg8 |     -- |   0x6C |   3 |  No (0) | Multiplies AL by a reg8, storing the result in AX.                                     | Flags affected: OF, CF (both equal). |
| --    |  reg16 |     -- |   0x6C |   4 |  No (0) | Multiplies AX by a reg6, storing the result in EAX.                                    | Flags affected: OF, CF (both equal). |
| --    |  reg32 |     -- |   0x6C |   5 |  No (0) | Multiplies EAX by a reg32, storing the lower half in EAX and the upper half in EDX.    | Flags affected: OF, CF (both equal). |
| smul  |  simm8 |     -- |   0x6C |   0 | Yes (1) | Multiplies AL by a simm8, storing the result in AX.                                    | Flags affected: OF, CF (both equal). |
| --    | simm16 |     -- |   0x6C |   1 | Yes (1) | Multiplies AX by a simm16, storing the result in EAX.                                  | Flags affected: OF, CF (both equal). |
| --    | simm32 |     -- |   0x6C |   2 | Yes (1) | Multiplies EAX by a simm32, storing the lower half in EAX and the upper half in EDX.   | Flags affected: OF, CF (both equal). |
| --    |   reg8 |     -- |   0x6C |   3 | Yes (1) | Multiplies AL by a reg8, storing the result in AX.                                     | Flags affected: OF, CF (both equal). |
| --    |  reg16 |     -- |   0x6C |   4 | Yes (1) | Multiplies AX by a reg6, storing the result in EAX.                                    | Flags affected: OF, CF (both equal). |
| --    |  reg32 |     -- |   0x6C |   5 | Yes (1) | Multiplies EAX by a reg32, storing the lower half in EAX and the upper half in EDX.    | Flags affected: OF, CF (both equal). |

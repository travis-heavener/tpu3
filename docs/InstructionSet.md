# Instruction Set

The following table depicts the complete instruction set for the 32-bit TPU3 architecture:

\* Unimplemented

## General Instructions

| Instruction   | Op. A | Op. B | OpCode | MOD Bits | Signed? | Description |
|---------------|-------|-------|--------|----------|---------|-------------|
| nop           |    -- |    -- |   0x00 |       -- |      -- | No operation. |
| hlt           |    -- |    -- |   0x01 |       -- |      -- | Stops the clock on the CPU. |
| syscall*      |  imm8 |    -- |   0x02 |       -- |      -- | Triggers a syscall, entering kernel mode for protected instructions. See `call`. |
| sysret*       |       |    -- |   0x03 |       -- |      -- | Returns from a syscall, exiting kernel mode and returning to user mode. See `ret`. |
| call          | label |    -- |   0x04 |       -- |      -- | Moves the IP to the label's address, storing the current IP in RP. |
| ret           |    -- |    -- |   0x05 |       -- |      -- | Reverts the IP to the previous memory address stored in RP. |
| jmp           | label |    -- |   0x06 |        0 |      -- | Moves the IP to the label's address. |
| jz            | label |    -- |   0x06 |        1 |      -- | Moves the IP to the label's address, if the zero flag (ZF) is set. |
| jnz           | label |    -- |   0x06 |        2 |      -- | Moves the IP to the label's address, if the zero flag (ZF) is cleared. |
| jc            | label |    -- |   0x06 |        3 |      -- | Moves the IP to the label's address, if the carry flag (CF) is set. |
| jnc           | label |    -- |   0x06 |        4 |      -- | Moves the IP to the label's address, if the carry flag (CF) is cleared. |
| jo            | label |    -- |   0x06 |        5 |      -- | Moves the IP to the label's address, if the overflow flag (OF) is set. |
| jno           | label |    -- |   0x06 |        6 |      -- | Moves the IP to the label's address, if the overflow flag (OF) is cleared. |
| mov           |  reg8 |  imm8 |   0x07 |        0 |      -- | Moves an immediate 8-bit value into an 8-bit register. |
| movw          | reg16 | imm16 |   0x07 |        1 |      -- | Moves an immediate 16-bit value into a 16-bit register. |
| movdw         | reg32 | imm32 |   0x07 |        2 |      -- | Moves an immediate 32-bit value into a 32-bit register. |
| mov           |  reg8 |  reg8 |   0x07 |        3 |      -- | Moves an 8-bit value between two 8-bit registers. |
| movw          | reg16 | reg16 |   0x07 |        4 |      -- | Moves a 16-bit value between two 16-bit registers. |
| movdw         | reg32 | reg32 |   0x07 |        5 |      -- | Moves a 32-bit value between two 32-bit registers. |
| lb            |  reg8 | @addr |   0x08 |        0 |      -- | Loads a byte from a memory address into an 8-bit register. |
| lw            | reg16 | @addr |   0x08 |        1 |      -- | Loads a word starting at a memory address into a 16-bit register. |
| ldw           | reg32 | @addr |   0x08 |        2 |      -- | Loads a dword starting at a memory address into a 32-bit register. |
| sb            |  reg8 | @addr |   0x09 |        0 |      -- | Saves a byte from an 8-bit register to a memory address. |
| sw            | reg16 | @addr |   0x09 |        1 |      -- | Saves a word from a 16-bit register to a memory address. |
| sdw           | reg32 | @addr |   0x09 |        2 |      -- | Saves a dword from a 32-bit register to a memory address. |
| push          |  reg8 |    -- |   0x0A |        0 |      -- | Pushes the value of an 8-bit register onto the top of the stack. |
| push          |  imm8 |    -- |   0x0A |        1 |      -- | Pushes an immediate 8-bit value onto the top of the stack. |
| pushw         | reg16 |    -- |   0x0A |        2 |      -- | Pushes the value of a 16-bit register onto the top of the stack. |
| pushw         | imm16 |    -- |   0x0A |        3 |      -- | Pushes an immediate 16-bit value onto the top of the stack. |
| pushdw        | reg32 |    -- |   0x0A |        4 |      -- | Pushes the value of a 32-bit register onto the top of the stack. |
| pushdw        | imm32 |    -- |   0x0A |        5 |      -- | Pushes an immediate 32-bit value onto the top of the stack. |
| pop           |  reg8 |    -- |   0x0B |        0 |      -- | Pops the top byte of the stack into an 8-bit register. |
| pop           |    -- |    -- |   0x0B |        1 |      -- | Discards the top byte of the stack. |
| popw          | reg16 |    -- |   0x0B |        2 |      -- | Pops the top word of the stack into a 16-bit register. |
| popw          |    -- |    -- |   0x0B |        3 |      -- | Discards the top word of the stack. |
| popdw         | reg32 |    -- |   0x0B |        4 |      -- | Pops the top dword of the stack into a 32-bit register. |
| popdw         |    -- |    -- |   0x0B |        5 |      -- | Discards the top dword of the stack. |

## Bitwise & Arithmetic Instructions

| Instruction   | Op. A | Op. B | OpCode | MOD Bits | Signed? | Description |
|---------------|-------|-------|--------|----------|---------|-------------|
| buf           |  reg8 |    -- |   0x20 |        0 |      -- | Updates flags according to the value in an 8-bit register. |
| buf           | reg16 |    -- |   0x20 |        1 |      -- | Updates flags according to the value in a 16-bit register. |
| buf           | reg32 |    -- |   0x20 |        2 |      -- | Updates flags according to the value in a 32-bit register. |
| cmp           |  reg8 |  imm8 |   0x21 |        0 |  No (0) | Compares an 8-bit register and imm8. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| cmp           | reg16 | imm16 |   0x21 |        1 |  No (0) | Compares a 16-bit register and imm16. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| cmp           | reg32 | imm32 |   0x21 |        2 |  No (0) | Compares a 32-bit register and imm32. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| cmp           |  reg8 |  reg8 |   0x21 |        3 |  No (0) | Compares two 8-bit registers. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| cmp           | reg16 | reg16 |   0x21 |        4 |  No (0) | Compares two 16-bit registers. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| cmp           | reg32 | reg32 |   0x21 |        5 |  No (0) | Compares two 32-bit registers. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| scmp          |  reg8 |  imm8 |   0x21 |        0 | Yes (1) | Signed-compares an 8-bit register and imm8. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| scmp          | reg16 | imm16 |   0x21 |        1 | Yes (1) | Signed-compares a 16-bit register and imm16. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| scmp          | reg32 | imm32 |   0x21 |        2 | Yes (1) | Signed-compares a 32-bit register and imm32. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| scmp          |  reg8 |  reg8 |   0x21 |        3 | Yes (1) | Signed-compares two 8-bit registers. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| scmp          | reg16 | reg16 |   0x21 |        4 | Yes (1) | Signed-compares two 16-bit registers. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| scmp          | reg32 | reg32 |   0x21 |        5 | Yes (1) | Signed-compares two 32-bit registers. If A < B, ZF=0 and CF=1; if A == B, ZF=1 and CF=0; if A > B, ZF=0 and CF=1. |
| and           |  reg8 |  imm8 |   0x22 |        0 |      -- | Bitwise AND between an 8-bit register and imm8, stored in first operand. |
| and           | reg16 | imm16 |   0x22 |        1 |      -- | Bitwise AND between a 16-bit register and imm16, stored in first operand. |
| and           | reg32 | imm32 |   0x22 |        2 |      -- | Bitwise AND between a 32-bit register and imm32, stored in first operand. |
| and           |  reg8 |  reg8 |   0x22 |        3 |      -- | Bitwise AND between two 8-bit registers, stored in first operand. |
| and           | reg16 | reg16 |   0x22 |        4 |      -- | Bitwise AND between two 16-bit registers, stored in first operand. |
| and           | reg32 | reg32 |   0x22 |        5 |      -- | Bitwise AND between two 32-bit registers, stored in first operand. |
| or            |  reg8 |  imm8 |   0x23 |        0 |      -- | Bitwise OR between an 8-bit register and imm8, stored in first operand. |
| or            | reg16 | imm16 |   0x23 |        1 |      -- | Bitwise OR between a 16-bit register and imm16, stored in first operand. |
| or            | reg32 | imm32 |   0x23 |        2 |      -- | Bitwise OR between a 32-bit register and imm32, stored in first operand. |
| or            |  reg8 |  reg8 |   0x23 |        3 |      -- | Bitwise OR between two 8-bit registers, stored in first operand. |
| or            | reg16 | reg16 |   0x23 |        4 |      -- | Bitwise OR between two 16-bit registers, stored in first operand. |
| or            | reg32 | reg32 |   0x23 |        5 |      -- | Bitwise OR between two 32-bit registers, stored in first operand. |
| xor           |  reg8 |  imm8 |   0x24 |        0 |      -- | Bitwise XOR between an 8-bit register and imm8, stored in first operand. |
| xor           | reg16 | imm16 |   0x24 |        1 |      -- | Bitwise XOR between a 16-bit register and imm16, stored in first operand. |
| xor           | reg32 | imm32 |   0x24 |        2 |      -- | Bitwise XOR between a 32-bit register and imm32, stored in first operand. |
| xor           |  reg8 |  reg8 |   0x24 |        3 |      -- | Bitwise XOR between two 8-bit registers, stored in first operand. |
| xor           | reg16 | reg16 |   0x24 |        4 |      -- | Bitwise XOR between two 16-bit registers, stored in first operand. |
| xor           | reg32 | reg32 |   0x24 |        5 |      -- | Bitwise XOR between two 32-bit registers, stored in first operand. |
| not           |  reg8 |    -- |   0x25 |        0 |      -- | Bitwise NOT on an 8-bit register, stored in place. |
| not           | reg16 |    -- |   0x25 |        1 |      -- | Bitwise NOT on a 16-bit register, stored in place. |
| not           | reg32 |    -- |   0x25 |        2 |      -- | Bitwise NOT on a 32-bit register, stored in place. |
| add           |  reg8 |  imm8 |   0x2A |        0 |  No (0) | Unsigned-adds an 8-bit register and imm8, stored in first operand. |
| add           | reg16 | imm16 |   0x2A |        1 |  No (0) | Unsigned-adds a 16-bit register and imm16, stored in first operand. |
| add           | reg32 | imm32 |   0x2A |        2 |  No (0) | Unsigned-adds a 32-bit register and imm32, stored in first operand. |
| add           |  reg8 |  reg8 |   0x2A |        3 |  No (0) | Unsigned-adds two 8-bit registers, stored in first operand. |
| add           | reg16 | reg16 |   0x2A |        4 |  No (0) | Unsigned-adds two 16-bit registers, stored in first operand. |
| add           | reg32 | reg32 |   0x2A |        5 |  No (0) | Unsigned-adds two 32-bit registers, stored in first operand. |
| sadd          |  reg8 |  imm8 |   0x2A |        0 | Yes (1) | Signed-adds an 8-bit register and imm8, stored in first operand. |
| sadd          | reg16 | imm16 |   0x2A |        1 | Yes (1) | Signed-adds a 16-bit register and imm16, stored in first operand. |
| sadd          | reg32 | imm32 |   0x2A |        2 | Yes (1) | Signed-adds a 32-bit register and imm32, stored in first operand. |
| sadd          |  reg8 |  reg8 |   0x2A |        3 | Yes (1) | Signed-adds two 8-bit registers, stored in first operand. |
| sadd          | reg16 | reg16 |   0x2A |        4 | Yes (1) | Signed-adds two 16-bit registers, stored in first operand. |
| sadd          | reg32 | reg32 |   0x2A |        5 | Yes (1) | Signed-adds two 32-bit registers, stored in first operand. |
| sub           |  reg8 |  imm8 |   0x2B |        0 |  No (0) | Unsigned-subtracts an imm8 from an 8-bit register, stored in first operand. |
| sub           | reg16 | imm16 |   0x2B |        1 |  No (0) | Unsigned-subtracts an imm16 from a 16-bit register, stored in first operand. |
| sub           | reg32 | imm32 |   0x2B |        2 |  No (0) | Unsigned-subtracts an imm32 from a 32-bit register, stored in first operand. |
| sub           |  reg8 |  reg8 |   0x2B |        3 |  No (0) | Unsigned-subtracts the second 8-bit register from the first, stored in first operand. |
| sub           | reg16 | reg16 |   0x2B |        4 |  No (0) | Unsigned-subtracts the second 16-bit register from the first, stored in first operand. |
| sub           | reg32 | reg32 |   0x2B |        5 |  No (0) | Unsigned-subtracts the second 32-bit register from the first, stored in first operand. |
| ssub          |  reg8 |  imm8 |   0x2B |        0 | Yes (1) | Signed-subtracts an imm8 from an 8-bit register, stored in first operand. |
| ssub          | reg16 | imm16 |   0x2B |        1 | Yes (1) | Signed-subtracts an imm16 from a 16-bit register, stored in first operand. |
| ssub          | reg32 | imm32 |   0x2B |        2 | Yes (1) | Signed-subtracts an imm32 from a 32-bit register, stored in first operand. |
| ssub          |  reg8 |  reg8 |   0x2B |        3 | Yes (1) | Signed-subtracts the second 8-bit register from the first, stored in first operand. |
| ssub          | reg16 | reg16 |   0x2B |        4 | Yes (1) | Signed-subtracts the second 16-bit register from the first, stored in first operand. |
| ssub          | reg32 | reg32 |   0x2B |        5 | Yes (1) | Signed-subtracts the second 32-bit register from the first, stored in first operand. |
| mul           |  imm8 |    -- |   0x2C |        0 |  No (0) | Unsigned-multiplies the AL register by an imm8, storing the result in the AX register. |
| mul           | imm16 |    -- |   0x2C |        1 |  No (0) | Unsigned-multiplies the AX register by an imm16, storing the result in the EAX register. |
| mul           | imm32 |    -- |   0x2C |        2 |  No (0) | Unsigned-multiplies the EAX register by an imm32, storing the lower half of the result in EAX and the upper half in EDX. |
| mul           |  reg8 |    -- |   0x2C |        3 |  No (0) | Unsigned-multiplies the AL register by a reg8, storing the result in the AX register. |
| mul           | reg16 |    -- |   0x2C |        4 |  No (0) | Unsigned-multiplies the AX register by a reg6, storing the result in the EAX register. |
| mul           | reg32 |    -- |   0x2C |        5 |  No (0) | Unsigned-multiplies the EAX register by a reg32, storing the lower half of the result in EAX and the upper half in EDX. |
| smul          |  imm8 |    -- |   0x2C |        0 | Yes (1) | Signed-multiplies the AL register by an imm8, storing the result in the AX register. |
| smul          | imm16 |    -- |   0x2C |        1 | Yes (1) | Signed-multiplies the AX register by an imm16, storing the result in the EAX register. |
| smul          | imm32 |    -- |   0x2C |        2 | Yes (1) | Signed-multiplies the EAX register by an imm32, storing the lower half of the result in EAX and the upper half in EDX. |
| smul          |  reg8 |    -- |   0x2C |        3 | Yes (1) | Signed-multiplies the AL register by a reg8, storing the result in the AX register. |
| smul          | reg16 |    -- |   0x2C |        4 | Yes (1) | Signed-multiplies the AX register by a reg6, storing the result in the EAX register. |
| smul          | reg32 |    -- |   0x2C |        5 | Yes (1) | Signed-multiplies the EAX register by a reg32, storing the lower half of the result in EAX and the upper half in EDX. |

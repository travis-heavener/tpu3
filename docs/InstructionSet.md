# Instruction Set

The following table depicts the complete instruction set for the 32-bit TPU3 architecture:

| Instruction   | Op. A | Op. B | OpCode | MOD Bits | Signed? | Description |
|---------------|-------|-------|--------|----------|---------|-------------|
| nop           |    -- |    -- |   0x00 |       -- |      -- | No operation. |
| hlt           |    -- |    -- |   0x01 |       -- |      -- | Stops the clock on the CPU. |
| syscall       |    -- |    -- |   0x02 |       -- |      -- | Triggers a syscall, entering kernel mode for protected instructions. See `call`. |
| sysret        |    -- |    -- |   0x03 |       -- |      -- | Returns from a syscall, exiting kernel mode and returning to user mode. See `ret`. |
| call          | label |    -- |   0x04 |       -- |      -- | Moves the IP to the label's address, storing the current IP in RP. |
| ret           |    -- |    -- |   0x05 |       -- |      -- | Reverts the IP to the previous memory address stored in RP. |
| jmp           | label |    -- |   0x06 |        0 |      -- | Moves the IP to the label's address. |
| jz            | label |    -- |   0x06 |        1 |      -- | Moves the IP to the label's address, if the zero flag (ZF) is set. |
| jnz           | label |    -- |   0x06 |        2 |      -- | Moves the IP to the label's address, if the zero flag (ZF) is cleared. |
| jc            | label |    -- |   0x06 |        3 |      -- | Moves the IP to the label's address, if the carry flag (CF) is set. |
| jnc           | label |    -- |   0x06 |        4 |      -- | Moves the IP to the label's address, if the carry flag (CF) is cleared. |
| mov           |  reg8 |  imm8 |   0x07 |        0 |      -- | Moves an immediate 8-bit value into an 8-bit register. |
| movw          | imm16 | imm16 |   0x07 |        1 |      -- | Moves an immediate 16-bit value into a 16-bit register. |
| movdw         | imm32 | imm32 |   0x07 |        2 |      -- | Moves an immediate 32-bit value into a 32-bit register. |
| mov           |  reg8 |  reg8 |   0x07 |        3 |      -- | Moves an 8-bit value between two 8-bit registers. |
| movw          | reg16 | reg16 |   0x07 |        4 |      -- | Moves a 16-bit value between two 16-bit registers. |
| movdw         | reg32 | reg32 |   0x07 |        5 |      -- | Moves a 32-bit value between two 32-bit registers. |
| lb            |  reg8 | @addr |   0x08 |        0 |      -- | Loads a byte from a memory address into an 8-bit register. |
| lw            | reg16 | @addr |   0x08 |        1 |      -- | Loads a word starting at a memory address into a 16-bit register. |
| ldw           | reg32 | @addr |   0x08 |        2 |      -- | Loads a dword starting at a memory address into a 32-bit register. |
| sb            | @addr |  reg8 |   0x09 |        0 |      -- | Saves a byte from an 8-bit register to a memory address. |
| sw            | @addr | reg16 |   0x09 |        1 |      -- | Saves a word from a 16-bit register to a memory address. |
| sdw           | @addr | reg32 |   0x09 |        2 |      -- | Saves a dword from a 32-bit register to a memory address. |
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

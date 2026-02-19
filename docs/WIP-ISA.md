# Instruction Set

This file shows the complete instruction set for the 32-bit TPU3 architecture.

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

\* Unimplemented

## Table of Contents

- [General Instructions](#general-instructions)
  - [nop](#nop)
  - [hlt](#hlt)
  - [syscall*](#syscall)
  - [sysret*](#sysret)
  - [call](#call)
  - [ret](#ret)
  - [jmp / jz / jnz / jc / jnc / jo / jno](#jmp--jz--jnz--jc--jnc--jo--jno)
  - [mov / movw / movdw](#mov--movw--movdw)
  - [lb / lw / ldw](#lb--lw--ldw)
  - [sb / sw / sdw](#sb--sw--sdw)
  - [push / pushw / pushdw](#push--pushw--pushdw)
  - [pop / popw / popdw](#pop--popw--popdw)
- [Bitwise & Arithmetic Instructions](#bitwise--arithmetic-instructions)
  - [buf](#buf)
  - [cmp / scmp](#cmp--scmp)
  - [and](#and)
  - [or](#or)
  - [xor](#xor)
  - [not](#not)
  - [add / sadd](#add--sadd)
  - [sub / ssub](#sub--ssub)
  - [mul / smul](#mul--smul)

## General Instructions

### nop
#### (0x00, MOD: --)
No operation.

### hlt
#### (0x01, MOD: --)
Stops the clock on the CPU.

### syscall
#### (0x02, MOD: imm8)
- syscall imm8 — Triggers a syscall, entering kernel mode for protected instructions.

### sysret
#### (0x03, MOD: --)
- sysret — Returns from a syscall, exiting kernel mode.

### call
#### (0x04, MOD: --)
- call label — Moves IP to label address; stores return IP in RP.

### ret
#### (0x05, MOD: --)
- ret — Restores IP from RP.

### jmp / jz / jnz / jc / jnc / jo / jno
#### (0x06, MOD: 0..6)
- MOD 0: jmp label — Unconditional jump.
- MOD 1: jz  label — Jump if ZF set.
- MOD 2: jnz label — Jump if ZF cleared.
- MOD 3: jc  label — Jump if CF set.
- MOD 4: jnc label — Jump if CF cleared.
- MOD 5: jo  label — Jump if OF set.
- MOD 6: jno label — Jump if OF cleared.

### mov / movw / movdw
#### (0x07, MOD: 0..5)
- MOD 0: mov  reg8,  imm8  — Move immediate 8-bit into 8-bit register.
- MOD 1: movw reg16, imm16 — Move immediate 16-bit into 16-bit register.
- MOD 2: movdw reg32, imm32 — Move immediate 32-bit into 32-bit register.
- MOD 3: mov  reg8,  reg8  — Move between 8-bit registers.
- MOD 4: movw reg16, reg16 — Move between 16-bit registers.
- MOD 5: movdw reg32, reg32 — Move between 32-bit registers.

### lb / lw / ldw
#### (0x08, MOD: 0..2)
- MOD 0: lb  reg8,  @addr — Load byte into 8-bit register.
- MOD 1: lw  reg16, @addr — Load word into 16-bit register.
- MOD 2: ldw reg32, @addr — Load dword into 32-bit register.

### sb / sw / sdw
#### (0x09, MOD: 0..2)
- MOD 0: sb  reg8,  @addr — Store byte from 8-bit register.
- MOD 1: sw  reg16, @addr — Store word from 16-bit register.
- MOD 2: sdw reg32, @addr — Store dword from 32-bit register.

### push / pushw / pushdw
#### (0x0A, MOD: 0..5)
- MOD 0: push  reg8  — Push value of 8-bit register.
- MOD 1: push  imm8  — Push immediate 8-bit value.
- MOD 2: pushw reg16 — Push value of 16-bit register.
- MOD 3: pushw imm16 — Push immediate 16-bit value.
- MOD 4: pushdw reg32 — Push value of 32-bit register.
- MOD 5: pushdw imm32 — Push immediate 32-bit value.

### pop / popw / popdw
#### (0x0B, MOD: 0..5)
- MOD 0: pop   reg8  — Pop top byte into 8-bit register.
- MOD 1: pop        — Discard top byte.
- MOD 2: popw  reg16 — Pop top word into 16-bit register.
- MOD 3: popw       — Discard top word.
- MOD 4: popdw reg32 — Pop top dword into 32-bit register.
- MOD 5: popdw      — Discard top dword.

## Bitwise & Arithmetic Instructions

### buf
#### (0x20, MOD: 0..2)
- MOD 0: buf reg8  — Update flags from 8-bit register.
- MOD 1: buf reg16 — Update flags from 16-bit register.
- MOD 2: buf reg32 — Update flags from 32-bit register.

### cmp / scmp
#### (0x21, MOD: 0..5)
Unsigned cmp variants:
- MOD 0: cmp  reg8,  imm8  — Compare 8-bit register vs imm8.
- MOD 1: cmp  reg16, imm16 — Compare 16-bit register vs imm16.
- MOD 2: cmp  reg32, imm32 — Compare 32-bit register vs imm32.
- MOD 3: cmp  reg8,  reg8  — Compare two 8-bit registers.
- MOD 4: cmp  reg16, reg16 — Compare two 16-bit registers.
- MOD 5: cmp  reg32, reg32 — Compare two 32-bit registers.

Signed scmp variants (same MOD mapping, signed compare):
- MOD 0: scmp reg8,  imm8
- MOD 1: scmp reg16, imm16
- MOD 2: scmp reg32, imm32
- MOD 3: scmp reg8,  reg8
- MOD 4: scmp reg16, reg16
- MOD 5: scmp reg32, reg32

Behavior (both): If A < B => ZF=0, CF=1; A == B => ZF=1, CF=0; A > B => ZF=0, CF=1.

### and
#### (0x22, MOD: 0..5)
- MOD 0: and  reg8,  imm8
- MOD 1: and  reg16, imm16
- MOD 2: and  reg32, imm32
- MOD 3: and  reg8,  reg8
- MOD 4: and  reg16, reg16
- MOD 5: and  reg32, reg32

### or
#### (0x23, MOD: 0..5)
- MOD 0: or   reg8,  imm8
- MOD 1: or   reg16, imm16
- MOD 2: or   reg32, imm32
- MOD 3: or   reg8,  reg8
- MOD 4: or   reg16, reg16
- MOD 5: or   reg32, reg32

### xor
#### (0x24, MOD: 0..5)
- MOD 0: xor  reg8,  imm8
- MOD 1: xor  reg16, imm16
- MOD 2: xor  reg32, imm32
- MOD 3: xor  reg8,  reg8
- MOD 4: xor  reg16, reg16
- MOD 5: xor  reg32, reg32

### not
#### (0x25, MOD: 0..2)
- MOD 0: not  reg8  — Bitwise NOT on 8-bit register.
- MOD 1: not  reg16 — Bitwise NOT on 16-bit register.
- MOD 2: not  reg32 — Bitwise NOT on 32-bit register.

### add / sadd
#### (0x2A, MOD: 0..5)
Unsigned add variants (add):
- MOD 0: add  reg8,  imm8
- MOD 1: add  reg16, imm16
- MOD 2: add  reg32, imm32
- MOD 3: add  reg8,  reg8
- MOD 4: add  reg16, reg16
- MOD 5: add  reg32, reg32

Signed add variants (sadd) — same MOD mapping:
- MOD 0: sadd reg8,  imm8
- MOD 1: sadd reg16, imm16
- MOD 2: sadd reg32, imm32
- MOD 3: sadd reg8,  reg8
- MOD 4: sadd reg16, reg16
- MOD 5: sadd reg32, reg32

### sub / ssub
#### (0x2B, MOD: 0..5)
Unsigned sub variants (sub):
- MOD 0: sub  reg8,  imm8
- MOD 1: sub  reg16, imm16
- MOD 2: sub  reg32, imm32
- MOD 3: sub  reg8,  reg8
- MOD 4: sub  reg16, reg16
- MOD 5: sub  reg32, reg32

Signed sub variants (ssub) — same MOD mapping:
- MOD 0: ssub reg8,  imm8
- MOD 1: ssub reg16, imm16
- MOD 2: ssub reg32, imm32
- MOD 3: ssub reg8,  reg8
- MOD 4: ssub reg16, reg16
- MOD 5: ssub reg32, reg32

### mul / smul
#### (0x2C, MOD: 0..5)
Unsigned mul variants:
- MOD 0: mul  imm8  — AL * imm8 => AX (lower+upper).
- MOD 1: mul  imm16 — AX * imm16 => EAX (result fits).
- MOD 2: mul  imm32 — EAX * imm32 => low->EAX, high->EDX.
- MOD 3: mul  reg8  — AL * reg8 => AX.
- MOD 4: mul  reg16 — AX * reg16 => EAX.
- MOD 5: mul  reg32 — EAX * reg32 => low->EAX, high->EDX.

Signed smul variants (same MOD mapping, signed multiply):
- MOD 0: smul imm8
- MOD 1: smul imm16
- MOD 2: smul imm32
- MOD 3: smul reg8
- MOD 4: smul reg16
- MOD 5: smul reg32

# Instruction Set

This file shows the complete instruction set for the 32-bit TPU3 architecture:

\* Unimplemented

## Table of Contents

- [nop](#nop)
- [hlt](#hlt)
- syscall
- sysret
- call
- [ret](#ret)
- jmp/jz/jnz/jc/jnc/jo/jno
- mov/movw/movdw
- lb/lw/ldw
- sb/sw/sdw
- push/pushw/pushdw
- pop/popw/popdw
- buf
- cmp
- scmp
- and
- or
- xor
- not
- add/sadd
- sub/ssub
- mul/smul

## NOP

No operation.

## HLT

Stops the clock on the CPU.

## RET

Reverts the IP to the previous memory address stored in RP.

# Instruction Set

## About

This file lists all available syscalls on the TPU.

The syscall number must be loaded into EAX (see [InstructionSet.md](InstructionSet.md)).

## Syscalls

| EAX | Syscall | Description |
|-----|---------|-------------|
| 22  | Halt    | Informs the kernel to clean up and then stop the TPU. |

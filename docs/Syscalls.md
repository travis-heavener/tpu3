# Instruction Set

## About

This file lists all available syscalls on the TPU.

The syscall number must be loaded into EAX (see [InstructionSet.md](InstructionSet.md)).
Additionally, each syscall sets EAX to 0xFFFFFFFF on error or 0 if successful.

## Syscalls

| EAX | Syscall | Description |
|-----|---------|-------------|
| 1   | Write   | Writes a string starting at the address in ESI of length ECX to the file descriptor in EBX (1 for stdout, 2 for stderr). |
| 2   | Read    | Reads from a file descriptor (EBX, 0 for stdin) to a buffer (EDI) up to a length in ECX. Sets EAX to the number of bytes read. |
| 9   | Time    | Returns the number of seconds since the Epoch in EBX. |
| 22  | Halt    | Informs the kernel to clean up and then stop the TPU. |

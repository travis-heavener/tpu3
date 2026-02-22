# TPU Assembler (TASM) Usage Guide

## Build Instructions

#### `make tasm`

## Usage

#### `<tasm> /path/to/tasm/file`

The TPU Assembler (TASM) takes `.tsm` files and generates a binary `.tpu` file to run on the TPU.

To invoke TASM, run `bin/tasm` from your terminal.

Note: `bin/tasm` is actually just a symbolic link to `tasm/tasm.py`.

## .TPU File Format

The first 4 bytes of this file point to the address of the first instruction of the program.

The next 4 bytes indicate the size of the instructions (meaning the call stack starts at this first address + these 4 bytes).

All memory after these first 8 bytes and before the first instruction bytes are reserved for the read-only .data section.

The call stack is positioned immediately after the program's instructions and takes up 8 KiB, which is followed by the 16 KiB stack.

Everything after this is free memory that the program can use as it pleases.

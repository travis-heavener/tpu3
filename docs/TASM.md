# TPU Assembler (TASM) Usage Guide

## Build Instructions

#### `make tasm`

## Usage

#### `<tasm> /path/to/tasm/file`

The TPU Assembler (TASM) takes `.tsm` files and generates a binary `.tpu` file to run on the TPU.

To invoke TASM, run `bin/tasm` from your terminal.

Note: `bin/tasm` is actually just a symbolic link to `tasm/tasm.py`.

### Specifications

- TASM, like the TPU, uses little endian byte representation.
- ALL signed values use Two's Complement (which the TPU requires as of C++20).

## .TPU File Format

The first 7 bytes of this file are a JMP instruction to the start of the instructions for the program.
Everything between this instruction and the instructions (text section) is the data section.

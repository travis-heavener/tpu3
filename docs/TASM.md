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

The first 4 bytes are the length of the kernel and kernel-data segments.
The next 4 bytes are the length of the text and data segments.

Immediately after the text and kernel segments are their respective data and kernel-data segments, if there are any.

The TPU will load the kernel section into reserved memory and start executing from it.
The first 7 bytes of the kernel segment are a JMP instruction to the start of the instructions for the kernel program.

The first 7 bytes of the text segment are a JMP instruction to the start of the instructions for the user program, which is loaded into user space.

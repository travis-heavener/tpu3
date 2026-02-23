# TPU Usage Guide

## Build Instructions

#### `make tpu`

## Usage

#### `<tpu> /path/to/image.tpu`

To use the TPU, run `bin/tpu` from your terminal.

See [TASM.md](TASM.md) for a guide on the .TPU File Format.

## Memory Usage

Because the TPU has to store the TPU program image in memory, refer to the following for information about how physical memory is mapped and reserved:

- 0x0000_0000 to 0x0003_FFFF (inclusive): 256 KiB reserved for the TPU's kernel image
    - 0x0000_0100 to 0x0000_04FF (inclusive): 1 KiB for the syscall pointers table
    - 0x0000_0500 to 0x0001_04FF (inclusive): 64 KiB for the kernel stack
    - 0x0001_0500 to 0x0003_04FF (inclusive): 128 KiB for the actual TPU's kernel image
- 0x0004_0000 to 0xFFFF_FFFF (inclusive): memory for the TPU image, heap, and stack segments, depending on how much memory is allocated to the TPU
    - The order of segments is as such:
        - User stack: starts 0x0300_0000 bytes before the end of allocated memory
        - User heap: starts 0x0400_0000 bytes before the stack
        - User program image: starts at 0x0004_0000 and continues up to the heap start

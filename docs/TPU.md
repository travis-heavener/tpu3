# TPU Usage Guide

## Build Instructions

#### `make tpu`

## Usage

#### `<tpu> /path/to/image.tpu`

To use the TPU, run `bin/tpu` from your terminal.

See [TASM.md](TASM.md) for a guide on the .TPU File Format.

## Memory Usage

Because the TPU has to store the TPU program image in memory, refer to the following for information about how physical memory is mapped and reserved:

- 0x0000_0000 to 0x0003_FFFF (inclusive): 256 KiB reserved for the TPU's program image
    - 0x0000_0100 to 0x0000_04FF (inclusive): 1 KiB for the syscall pointers table
    - 0x0000_0500 to 0x0001_04FF (inclusive): 64 KiB for the kernel stack
- 0x0004_0000 to 0xFFFF_FFFF (inclusive): free memory for the TPU image to utilize as it sees fit, depending on how much memory is available to the TPU's virtual environment

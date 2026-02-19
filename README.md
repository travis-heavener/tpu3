# TPU3
## Travis Heavener

## About
The 2026 revival of the TPU (Travis's Processing Unit), this time with a more robust testing suite, CI/CD pipeline, and more comprehensive development methodology.

The TPU is an 8086-inspired, emulated, little-endian CPU that has historically run on a 16-bit architecture.
However this time around, the TPU will be a **32-bit processor**.
This change will fundamentally improve the scalability of programs that can be run on the TPU.

### What's Changed

The last version of the TPU (see [TPU2](https://github.com/travis-heavener/tpu2)) started small but lacked a few core features that I'll be implementing here:

- **Using a test suite, like I've done for [Mercury](https://wowtravis.com/mercury) (to ensure that new features don't break old ones)**
- Using GitHub Actions for CI/CD test runs
- Incrementally expanding the scope (not jumping into a C-style language/kernel/OS all at once)
- Properly using a single branch for development
- Restricting deployments to Linux only (not Windows, for now?)

## Build Instructions

1. Run `make init` when you first clone the repository.
2. Build a TPU image binary to run on the TPU via `bin/tasm path/to/image.tpu`.
3. Load the image to the TPU and execute it via `bin/tpu path/to/image.tpu`.

### Directory Layout

The TPU3 project consists of a few modular pieces, each compiled from the GNU Makefile.

- `tpu/`: Source for the TPU itself (see [TPU.md](docs/TPU.md))
- `tasm/`: Source for the TPU Assembler (see [TASM.md](docs/TASM.md))

#!/usr/bin/env python3
from sys import argv

SIZE = 4096 # Size of file, in bytes

if __name__ == "__main__":
    # Begin parsing the input file
    if len(argv) < 2:
        print("Error:\n  Invalid usage: <tasm> /path/to/tasm/file")
        exit(1)

    if not argv[1].lower().endswith(".tpu"):
        print("Error:\n  Invalid destination file, must have .TPU extension")
        exit(1)

    # Open file for writing
    with open(argv[1], "wb") as f:
        # Zero out the file
        f.seek(SIZE - 1)
        f.write(b"\0")

        # Write instruction
        f.seek(0)
        f.write(b"\xEF\xBE\x00\x00")

        f.seek(0x0000BEEF)
        f.write(b"\x00")

    print("Successfully built TPU image at:", argv[1])


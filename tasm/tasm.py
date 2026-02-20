#!/usr/bin/env python3
from sys import argv

from parser import parse_input

if __name__ == "__main__":
    # Begin parsing the input file
    if len(argv) < 3:
        print("Usage: <tasm> /path/to/input.tsm /path/to/output.tpu")
        exit(1)

    if not argv[1].lower().endswith(".tsm"):
        print("Invalid input file, must have .tsm extension")
        exit(1)

    if not argv[2].lower().endswith(".tpu"):
        print("Invalid output file, must have .tpu extension")
        exit(1)

    # Parse the input file
    data: list[int] = []
    try:
        parse_input(argv[1], data)
    except Exception as e:
        print(e)
    print("DATA:", data)

    # Open file for writing
    with open(argv[2], "wb") as f:
        # # Zero out the file
        # f.seek(SIZE - 1)
        # f.write(b"\0")

        # # Write instruction start address
        # f.seek(0)
        # f.write(b"\xEF\xBE\x00\x00")

        # # Write instruction
        # f.seek(0x0000BEEF) # <-- start of instructions
        pass

    print("Successfully built TPU image at:", argv[1])

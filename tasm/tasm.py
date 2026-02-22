#!/usr/bin/env python3
from sys import argv

from assembler import parse_input

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

    if len(data) == 7: # First jmp instruction is 7 bytes
        print("ERROR: Missing \"text\" section, nothing to build.")
        exit(1)

    # Open file for writing
    with open(argv[2], "wb") as f:
        # Write instructions (text segment)
        for byte in data:
            f.write(byte.to_bytes(1, "little", signed=False))

    print("Successfully built TPU image at:", argv[1])

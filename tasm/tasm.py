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

        # Write instruction start address
        f.seek(0)
        f.write(b"\xEF\xBE\x00\x00")

        # Write instruction
        f.seek(0x0000BEEF) # <-- start of instructions
        f.write(b"\x07\x00\x0B\x22") # <-- mov CL, 0x22
        f.write(b"\x07\x01\x12\xEF\x3E") # <-- mov SP, 0x3EEF
        f.write(b"\x07\x02\x13\xEF\xBE\xAD\xDE") # <-- mov EBP, 0xDEADBEEF
        f.write(b"\x07\x03\x0E\x0B") # <-- mov DH, CL
        f.write(b"\x07\x04\x01\x12") # <-- mov AX, SP
        f.write(b"\x07\x05\x08\x10") # <-- mov ECX, IP
        # f.write(b"\x06\x00\xEF\xBE\x00\x00") # <-- jmp <0x0000BEEF> (loop)
        f.write(b"\x09\x00\x0E\x14\x00\x00\x00") # <-- sb DH, 0x14
        f.write(b"\x09\x01\x12\x15\x00\x00\x00") # <-- sw SP, 0x15
        f.write(b"\x09\x02\x08\x17\x00\x00\x00") # <-- sdw ECX, 0x17
        f.write(b"\x08\x00\x0F\x14\x00\x00\x00") # <-- lb DL, 0x14
        f.write(b"\x08\x01\x16\x15\x00\x00\x00") # <-- lw SI, 0x15
        f.write(b"\x08\x02\x17\x17\x00\x00\x00") # <-- ldw EDI, 0x17
        f.write(b"\x01") # <-- HLT

    print("Successfully built TPU image at:", argv[1])

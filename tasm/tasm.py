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
        # f.write(b"\x07\x00\x0B\x22") # <-- mov CL, 0x22
        # f.write(b"\x07\x01\x12\xEF\x3E") # <-- mov SP, 0x3EEF
        # f.write(b"\x07\x02\x13\xEF\xBE\xAD\xDE") # <-- mov EBP, 0xDEADBEEF
        # f.write(b"\x07\x03\x0E\x0B") # <-- mov DH, CL
        # f.write(b"\x07\x04\x01\x12") # <-- mov AX, SP
        # f.write(b"\x07\x05\x08\x10") # <-- mov ECX, IP

        # f.write(b"\x06\x00\xEF\xBE\x00\x00") # <-- jmp <0x0000BEEF> (loop)

        # f.write(b"\x09\x00\x0E\x14\x00\x00\x00") # <-- sb DH, 0x14
        # f.write(b"\x09\x01\x12\x15\x00\x00\x00") # <-- sw SP, 0x15
        # f.write(b"\x09\x02\x08\x17\x00\x00\x00") # <-- sdw ECX, 0x17
        # f.write(b"\x08\x00\x0F\x14\x00\x00\x00") # <-- lb DL, 0x14
        # f.write(b"\x08\x01\x16\x15\x00\x00\x00") # <-- lw SI, 0x15
        # f.write(b"\x08\x02\x17\x17\x00\x00\x00") # <-- ldw EDI, 0x17

        # f.write(b"\x07\x00\x0E\x22") # <-- mov DH, 0x22
        # f.write(b"\x0A\x02\x0D") # <-- push DX
        # f.write(b"\x0B\x02\x01") # <-- pop AX

        # f.write(b"\x07\x00\x0E\x80") # <-- mov DH, 0
        # f.write(b"\x07\x01\x09\x21\x00") # <-- mov CX, 0x21
        # f.write(b"\x07\x02\x13\xEF\xBE\xAD\xDE") # <-- mov EBP, 0xDEADBEEF
        # f.write(b"\x20\x00\x0E") # <-- buf DH
        # f.write(b"\x20\x01\x09") # <-- buf CX
        # f.write(b"\x20\x02\x13") # <-- buf EBP

        # f.write(b"\x07\x00\x0E\x00") # <-- mov DH, 0
        # f.write(b"\x07\x01\x09\x21\x00") # <-- mov CX, 0x21
        # f.write(b"\x07\x02\x13\xEF\xBE\xAD\xDE") # <-- mov EBP, 0xDEADBEEF
        # f.write(b"\x21\x00\x0E\xFF")
        # f.write(b"\x21\x01\x09\xDE\xFF")
        # f.write(b"\x21\x02\x13\x10\x41\x52\x21")

        # Test unsigned/sign addition
        # f.write(b"\x07\x00\x0E\x80") # <-- mov DH, 0x80
        # f.write(b"\x2A\x00\x0E\x12") # <-- add DH, 0x12
        # f.write(b"\x2A\x08\x0E\x80") # <-- sadd DH, 0x80
        # f.write(b"\x07\x01\x09\x21\x80") # <-- mov CX, 0x21
        # f.write(b"\x2A\x01\x09\x12\x80") # <-- add CX, 0x12
        # f.write(b"\x2A\x09\x09\x80\x80") # <-- sadd CX, 0x80
        # f.write(b"\x07\x02\x13\xEF\xBE\xAD\xDE") # <-- mov EBP, 0xDEADBEEF
        # f.write(b"\x2A\x02\x13\x12\x00\xAD\xDE") # <-- add EBP, 0xDEAD0012
        # f.write(b"\x2A\x0A\x13\x00\x00\x00\x80") # <-- sadd EBP, 0x80000000

        # f.write(b"\x07\x00\x03\x05") # mov AL, 5
        # f.write(b"\x2C\x00\x0A") # mul AL, 10
        # f.write(b"\x07\x00\x03\xFF") # mov AL, 0xFF
        # f.write(b"\x2C\x00\x02") # mul AL, 2
        # f.write(b"\x07\x00\x03\xF6") # mov AL, 0xF6
        # f.write(b"\x2C\x08\x02") # smul AL, 2
        # f.write(b"\x07\x00\x03\x64") # mov AL, 100
        # f.write(b"\x2C\x08\x02") # smul AL, 2

        # f.write(b"\x07\x01\x01\xE8\x03") # mov AX, 1000
        # f.write(b"\x2C\x01\x0A\x00") # mul AX, 10
        # f.write(b"\x07\x01\x01\xE8\x03") # mov AX, 1000
        # f.write(b"\x2C\x01\xE8\x03") # mul AX, 1000
        # f.write(b"\x07\x01\x01\x18\xFC") # mov AX, -1000
        # f.write(b"\x2C\x09\x0A\x00") # smul AX, 10
        # f.write(b"\x07\x01\x01\x20\x4E") # mov AX, 20000
        # f.write(b"\x2C\x09\x04\x00") # smul AX, 4

        # f.write(b"\x07\x02\x00\xA0\x86\x01\x00") # mov EAX, 100000
        # f.write(b"\x2C\x02\x03\x00\x00\x00") # mul EAX, 3
        # f.write(b"\x07\x02\x00\xFF\xFF\xFF\xFF") # mov EAX, 0xFFFFFFFF
        # f.write(b"\x2C\x02\x02\x00\x00\x00") # mul EAX, 2
        # f.write(b"\x07\x02\x00\xC0\xF2\xFC\xFF") # mov EAX, -200000
        # f.write(b"\x2C\x0A\x03\x00\x00\x00") # smul EAX, 3
        # f.write(b"\x07\x02\x00\x00\x00\x00\x80") # mov EAX, 0x80000000
        # f.write(b"\x2C\x0A\xFF\xFF\xFF\xFF") # smul EAX, -1

        f.write(b"\x01") # <-- HLT

    print("Successfully built TPU image at:", argv[1])

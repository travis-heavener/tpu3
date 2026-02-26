from pathlib import Path
import re

from instructions import *

# Returns a line without any additional comments on it
def remove_comments(line: str) -> str:
    # Find first comment character (;)
    in_str_literal = False

    for i in range(len(line)):
        c = line[i]
        match c:
            case '"':
                in_str_literal = not in_str_literal
            case '\\':
                # Skip escaped string literal
                if in_str_literal:
                    i += 1
            case ';':
                if not in_str_literal:
                    return line[:i]

    # Base case
    return line

# Assembles the arguments of a line into a list
def assemble_args(parts: list[str]) -> tuple[Arg]:
    args: list[Arg] = []

    for part in parts:
        if reg := re.match(r"^0x[0-9a-fA-F]+$", part): # Hex literals
            args.append( Arg( type=ArgType.IMM, value=int(reg.group()[2:], 16) ) )
        elif reg := re.match(r"^\d+$", part): # Decimal literals
            args.append( Arg( type=ArgType.IMM, value=int(reg.group()) ) )

        elif reg := re.match(r"^[\-\+]0x[0-9a-fA-F]+$", part): # SIGNED Hex literals
            sign = -1 if reg.group()[0] == "-" else 1
            args.append( Arg( type=ArgType.SIMM, value=(sign * int(reg.group()[3:], 16)) ) )
        elif reg := re.match(r"^[\-\+]\d+$", part): # SIGNED Decimal literals
            sign = -1 if reg.group()[0] == "-" else 1
            args.append( Arg( type=ArgType.SIMM, value=(sign * int(reg.group()[1:])) ) )

        elif reg := re.match(r"^AL|AH|BL|BH|CL|CH|DL|DH$", part): # reg8
            args.append( Arg( type=ArgType.REG8, value=regcode(reg.group()) ) )
        elif reg := re.match(r"^AX|BX|CX|DX|SP|BP|SI|DI$", part): # reg16
            args.append( Arg( type=ArgType.REG16, value=regcode(reg.group()) ) )
        elif reg := re.match(r"^EAX|EBX|ECX|EDX|ESP|EBP|ESI|EDI|RP$", part): # reg32
            args.append( Arg( type=ArgType.REG32, value=regcode(reg.group()) ) )

        elif reg := re.match(r"^[_a-zA-Z][_a-zA-Z0-9]*$", part): # Labels
            args.append( Arg( type=ArgType.LABEL, value=reg.group() ) )
        elif reg := re.match(r"^@0x[0-9a-fA-F]+$", part): # Hex Addresses
            args.append( Arg( type=ArgType.ADDR, value=int(reg.group()[3:], 16) ) )
        elif reg := re.match(r"^\[\s*(EAX|EBX|ECX|EDX|ESP|EBP|ESI|EDI|RP|IP)\s*([\-\+])\s*(0x[0-9a-fA-F]+|\d+)\s*\]$", part): # rel32
            sign = -1 if reg.group(2) == "-" else 1
            offset_base = 16 if reg.group(3).startswith("0x") else 10
            args.append(Arg(
                type=ArgType.REL32,
                value=( sign * int(reg.group(3)[(2 if offset_base == 16 else 0):], offset_base) ),
                relreg=regcode(reg.group(1))
            ))
        else:
            raise TASMError(f"Invalid argument: {part}")

    # Tuple-ize for fun
    return tuple( args )

"""
STATIC FIELDS FOR ASSEMBLER
"""
t_text: list[int] = []  # text segment instructions
k_text: list[int] = []  # kernel segment instructions
t_data: list[int] = []  # data segment
k_data: list[int] = []  # kernel-data segment

# A list of labels that must be resolved for kernel and text
t_labels_to_replace: list[Label] = []
k_labels_to_replace: list[Label] = []

# The offset positions of known labels for kernel and text
t_text_labels: dict[str, int] = {}
k_text_labels: dict[str, int] = {}
t_data_labels: dict[str, int] = {}
k_data_labels: dict[str, int] = {}

# The included file stack, to prevent cyclical includes
include_stack: list[str] = []

def assemble_file(fname: str) -> None:
    # Detect cyclical imports
    full_fname = Path(fname).resolve()
    if full_fname in include_stack:
        raise TASMError(f"Cyclical import of {full_fname}")
    include_stack.append( full_fname )

    section: str = None
    line_num = 0

    with open(fname, "r") as f:
        # Update TASMError static fields
        TASMError.fname = full_fname
        TASMError.line = line_num

        for line in f:
            # Update TASMError's line ref
            line_num += 1
            TASMError.line = line_num

            line = line.replace("\r", "").replace("\n", "") # Remove CR & LF
            line = remove_comments(line).strip()            # Remove comments
            if len(line) == 0: continue                     # Skip blank lines

            # Check for file include
            if reg := re.match(r"^include\s+(.+)$", line):
                # Assemble file
                fname_dir = Path(fname).resolve().parent
                assemble_file( fname_dir / reg.group(1) )

                # Update fname
                TASMError.fname = fname
                continue

            # Check for data segment type
            if reg := re.match(r"^(u8|u16|u32|s8|s16|s32|str|strz|space)\s+([_a-zA-Z][_a-zA-Z0-9]*)\s+(.+)$", line):
                if section not in ("data", "kernel-data"):
                    raise TASMError("Cannot use data labels in non-data section")
                datatype = reg.group(1)
                label_name = reg.group(2)
                literal = reg.group(3)

                # Append label
                if section == "data":   t_data_labels[label_name] = len(t_data)
                else:                   k_data_labels[label_name] = len(k_data)

                # Parse data
                parse_data_label(datatype, literal, t_data if section == "data" else k_data)
                continue

            # Check for new section
            if reg := re.match(r"^section (.+)$", line):
                section = reg.group(1)
                if section not in ("text", "data", "kernel", "kernel-data"):
                    raise TASMError(f"Invalid section: {section}")
                continue

            # Check for label
            if reg := re.match(r"^([_a-zA-Z][_a-zA-Z0-9]*):$", line):
                label_name = reg.group(1)

                if (section == "text" and label_name in {**t_data_labels, **t_text_labels}) \
                    or (section == "kernel" and label_name in {**k_data_labels, **k_text_labels}):
                    raise TASMError(f"Duplicate label: {label_name}")

                # Insert label
                if section == "text":       t_text_labels[label_name] = len(t_text)
                elif section == "kernel":   k_text_labels[label_name] = len(k_text)
                continue

            if section == "text" or section == "kernel":
                # Check for instructions
                inst = line.split(" ")[0]
                remainder = line.replace(inst, "", 1)
                parts = [p.strip() for p in remainder.split(",")] if remainder.strip() else []

                if "" in parts:
                    raise TASMError("Syntax error")

                args: list[Arg] = assemble_args( parts )

                if section == "kernel":
                    text = k_text
                    labels_to_replace = k_labels_to_replace
                else:
                    text = t_text
                    labels_to_replace = t_labels_to_replace

                match inst:
                    case "nop":                         text.append(Inst.NOP)
                    case "syscall":                     text.append(Inst.SYSCALL)
                    case "sysret":                      text.append(Inst.SYSRET)
                    case "call":                        assembleJMPLike(inst, args, text, labels_to_replace)
                    case "ret":                         text.append(Inst.RET)
                    case "jmp" | "jz" | "jnz" \
                        | "jc" | "jnc" | "jo" | "jno" \
                        | "js" | "jns" | "jp" | "jnp":  assembleJMPLike(inst, args, text, labels_to_replace)
                    case "dbg":                         text.append(Inst.DBG)
                    case "hlt":                         text.append(Inst.HLT)
                    case "uret":                        assembleURET(args, text)
                    case "setsyscall":                  assembleSETSYSCALL(args, text, labels_to_replace)
                    case "mov":                         assembleMOV(args, text, labels_to_replace)
                    case "lb" | "lw" | "ldw" \
                        | "sb" | "sw" | "sdw":          assembleLOADSAVE(inst, args, text, labels_to_replace)
                    case "push" | "pushw" | "pushdw":   assemblePUSH(inst, args, text)
                    case "pop" | "popw" | "popdw":      assemblePOP(inst, args, text)
                    case "cmp" | "scmp" \
                        | "and" | "or" | "xor":         assembleArith2(inst, args, text)
                    case "not":                         assembleNOT(args, text)
                    case "add" | "sadd" \
                        | "sub" | "ssub":               assembleArith2(inst, args, text)
                    case "mul" | "smul":                assembleArith1(inst, args, text)
                    case _: raise TASMError(f"Invalid instruction: {inst}")
            else:
                raise TASMError(f"Invalid section: {section}")

    # Remove this file from the include stack
    include_stack.pop()

# Reads the input file to assemble each line
def parse_input(fname: str, master_data: list[int]) -> None:
    # Add jmp instruction to start labels
    t_text.extend( [Inst.JMP, 0, regcode("IP"), 0, 0, 0, 0] )
    t_labels_to_replace.append( Label(name="_start", replace_pos=3, current_ip=7) )

    k_text.extend( [Inst.JMP, 0, regcode("IP"), 0, 0, 0, 0] )
    k_labels_to_replace.append( Label(name="_kernel_start", replace_pos=3, current_ip=7) )

    # Assemble the file
    assemble_file(fname)

    # Replace text labels
    for label in t_labels_to_replace:
        # Lookup label
        if not label.name in t_text_labels and not label.name in t_data_labels:
            TASMError.fname = label.fname
            TASMError.line = label.line
            raise TASMError(f"Unable to locate text segment label: {label.name}")

        # Calculate offset
        if label.name in t_text_labels:
            offset = t_text_labels[label.name] - label.current_ip
        else:
            offset = len(t_text) + t_data_labels[label.name] - label.current_ip
        insert_label_offset(offset=offset, insert_at=label.replace_pos, data=t_text)

    # Replace kernel labels
    for label in k_labels_to_replace:
        # Lookup label
        if not label.name in k_text_labels and not label.name in k_data_labels:
            TASMError.fname = label.fname
            TASMError.line = label.line
            raise TASMError(f"Unable to locate kernel segment label: {label.name}")

        # Calculate offset
        if label.name in k_text_labels:
            offset = k_text_labels[label.name] - label.current_ip
        else:
            offset = len(k_text) + k_data_labels[label.name] - label.current_ip
        insert_label_offset(offset=offset, insert_at=label.replace_pos, data=k_text)

    # Insert to master data
    imm_to_bytes( len(k_text) + len(k_data), 32, master_data ) # Length of kernel image
    imm_to_bytes( len(t_text) + len(t_data), 32, master_data ) # Length of user image

    master_data.extend( [*k_text, *k_data] ) # Append kernel image
    master_data.extend( [*t_text, *t_data] ) # Append user image

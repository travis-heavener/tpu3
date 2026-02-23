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

# Reads the input file to assemble each line
def parse_input(fname: str, data: list[int]) -> None:
    section: str = None

    # A list of labels that must be resolved
    labels_to_replace: list[Label] = []

    # The offset positions of known labels
    known_labels: dict[str, int] = {}

    # Init TASMError static fields
    line_num = 0
    TASMError.fname = fname
    TASMError.line = line_num
    
    # Add jmp instruction to _start label
    data.extend( [Inst.JMP, 0, regcode("IP"), 0, 0, 0, 0] )
    labels_to_replace.append( Label(name="_start", replace_pos=3, current_ip=7) )

    with open(fname, "r") as f:
        for line in f:
            # Update TASMError's line ref
            line_num += 1
            TASMError.line = line_num

            # Remove CR & LF
            line = line.replace("\r", "").replace("\n", "")

            # Remove comments
            line = remove_comments(line).strip()

            # Skip blank lines
            if len(line) == 0: continue

            # Check for new section
            if reg := re.match(r"^section (.+)$", line):
                section = reg.group(1)
                if section not in ("text", "data"):
                    raise TASMError(f"Invalid section: {section}")
                continue

            # Check for label
            if reg := re.match(r"^([_a-zA-Z][_a-zA-Z0-9]*):$", line):
                label_name = reg.group(1)

                if label_name in known_labels:
                    raise TASMError(f"Duplicate label: {label_name}")

                # Insert label
                known_labels[label_name] = len(data)
                continue

            if section == "text":
                # Check for instructions
                inst = line.split(" ")[0]
                remainder = line.replace(inst, "", 1)
                parts = [p.strip() for p in remainder.split(",")] if remainder.strip() else []

                if "" in parts:
                    raise TASMError("Syntax error")

                args: list[Arg] = assemble_args( parts )

                match inst:
                    case "nop":                         data.append(Inst.NOP)
                    case "syscall":                     data.append(Inst.SYSCALL)
                    case "sysret":                      data.append(Inst.SYSRET)
                    case "call":                        assembleJMPLike(inst, args, data, labels_to_replace)
                    case "ret":                         data.append(Inst.RET)
                    case "jmp" | "jz" | "jnz" \
                        | "jc" | "jnc" | "jo" | "jno" \
                        | "js" | "jns" | "jp" | "jnp":  assembleJMPLike(inst, args, data, labels_to_replace)
                    case "hlt":                         data.append(Inst.HLT)
                    case "uret":                        assembleURET(args, data)
                    case "mov":                         assembleMOV(args, data)
                    case "lb" | "lw" | "ldw" \
                        | "sb" | "sw" | "sdw":          assembleLOADSAVE(inst, args, data, labels_to_replace)
                    case "push" | "pushw" | "pushdw":   assemblePUSH(inst, args, data)
                    case "pop" | "popw" | "popdw":      assemblePOP(inst, args, data)
                    case "cmp" | "scmp" \
                        | "and" | "or" | "xor":         assembleArith2(inst, args, data)
                    case "not":                         assembleNOT(args, data)
                    case "add" | "sadd" \
                        | "sub" | "ssub":               assembleArith2(inst, args, data)
                    case "mul" | "smul":                assembleArith1(inst, args, data)
                    case _: raise TASMError(f"Invalid instruction: {inst}")
            elif section == "data":
                # Check for data
                pass
            else:
                raise TASMError(f"Invalid section: {section}")

    # Replace labels
    for label in labels_to_replace:
        # Lookup label
        if not label.name in known_labels:
            TASMError.fname = label.fname
            TASMError.line = label.line
            raise TASMError(f"Unable to locate label: {label.name}")

        # Calculate offset
        offset = known_labels[label.name] - label.current_ip
        insert_label_offset(offset=offset, insert_at=label.replace_pos, data=data)

import re

class TASMError(Exception):
    def __init__(self, fname: str, line: int, msg: str):
        super().__init__(f"TASMError:\n  From: {fname}:{line}\n    {msg}")

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

# Parses the arguments of a line into a list
def parse_args(parts: list[str]) -> tuple:
    args = []

    """

    TODO
    - Parsing literals
        - Signed & unsigned
        - simm8/simm16/simm32?
    - Distinguish rel32 from imm32
    - Parsing addresses
    - Offset notation
        - Would fix the rel32 addresses
        - [IP + offset], [EAX - offset]
        - ONLY for 32-bit registers
    - @addr

    """

    for part in parts:
        if reg := re.match(r"^0x[0-9a-fA-F]+$", part): # Hex literals
            args.append({ "type": "Immediate", "value": int(reg.group()[2:], 16) })
        elif reg := re.match(r"^0b[01]+$", part): # Binary literals
            args.append({ "type": "Immediate", "value": int(reg.group()[2:], 2) })
        elif reg := re.match(r"^\d+$", part): # Decimal literals
            args.append({ "type": "Immediate", "value": int(reg.group()) })
        elif reg := re.match(r"^AL|AH|BL|BH|CL|CH|DL|DH$", part): # reg8
            args.append({ "type": "Reg8", "value": regcode(reg.group()) })
        elif reg := re.match(r"^AX|BX|CX|DX|SP|BP|SI|DI$", part): # reg16
            args.append({ "type": "Reg16", "value": regcode(reg.group()) })
        elif reg := re.match(r"^EAX|EBX|ECX|EDX|ESP|EBP|ESI|EDI|RP$", part): # reg32
            args.append({ "type": "Reg32", "value": regcode(reg.group()) })
        elif reg := re.match(r"^[_a-zA-Z0-9]+$", part): # labels
            args.append({ "type": "Label", "value": regcode(reg.group()) })

    # Tuple-ize for fun
    return tuple( args )

# Parses the input file to 
def parse_input(fname: str, data: list[int]) -> None:
    section: str = None

    line_num = 0
    with open(fname, "r") as f:
        for line in f:
            line_num += 1

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
                    raise TASMError(fname, line_num, f"Invalid section: {section}")
                continue

            # Check for label
            if reg := re.match(r"^([_a-zA-Z0-9]+):$", line):
                label_name = reg.group(1)
                continue

            if section == "text":
                # Check for instructions
                inst = line.split(" ")[0]
                remainder = line.replace(inst, "", 1)
                parts = [p.strip() for p in remainder.split(",")] if remainder.strip() else []

                if "" in parts:
                    raise TASMError(fname, line_num, f"Syntax error")

                args = parse_args( parts )
                print(args)

                match inst:
                    case "nop": data.append(0x00)
                    case "hlt": data.append(0x01)
                    case "sysret": data.append(0x03)
                    case _: raise TASMError(fname, line_num, f"Invalid instruction: {inst}")
            elif section == "data":
                # Check for data
                pass
            else:
                raise TASMError(fname, line_num, f"Invalid section: {section}")

# Returns the regcode for the particular register string
def regcode(reg: str) -> int:
    return [
        "EAX", "AX", "AH", "AL", "EBX", "BX", "BH", "BL", "ECX", "CX", "CH", "CL", "EDX", "DX", "DH", "DL",
        "IP", "ESP", "SP", "EBP", "BP", "ESI", "SI", "EDI", "DI", "RP"
    ].index(reg)

import re

class Inst:
    # Control Instructions
    NOP     = 0x00
    SYSCALL = 0x01
    SYSRET  = 0x02
    CALL    = 0x03
    RET     = 0x04
    JMP     = 0x05
    JZ      = 0x06
    JC      = 0x07
    JO      = 0x08
    JS      = 0x09
    JP      = 0x0A

    # Kernel protected instructions
    HLT     = 0x15
    URET    = 0x16
    SETSYSCALL = 0x17

    # Register & Memory Instructions
    MOV     = 0x30
    LB      = 0x31
    SB      = 0x32
    PUSH    = 0x33
    POP     = 0x34

    # Bitwise & Arithmetic Instructions
    CMP     = 0x61
    AND     = 0x62
    OR      = 0x63
    XOR     = 0x64
    NOT     = 0x65
    ADD     = 0x6A
    SUB     = 0x6B
    MUL     = 0x6C

class AddressMode:
    RELATIVE = 0
    ABSOLUTE = 1

class ArgType:
    IMM =   0
    SIMM =  1
    REG8 =  2
    REG16 = 3
    REG32 = 4
    ADDR =  5
    REL32 = 6
    LABEL = 7

class Arg:
    def __init__(self, type: ArgType, value: any, relreg: int=None):
        self.type = type     # The argument's type
        self.value = value   # The argument's value
        self.relreg = relreg # The offest register for rel32

class Label:
    def __init__(self, name: str, replace_pos: int, current_ip: int):
        self.name = name
        self.replace_pos = replace_pos
        self.current_ip = current_ip
        self.fname = TASMError.fname
        self.line = TASMError.line

# How many bits to shift each flag in the control byte by
_SHIFT_SIGN =      3
_SHIFT_ADDR_MODE = 4

class TASMError(Exception):
    # Static fields
    fname: str = None
    line: int = -1

    def __init__(self, msg: str):
        super().__init__(f"TASMError: {TASMError.fname}:{TASMError.line}\n    {msg}")

# Returns True if the SIGNED value fits into the number of bits provided
def does_signed_fit(val: int, bits: int) -> bool:
    valmax = (2 ** (bits-1)) - 1
    valmin = -(2 ** (bits-1))
    return valmin <= val and val <= valmax

# Returns True if the UNSIGNED value fits into the number of bits provided
def does_unsigned_fit(val: int, bits: int) -> bool:
    return 0 <= val and val <= ((2 ** bits) - 1)

# Converts an UNSIGNED immediate value into its byte representation
# Will throw a TASMError if the value doesn't fit in range
def imm_to_bytes(val: int, bits: int, data: list[int]) -> None:
    if not does_unsigned_fit(val, bits):
        raise TASMError(f"Unsigned immediate doesn't fit into {bits} bits: {val}")

    # Break apart the value
    for b in range(0, bits, 8):
        data.append( (val >> b) & 0xFF )

# Converts a SIGNED immediate value into its byte representation (Two's Complement)
# Will throw a TASMError if the value doesn't fit in range
def simm_to_bytes(val: int, bits: int, data: list[int]) -> None:
    if not does_signed_fit(val, bits):
        raise TASMError(f"Signed immediate doesn't fit into {bits} bits: {val}")

    # Convert negative numbers (Two's Complement)
    if val < 0:
        val = (1 << bits) + val

    # Break apart the value
    for b in range(0, bits, 8):
        data.append( (val >> b) & 0xFF )

# Inserts the calculated offset to a named label into the relative instruction
def insert_label_offset(offset: int, insert_at: int, data: list[int]) -> None:
    # Convert negative numbers (Two's Complement)
    simm32 = offset
    if simm32 < 0:
        simm32 = (1 << 32) + + simm32

    # Insert into the array (little endian)
    data[insert_at]   =  simm32        & 0xFF
    data[insert_at+1] = (simm32 >> 8)  & 0xFF
    data[insert_at+2] = (simm32 >> 16) & 0xFF
    data[insert_at+3] = (simm32 >> 24) & 0xFF

# Returns the regcode for the particular register string
def regcode(reg: str) -> int:
    return [
        "EAX", "AX", "AH", "AL", "EBX", "BX", "BH", "BL", "ECX", "CX", "CH", "CL", "EDX", "DX", "DH", "DL",
        "IP", "ESP", "SP", "EBP", "BP", "ESI", "SI", "EDI", "DI", "RP"
    ].index(reg)

#################################################################################################
##################################### Data Segment Assembler ####################################
#################################################################################################

def parse_data_label(datatype: str, literal: str, data: list[int]) -> None:
    # Handle datatype
    match datatype:
        case "space":
            # Extract space size
            raw = re.match(r"^(0x[a-fA-F0-9]+|\d+)$", literal)
            if not raw: raise TASMError(f"Cannot parse {datatype}: {literal}")
            raw = raw.group(1)
            space_len = int(raw, 16) if raw.startswith("0x") else int(raw)

            # Add empty space
            for i in range(space_len):
                data.append(0)
        case "s8" | "s16" | "s32":
            raw = re.match(r"^([+-]0x[a-fA-F0-9]+|[+-]\d+)$", literal)
            if not raw: raise TASMError(f"Cannot parse {datatype}: {literal}")
            raw = raw.group(1)

            # Parse value
            width = int(datatype[1:])
            sign = -1 if raw[0] == "-" else 1
            raw = raw[1:] # Remove sign

            if raw.startswith("0x"):
                simm_to_bytes( sign * int(raw, 16), width, data )
            else:
                simm_to_bytes( sign * int(raw), width, data )
        case "u8" | "u16" | "u32":
            raw = re.match(r"^(0x[a-fA-F0-9]+|\d+)$", literal)
            if not raw: raise TASMError(f"Cannot parse {datatype}: {literal}")
            raw = raw.group(1)
            width = int(datatype[1:])

            if raw.startswith("0x"):
                simm_to_bytes( int(raw, 16), width, data )
            else:
                simm_to_bytes( int(raw), width, data )
        case "str" | "strz":
            raw = re.match(r'^".*"$', literal)
            if not raw: raise TASMError(f"Cannot parse {datatype}: {literal}")

            # Check for unescaped backslashes
            escapes = {
                "\\\\": "\\",
                "\\\"": "\"",
                "\\'": "'",
                "\\t": "\t",
                "\\n": "\n",
                "\\r": "\r",
                "\\0": "\0"
            }

            # Escape characters & build string
            i = 1
            while i < len(literal) - 1:
                if literal[i] == "\\":
                    if i + 2 == len(literal):
                        raise TASMError(f"Cannot parse {datatype}: {literal}")
                    elif literal[i:i+2] in escapes:
                        data.append( ord(escapes[literal[i:i+2]]) )
                        i += 1
                    else:
                        raise TASMError(f"Cannot parse {datatype}: {literal}")
                else:
                    data.append( ord(literal[i]) )
                i += 1

            # Append null terminator
            if datatype == "strz":
                data.append( 0 )

#################################################################################################
################################# Instruction Assembler Methods #################################
#################################################################################################

def assembleJMPLike(inst: str, args: tuple[Arg], data: list[int], labels: list[Label]) -> None:
    # Check args length
    if len(args) != 1: raise TASMError(f"Invalid number of arguments for {inst.upper()}: {len(args)}")

    match inst:
        case "call":        data.append(Inst.CALL)
        case "jmp":         data.append(Inst.JMP)
        case "jz" | "jnz":  data.append(Inst.JZ)
        case "jc" | "jnc":  data.append(Inst.JC)
        case "jo" | "jno":  data.append(Inst.JO)
        case "js" | "jns":  data.append(Inst.JS)
        case "jp" | "jnp":  data.append(Inst.JP)

    is_inverse_flag = inst in ("jnz", "jnc", "jno", "jns", "jnp")

    # Determine control byte
    cbyte = 0
    if args[0].type == ArgType.REL32 or args[0].type == ArgType.LABEL:
        cbyte  = 0                                          # MOD
        cbyte += 2 if is_inverse_flag else 0                # Differentiate jz vs jnz, for example
        cbyte |= (AddressMode.RELATIVE) << _SHIFT_ADDR_MODE # Addressing mode
        data.append(cbyte)                                  # Append control byte

        if args[0].type == ArgType.LABEL:
            data.append(regcode("IP"))  # Append offset register (ALWAYS IP FOR LABELS)
            replace_pos = len(data)     # Store replacement position for label offset
            data.extend([0, 0, 0, 0])   # Append placeholder offset

            # Store label to be replaced
            labels.append( Label(name=args[0].value, replace_pos=replace_pos, current_ip=len(data)) )
        else:
            data.append(args[0].relreg)            # Append offset register
            simm_to_bytes(args[0].value, 32, data) # Append signed offset address
    elif args[0].type == ArgType.ADDR:
        cbyte  = 0                                          # MOD
        cbyte += 2 if is_inverse_flag else 0                # Differentiate jz vs jnz, for example
        cbyte |= (AddressMode.ABSOLUTE) << _SHIFT_ADDR_MODE # Addressing mode
        data.append(cbyte)                                  # Append control byte
        imm_to_bytes(args[0].value, 32, data)               # Append absolute address
    elif args[0].type == ArgType.REG32:
        cbyte = 1                               # MOD
        cbyte += 2 if is_inverse_flag else 0    # Differentiate jz vs jnz, for example
        data.append(cbyte)                      # Append control byte
        data.append(args[0].value)              # Append regcode
    else:
        raise TASMError(f"Invalid argument format to {inst.upper()}")

#################################################################################################
################################### Kernel Protected Methods ####################################
#################################################################################################

def assembleURET(args: tuple[Arg], data: list[int]) -> None:
    # Check args length
    if len(args) != 2: raise TASMError(f"Invalid number of arguments for URET: {len(args)}")

    data.append(Inst.URET)

    if args[0].type != ArgType.ADDR or args[1].type != ArgType.ADDR:
        raise TASMError("Invalid argument format to URET")

    # Append addresses
    imm_to_bytes(args[0].value, 32, data) 
    imm_to_bytes(args[1].value, 32, data) 

def assembleSETSYSCALL(args: tuple[Arg], data: list[int], labels: list[Label]) -> None:
    # Check args length
    if len(args) != 2: raise TASMError(f"Invalid number of arguments for SETSYSCALL: {len(args)}")

    data.append(Inst.SETSYSCALL)

    if args[0].type == ArgType.IMM and args[1].type in (ArgType.REL32, ArgType.LABEL):
        # Append syscall number
        imm_to_bytes(args[0].value, 8, data)

        if args[1].type == ArgType.LABEL:
            data.append(regcode("IP"))  # Append offset register (ALWAYS IP FOR LABELS)
            replace_pos = len(data)     # Store replacement position for label offset
            data.extend([0, 0, 0, 0])   # Append placeholder offset

            # Store label to be replaced
            labels.append( Label(name=args[1].value, replace_pos=replace_pos, current_ip=len(data)) )
        else:
            data.append(args[1].relreg)            # Append offset register
            simm_to_bytes(args[1].value, 32, data) # Append signed offset address
    else:
        raise TASMError("Invalid argument format to SETSYSCALL")

#################################################################################################
################################### Register & Memory Methods ###################################
#################################################################################################

def assembleMOV(args: tuple[Arg], data: list[int], labels: list[Label]) -> None:
    # Check args length
    if len(args) != 2: raise TASMError(f"Invalid number of arguments for MOV: {len(args)}")

    data.append(Inst.MOV)

    # Determine MOD bits
    if args[0].type == ArgType.REG8 and (args[1].type == ArgType.IMM or args[1].type == ArgType.SIMM):
        data.append( 0 )                # Append MOD
        data.append( args[0].value )    # Append first reg

        # Add immediate
        if args[1].type == ArgType.SIMM:
            simm_to_bytes( args[1].value, 8, data )
        else:
            imm_to_bytes( args[1].value, 8, data )
    elif args[0].type == ArgType.REG16 and (args[1].type == ArgType.IMM or args[1].type == ArgType.SIMM):
        data.append( 1 )                # Append MOD
        data.append( args[0].value )    # Append first reg

        # Add immediate
        if args[1].type == ArgType.SIMM:
            simm_to_bytes( args[1].value, 16, data )
        else:
            imm_to_bytes( args[1].value, 16, data )
    elif args[0].type == ArgType.REG32 and (args[1].type == ArgType.IMM or args[1].type == ArgType.SIMM):
        data.append( 2 )                # Append MOD
        data.append( args[0].value )    # Append first reg

        # Add immediate
        if args[1].type == ArgType.SIMM:
            simm_to_bytes( args[1].value, 32, data )
        else:
            imm_to_bytes( args[1].value, 32, data )
    elif args[0].type == ArgType.REG8 and args[1].type == ArgType.REG8:
        data.append( 3 )                # Append MOD
        data.append( args[0].value )    # Append first reg
        data.append( args[1].value )    # Append second reg
    elif args[0].type == ArgType.REG16 and args[1].type == ArgType.REG16:
        data.append( 4 )                # Append MOD
        data.append( args[0].value )    # Append first reg
        data.append( args[1].value )    # Append second reg
    elif args[0].type == ArgType.REG32 and args[1].type == ArgType.REG32:
        data.append( 5 )                # Append MOD
        data.append( args[0].value )    # Append first reg
        data.append( args[1].value )    # Append second reg
    elif args[0].type == ArgType.REG32 and args[1].type in (ArgType.REL32, ArgType.LABEL):
        data.append( 6 )                # Append MOD
        data.append( args[0].value )    # Append first reg

        if args[1].type == ArgType.LABEL:
            data.append(regcode("IP"))  # Append offset register (ALWAYS IP FOR LABELS)
            replace_pos = len(data)     # Store replacement position for label offset
            data.extend([0, 0, 0, 0])   # Append placeholder offset

            # Store label to be replaced
            labels.append( Label(name=args[1].value, replace_pos=replace_pos, current_ip=len(data)) )
        else:
            data.append(args[1].relreg)            # Append offset register
            simm_to_bytes(args[1].value, 32, data) # Append signed offset address
    else:
        raise TASMError("Invalid argument format to MOV")

def assembleLOADSAVE(inst: str, args: tuple[Arg], data: list[int], labels: list[Label]) -> None:
    # Check args length
    if len(args) != 2: raise TASMError(f"Invalid number of arguments for {inst.upper()}: {len(args)}")

    match inst:
        case "lb" | "lw" | "ldw": data.append(Inst.LB)
        case "sb" | "sw" | "sdw": data.append(Inst.SB)

    # Enforce width (e.g. lb only for reg8, lw only for reg16, ...)
    reg_width = 32 if inst.endswith("dw") else 16 if inst.endswith("w") else 8
    if (args[0].type == ArgType.REG8  and reg_width != 8)  or \
       (args[0].type == ArgType.REG16 and reg_width != 16) or \
       (args[0].type == ArgType.REG32 and reg_width != 32):
        raise TASMError(f"Invalid argument format to {inst.upper()}")

    if args[0].type in (ArgType.REG8, ArgType.REG16, ArgType.REG32) and (args[1].type == ArgType.REL32 or args[1].type == ArgType.LABEL):
        cbyte  = 0                                          # MOD if Reg8
        if args[0].type == ArgType.REG16: cbyte = 2         # MOD if Reg16
        if args[0].type == ArgType.REG32: cbyte = 4         # MOD if Reg32
        cbyte |= (AddressMode.RELATIVE) << _SHIFT_ADDR_MODE # Adddressing mode
        data.append(cbyte)                                  # Append control byte
        data.append(args[0].value)                          # Append first reg operand

        if args[1].type == ArgType.LABEL:
            data.append(regcode("IP"))  # Append offset register (ALWAYS IP FOR LABELS)
            replace_pos = len(data)     # Store replacement position for label offset
            data.extend([0, 0, 0, 0])   # Append placeholder offset

            # Store label to be replaced
            labels.append( Label(name=args[1].value, replace_pos=replace_pos, current_ip=len(data)) )
        else:
            data.append(args[1].reg)               # Append offset register
            simm_to_bytes(args[1].value, 32, data) # Append signed offset address
    elif args[0].type in (ArgType.REG8, ArgType.REG16, ArgType.REG32) and args[1].type == ArgType.ADDR:
        cbyte  = 0                                          # MOD if Reg8
        if args[0].type == ArgType.REG16: cbyte = 2         # MOD if Reg16
        if args[0].type == ArgType.REG32: cbyte = 4         # MOD if Reg32
        cbyte |= (AddressMode.ABSOLUTE) << _SHIFT_ADDR_MODE # Addressing mode
        data.append(cbyte)                                  # Append control byte

        data.append(args[0].value)              # Append first reg operand
        imm_to_bytes(args[1].value, 32, data)   # Append absolute address
    elif args[0].type in (ArgType.REG8, ArgType.REG16, ArgType.REG32) and args[1].type == ArgType.REG32:
        # Append control byte
        if args[0].type == ArgType.REG8:  data.append(1)
        if args[0].type == ArgType.REG16: data.append(3)
        if args[0].type == ArgType.REG32: data.append(5)

        data.append(args[0].value)  # Append first reg operand
        data.append(args[1].value)  # Append second reg operand
    else:
        raise TASMError(f"Invalid argument format to {inst.upper()}")

def assemblePUSH(inst: str, args: tuple[Arg], data: list[int]) -> None:
    # Check args length
    if len(args) != 1: raise TASMError(f"Invalid number of arguments for {inst.upper()}: {len(args)}")

    data.append(Inst.PUSH)

    # Enforce width (e.g. lb only for reg8, lw only for reg16, ...)
    inst_width = 32 if inst.endswith("dw") else 16 if inst.endswith("w") else 8
    if (args[0].type == ArgType.REG8  and inst_width != 8)  or \
       (args[0].type == ArgType.REG16 and inst_width != 16) or \
       (args[0].type == ArgType.REG32 and inst_width != 32):
        raise TASMError(f"Invalid argument format to {inst.upper()}")

    if args[0].type == ArgType.REG8:
        data.append(0)              # MOD
        data.append(args[0].value)  # Add regcode
    elif args[0].type == ArgType.REG16:
        data.append(2)              # MOD
        data.append(args[0].value)  # Add regcode
    elif args[0].type == ArgType.REG32:
        data.append(4)              # MOD
        data.append(args[0].value)  # Add regcode
    elif args[0].type == ArgType.IMM:
        val = args[0].value
        imm_width = 8 if does_unsigned_fit(val, 8) else 16 if does_unsigned_fit(val, 16) else 32

        # Validate instruction width vs argument width
        if inst_width != imm_width:
            raise TASMError(f"Invalid argument format to {inst.upper()}")

        # MOD
        data.append(1 if imm_width == 8 else 3 if imm_width == 16 else 5)
        imm_to_bytes(val, imm_width, data) # Add immediate
    elif args[0].type == ArgType.SIMM:
        val = args[0].value
        imm_width = 8 if does_signed_fit(val, 8) else 16 if does_signed_fit(val, 16) else 32

        # Validate instruction width vs argument width
        if inst_width != imm_width:
            raise TASMError(f"Invalid argument format to {inst.upper()}")

        # MOD
        data.append(1 if imm_width == 8 else 3 if imm_width == 16 else 5)
        simm_to_bytes(val, imm_width, data) # Add immediate
    else:
        raise TASMError(f"Invalid argument format to {inst.upper()}")

def assemblePOP(inst: str, args: tuple[Arg], data: list[int]) -> None:
    # Check args length
    if len(args) > 1: raise TASMError(f"Invalid number of arguments for {inst.upper()}: {len(args)}")

    data.append(Inst.POP)

    # Check instruction width
    inst_width = 32 if inst.endswith("dw") else 16 if inst.endswith("w") else 8

    # Check number of args
    if len(args) == 0:
        data.append(1 if inst_width == 8 else 3 if inst_width == 16 else 5)
        return

    # OTHERWISE, use the argument provided

    # Enforce width (e.g. lb only for reg8, lw only for reg16, ...)
    if (args[0].type == ArgType.REG8  and inst_width != 8)  or \
       (args[0].type == ArgType.REG16 and inst_width != 16) or \
       (args[0].type == ArgType.REG32 and inst_width != 32):
        raise TASMError(f"Invalid argument format to {inst.upper()}")

    if args[0].type == ArgType.REG8:
        data.append(0)              # MOD
        data.append(args[0].value)  # Add regcode
    elif args[0].type == ArgType.REG16:
        data.append(2)              # MOD
        data.append(args[0].value)  # Add regcode
    elif args[0].type == ArgType.REG32:
        data.append(4)              # MOD
        data.append(args[0].value)  # Add regcode
    else:
        raise TASMError(f"Invalid argument format to {inst.upper()}")

#################################################################################################
################################# Bitwise & Arithmetic Methods ##################################
#################################################################################################

def assembleArith1(inst: str, args: tuple[Arg], data: list[int]) -> None:
    # Check args length
    if len(args) != 1: raise TASMError(f"Invalid number of arguments for {inst.upper()}: {len(args)}")

    # Validate each instruction
    match inst:
        case "mul" | "smul": data.append(Inst.MUL)

    # Validate argument signedness
    A = args[0]
    is_signed = inst in ("smul",)

    if (is_signed and A.type == ArgType.IMM) or (not is_signed and A.type == ArgType.SIMM):
        raise TASMError(f"Invalid argument format to {inst.upper()}")

    # Determine MOD & args
    if A.type == ArgType.IMM:
        val = A.value
        if is_signed:
            imm_width = 8 if does_unsigned_fit(val, 8) else 16 if does_unsigned_fit(val, 16) else 32
        else:
            imm_width = 8 if does_signed_fit(val, 8) else 16 if does_signed_fit(val, 16) else 32

        # Determine control byte
        control_byte = 0 if imm_width == 8 else 1 if imm_width == 16 else 2
        if is_signed: control_byte |= (1 << _SHIFT_SIGN)
        data.append(control_byte)

        # Immediate
        if is_signed:
            simm_to_bytes(val, imm_width, data)
        else:
            imm_to_bytes(val, imm_width, data)
    elif A.type == ArgType.REG8:
        # Determine control byte
        control_byte = 3
        if is_signed: control_byte |= (1 << _SHIFT_SIGN)
        data.append(control_byte)
        data.append(A.value)
    elif A.type == ArgType.REG16:
        # Determine control byte
        control_byte = 4
        if is_signed: control_byte |= (1 << _SHIFT_SIGN)
        data.append(control_byte)
        data.append(A.value)
    elif A.type == ArgType.REG32:
        # Determine control byte
        control_byte = 5
        if is_signed: control_byte |= (1 << _SHIFT_SIGN)
        data.append(control_byte)
        data.append(A.value)
    else:
        raise TASMError(f"Invalid argument format to {inst.upper()}")

def assembleArith2(inst: str, args: tuple[Arg], data: list[int]) -> None:
    # Check args length
    if len(args) != 2: raise TASMError(f"Invalid number of arguments for {inst.upper()}: {len(args)}")

    # Validate each instruction
    match inst:
        case "cmp" | "scmp": data.append(Inst.CMP)
        case "and"         : data.append(Inst.AND)
        case "or"          : data.append(Inst.OR)
        case "xor"         : data.append(Inst.XOR)
        case "add" | "sadd": data.append(Inst.ADD)
        case "sub" | "ssub": data.append(Inst.SUB)

    # Validate argument signedness
    A, B = args
    is_signed = inst in ("scmp", "sadd", "ssub")

    if (is_signed and B.type == ArgType.IMM) or (not is_signed and B.type == ArgType.SIMM):
        raise TASMError(f"Invalid argument format to {inst.upper()}")

    # Determine MOD & args
    if A.type == ArgType.REG8 and B.type in (ArgType.IMM, ArgType.SIMM):
        control_byte = 0
        if B.type == ArgType.SIMM:
            control_byte |= (1 << _SHIFT_SIGN)

        data.append(control_byte)       # Control Byte (MOD)
        data.append(A.value)            # Regcode

        if is_signed: # Immediate
            simm_to_bytes(B.value, 8, data)
        else:
            imm_to_bytes(B.value, 8, data)
    elif A.type == ArgType.REG16 and B.type in (ArgType.IMM, ArgType.SIMM):
        control_byte = 1
        if B.type == ArgType.SIMM:
            control_byte |= (1 << _SHIFT_SIGN)

        data.append(control_byte)       # Control Byte (MOD)
        data.append(A.value)            # Regcode

        if is_signed: # Immediate
            simm_to_bytes(B.value, 16, data)
        else:
            imm_to_bytes(B.value, 16, data)
    elif A.type == ArgType.REG32 and B.type in (ArgType.IMM, ArgType.SIMM):
        control_byte = 2
        if B.type == ArgType.SIMM:
            control_byte |= (1 << _SHIFT_SIGN)

        data.append(control_byte)       # Control Byte (MOD)
        data.append(A.value)            # Regcode

        if is_signed: # Immediate
            simm_to_bytes(B.value, 32, data)
        else:
            imm_to_bytes(B.value, 32, data)
    elif A.type == B.type and A.type in (ArgType.REG8, ArgType.REG16, ArgType.REG32):
        control_byte = 3 if A.type == ArgType.REG8 else 4 if A.type == ArgType.REG16 else 5
        if B.type == ArgType.SIMM:
            control_byte |= (1 << _SHIFT_SIGN)

        data.append(control_byte)   # Control Byte (MOD)
        data.append(A.value)        # Regcode
        data.append(B.value)        # Regcode
    else:
        raise TASMError(f"Invalid argument format to {inst.upper()}")

def assembleNOT(args: tuple[Arg], data: list[int]) -> None:
    # Check args length
    if len(args) != 1: raise TASMError(f"Invalid number of arguments for NOT: {len(args)}")

    data.append(Inst.NOT)

    if args[0].type == ArgType.REG8:
        data.append(0)              # MOD
        data.append(args[0].value)  # Regcode
    elif args[0].type == ArgType.REG16:
        data.append(1)              # MOD
        data.append(args[0].value)  # Regcode
    elif args[0].type == ArgType.REG32:
        data.append(2)              # MOD
        data.append(args[0].value)  # Regcode
    else:
        raise TASMError(f"Invalid argument format to NOT")

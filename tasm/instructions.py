class Inst:
    # Control Instructions
    NOP     = 0x00
    HLT     = 0x01
    SYSCALL = 0x02
    SYSRET  = 0x03
    CALL    = 0x04
    RET     = 0x05
    JMP     = 0x06
    JZ      = 0x07
    JC      = 0x08
    JO      = 0x09
    JS      = 0x0A
    JP      = 0x0B

    # Register & Memory Instructions
    MOV     = 0x10
    LB      = 0x11
    SB      = 0x12
    PUSH    = 0x13
    POP     = 0x14

    # Bitwise & Arithmetic Instructions
    CMP     = 0x21
    AND     = 0x22
    OR      = 0x23
    XOR     = 0x24
    NOT     = 0x25
    ADD     = 0x2A
    SUB     = 0x2B
    MUL     = 0x2C

class AddressMode:
    RELATIVE = 0
    ABSOLUTE = 1

class Arg:
    def __init__(self, type: str, value: any, reg: int=None):
        self.type = type    # The argument's type
        self.value = value  # The argument's value
        self.reg = reg      # The offest register for rel32

class Label:
    def __init__(self, name: str, replace_pos: int, current_ip: int):
        self.name = name
        self.replace_pos = replace_pos
        self.current_ip = current_ip
        self.fname = TASMError.fname
        self.line = TASMError.line

class TASMError(Exception):
    # Static fields
    fname: str = None
    line: int = -1

    def __init__(self, msg: str):
        super().__init__(f"TASMError:\n  From: {TASMError.fname}:{TASMError.line}\n    {msg}")

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

#########################################
##### Instruction assembler methods #####
#########################################

def assembleJMPLike(inst: str, args: tuple[Arg], data: list[int], labels: list[Label]) -> None:
    # Check args length
    if len(args) != 1: raise TASMError(f"Invalid number of arguments for CALL or JMP-like: {len(args)}")

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
    if args[0].type == "Rel32" or args[0].type == "Label":
        cbyte  = 0                              # MOD
        cbyte += 2 if is_inverse_flag else 0    # Differentiate jz vs jnz, for example
        cbyte |= (AddressMode.RELATIVE) << 4    # Addressing mode
        data.append(cbyte)                      # Append control byte

        if args[0].type == "Label":
            data.append(regcode("IP"))  # Append offset register
            replace_pos = len(data)     # Store replacement position for label offset
            data.extend([0, 0, 0, 0])   # Append placeholder offset

            # Store label to be replaced
            labels.append( Label(name=args[0].value, replace_pos=replace_pos, current_ip=len(data)) )
        else:
            data.append(args[0].reg)               # Append offset register
            simm_to_bytes(args[0].value, 32, data) # Append signed offset address
    elif args[0].type == "Address":
        cbyte  = 0                              # MOD
        cbyte += 2 if is_inverse_flag else 0    # Differentiate jz vs jnz, for example
        cbyte |= (AddressMode.ABSOLUTE) << 4    # Addressing mode
        data.append(cbyte)                      # Append control byte
        imm_to_bytes(args[0].value, 32, data)   # Append absolute address
    elif args[0].type == "Reg32":
        cbyte = 1                               # MOD
        cbyte += 2 if is_inverse_flag else 0    # Differentiate jz vs jnz, for example
        data.append(cbyte)                      # Append control byte
        data.append(args[0].value)              # Append regcode

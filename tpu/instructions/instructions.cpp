#include "instructions.hpp"

#include "../defines.hpp"

namespace tpu {

    // Instruction handler methods
    void executeSYSCALL(TPU& tpu, Memory& mem) {
        if (tpu.getMode() != TPUMode::USER)
            throw tpu::InsufficientModeException("Attempted to call syscall from kernel mode.");

        const u32 syscallNumber = tpu.readReg32(RegCode::EAX);

        // Verify syscall number is valid
        if (syscallNumber >= SYSCALL_TABLE_SIZE / 4)
            throw tpu::InvalidSyscallException(std::to_string(static_cast<int>(syscallNumber)) + " is an invalid syscall.");

        // Backup IP after reading instruction
        tpu.setSRP( tpu.getIP() );

        // Determine address of syscall address from syscall table
        const u32 tableAddr = SYSCALL_TABLE_FIRST + 4 * syscallNumber;

        // Move stack ptr to kernel stack
        tpu.saveESPtoKSP(); // Backup SP
        tpu.setESP( KERNEL_STACK_LOWER );

        // Enter kernel mode
        tpu.setMode( TPUMode::KERNEL );

        // Dereference the syscall table address ptr
        tpu.setIP( mem.readDWord(tableAddr).dword );
    }

    void executeSYSRET(TPU& tpu, Memory&) {
        if (tpu.getMode() != TPUMode::KERNEL)
            throw tpu::InsufficientModeException("Attempted to call sysret from non-kernel mode.");

        // Revert to last IP stored in the syscall return ptr (SRP)
        tpu.setIP( tpu.getSRP() );

        // Revert stack ptr
        tpu.restoreESPfromKSP();

        // Drop to user mode
        tpu.setMode( TPUMode::USER );
    }

    void executeCALL(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = IMOD(controlByte);
        const bool isAbsAddrMode = IADDRMODE(controlByte) == ADDR_MODE_ABS;

        if (MOD == 0) {
            if (isAbsAddrMode) { // addr
                const u32 addr = tpu.nextDWord(mem).dword;
                tpu.setRP( tpu.getIP() ); // Backup IP AFTER instruction
                tpu.setIP( addr );
            } else { // rel32
                const RegCode reg = tpu.nextReg(mem);
                const s32 offset = static_cast<s32>( tpu.nextDWord(mem).dword );
                const u32 base = tpu.readReg32( reg ); // Read register AFTER instruction, in case it's IP

                tpu.setRP( tpu.getIP() ); // Backup IP AFTER instruction
                tpu.setIP( base + static_cast<u32>(offset) );
            }
        } else if (MOD == 1) {
            const u32 addr = tpu.readReg32(tpu.nextReg(mem));
            tpu.setRP( tpu.getIP() ); // Backup IP

            // Update IP AFTER backing it up
            tpu.setIP( addr );
        } else {
            throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for CALL.");
        }
    }

    void executeRET(TPU& tpu, Memory&) {
        tpu.setIP( tpu.readReg32(RegCode::RP) );
    }

    void executeJMP(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = IMOD(controlByte);
        const bool isAbsAddrMode = IADDRMODE(controlByte) == ADDR_MODE_ABS;
        switch (MOD) {
            case 0: tpu.setIP( isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem) ); break;
            case 1: tpu.setIP( tpu.readReg32(tpu.nextReg(mem)) ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for JMP.");
        }
    }

    #define executeJMPLike(name, flag) \
        void execute##name(TPU& tpu, Memory& mem) { \
            const u8 controlByte = tpu.nextByte(mem); \
            const bool isAbsAddrMode = IADDRMODE(controlByte) == ADDR_MODE_ABS; \
            switch (IMOD(controlByte)) { \
                case 0: { \
                    const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem); \
                    if (tpu.isFlag(flag)) tpu.setIP( addr ); \
                    break; \
                } \
                case 1: { \
                    const u32 addr = tpu.readReg32(tpu.nextReg(mem)); \
                    if (tpu.isFlag(flag)) tpu.setIP( addr ); \
                    break; \
                } \
                case 2: { \
                    const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem); \
                    if (!tpu.isFlag(flag)) tpu.setIP( addr ); \
                    break; \
                } \
                case 3: { \
                    const u32 addr = tpu.readReg32(tpu.nextReg(mem)); \
                    if (!tpu.isFlag(flag)) tpu.setIP( addr ); \
                    break; \
                } \
                default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(IMOD(controlByte))) + " is invalid for " #name "."); \
            } \
        }

    executeJMPLike(JZ, FLAG_ZERO);
    executeJMPLike(JC, FLAG_CARRY);
    executeJMPLike(JO, FLAG_OVERFLOW);
    executeJMPLike(JS, FLAG_SIGN);
    executeJMPLike(JP, FLAG_PARITY);

    #undef executeJMPLike

    void executeMOV(TPU& tpu, Memory& mem) {
        const u8 MOD = IMOD(tpu.nextByte(mem));
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            /* reg8, imm8 */   case 0: tpu.setReg8( regA, tpu.nextByte(mem) ); break;
            /* reg16, imm16 */ case 1: tpu.setReg16( regA, tpu.nextWord(mem).word ); break;
            /* reg32, imm32 */ case 2: tpu.setReg32( regA, tpu.nextDWord(mem).dword ); break;
            /* reg8, reg8 */   case 3: tpu.setReg8( regA, tpu.readReg8(tpu.nextReg(mem)) ); break;
            /* reg16, reg16 */ case 4: tpu.setReg16( regA, tpu.readReg16(tpu.nextReg(mem)) ); break;
            /* reg32, reg32 */ case 5: tpu.setReg32( regA, tpu.readReg32(tpu.nextReg(mem)) ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for MOV.");
        }
    }

    void executeLB(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = IMOD(controlByte);
        const bool isAbsAddrMode = IADDRMODE(controlByte) == ADDR_MODE_ABS;
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            case 0: {
                const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem);
                tpu.setReg8( regA, mem.readByte(addr) );
                break;
            }
            case 1: tpu.setReg8( regA, mem.readByte(tpu.readReg32(tpu.nextReg(mem))) ); break;
            case 2: {
                const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem);
                tpu.setReg16( regA, mem.readWord(addr).word );
                break;
            }
            case 3: tpu.setReg16( regA, mem.readWord(tpu.readReg32(tpu.nextReg(mem))).word ); break;
            case 4: {
                const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem);
                tpu.setReg32( regA, mem.readDWord(addr).dword );
                break;
            }
            case 5: tpu.setReg32( regA, mem.readDWord(tpu.readReg32(tpu.nextReg(mem))).dword ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for LB/LW/LDW.");
        }
    }

    void executeSB(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = IMOD(controlByte);
        const bool isAbsAddrMode = IADDRMODE(controlByte) == ADDR_MODE_ABS;
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            case 0: {
                const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem);
                mem.setByte( addr, tpu.readReg8(regA) );
                break;
            }
            case 1: mem.setByte( tpu.readReg32(tpu.nextReg(mem)), tpu.readReg8(regA) ); break;
            case 2: {
                const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem);
                mem.setWord( addr, tpu.readReg16(regA) );
                break;
            }
            case 3: mem.setWord( tpu.readReg32(tpu.nextReg(mem)), tpu.readReg16(regA) ); break;
            case 4: {
                const u32 addr = isAbsAddrMode ? tpu.nextDWord(mem).dword : tpu.readRel32(mem);
                mem.setDWord( addr, tpu.readReg32(regA) );
                break;
            }
            case 5: mem.setDWord( tpu.readReg32(tpu.nextReg(mem)), tpu.readReg32(regA) ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for SB/SW/SDW.");
        }
    }

    void executePUSH(TPU& tpu, Memory& mem) {
        const u8 MOD = IMOD(tpu.nextByte(mem));
        switch (MOD) {
            /* reg8 */   case 0: tpu.pushByte( mem, tpu.readReg8( tpu.nextReg(mem) ) ); break;
            /* imm8 */   case 1: tpu.pushByte( mem, tpu.nextByte(mem) ); break;
            /* reg16 */  case 2: tpu.pushWord( mem, tpu.readReg16( tpu.nextReg(mem) ) ); break;
            /* imm16 */  case 3: tpu.pushWord( mem, tpu.nextWord(mem).word ); break;
            /* reg32 */  case 4: tpu.pushDWord( mem, tpu.readReg32( tpu.nextReg(mem) ) ); break;
            /* imm32 */  case 5: tpu.pushDWord( mem, tpu.nextDWord(mem).dword ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for PUSH-like.");
        }
    }

    void executePOP(TPU& tpu, Memory& mem) {
        const u8 MOD = IMOD(tpu.nextByte(mem));
        switch (MOD) {
            // reg8
            case 0: {
                RegCode regA = tpu.nextReg(mem);
                tpu.setReg8( regA, tpu.popByte(mem) );
                break;
            }
            // <no dest, pop byte>
            case 1: tpu.popByte(mem); break;
            // reg16
            case 2: {
                RegCode regA = tpu.nextReg(mem);
                tpu.setReg16( regA, tpu.popWord(mem) );
                break;
            }
            // <no dest, pop word>
            case 3: tpu.popWord(mem); break;
            // reg32
            case 4: {
                RegCode regA = tpu.nextReg(mem);
                tpu.setReg32( regA, tpu.popDWord(mem) );
                break;
            }
            // <no dest, pop dword>
            case 5: tpu.popDWord(mem); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for POP-like.");
        }
    }

    void executeADD(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = IMOD(controlByte);
        const bool isSigned = ISIGN(controlByte) > 0;
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            case 0:   // reg8, imm8
            case 3: { // reg8, reg8
                const u8 a = tpu.readReg8(regA);
                const u8 b = (MOD == 3) ? tpu.readReg8(tpu.nextReg(mem)) : tpu.nextByte(mem);
                aluADD(tpu, a, b, regA, isSigned);
                break;
            }
            case 1:   // reg16, imm16
            case 4: { // reg16, reg16
                const u16 a = tpu.readReg16(regA);
                const u16 b = (MOD == 4) ? tpu.readReg16(tpu.nextReg(mem)) : tpu.nextWord(mem).word;
                aluADD(tpu, a, b, regA, isSigned);
                break;
            }
            case 2:   // reg32, imm32
            case 5: { // reg32, reg32
                const u32 a = tpu.readReg32(regA);
                const u32 b = (MOD == 5) ? tpu.readReg32(tpu.nextReg(mem)) : tpu.nextDWord(mem).dword;
                aluADD(tpu, a, b, regA, isSigned);
                break;
            }
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for ADD.");
        }
    }
    
    void executeSUB(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = IMOD(controlByte);
        const bool isSigned = ISIGN(controlByte) > 0;
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            case 0:   // reg8, imm8
            case 3: { // reg8, reg8
                const u8 a = tpu.readReg8(regA);
                const u8 b = (MOD == 3) ? tpu.readReg8(tpu.nextReg(mem)) : tpu.nextByte(mem);
                aluSUB(tpu, a, b, regA, isSigned);
                break;
            }
            case 1:   // reg16, imm16
            case 4: { // reg16, reg16
                const u16 a = tpu.readReg16(regA);
                const u16 b = (MOD == 4) ? tpu.readReg16(tpu.nextReg(mem)) : tpu.nextWord(mem).word;
                aluSUB(tpu, a, b, regA, isSigned);
                break;
            }
            case 2:   // reg32, imm32
            case 5: { // reg32, reg32
                const u32 a = tpu.readReg32(regA);
                const u32 b = (MOD == 5) ? tpu.readReg32(tpu.nextReg(mem)) : tpu.nextDWord(mem).dword;
                aluSUB(tpu, a, b, regA, isSigned);
                break;
            }
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for SUB.");
        }
    }

    void executeMUL(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = IMOD(controlByte);
        const bool isSigned = ISIGN(controlByte) > 0;
        switch (MOD) {
            case 0:   // imm8
            case 3: { // reg8
                const u8 a = tpu.readReg8(RegCode::AL);
                const u8 b = (MOD == 3) ? tpu.readReg8(tpu.nextReg(mem)) : tpu.nextByte(mem);
                aluMUL(tpu, a, b, isSigned);
                break;
            }
            case 1:   // imm16
            case 4: { // reg16
                const u16 a = tpu.readReg16(RegCode::AX);
                const u16 b = (MOD == 4) ? tpu.readReg16(tpu.nextReg(mem)) : tpu.nextWord(mem).word;
                aluMUL(tpu, a, b, isSigned);
                break;
            }
            case 2:   // imm32
            case 5: { // reg32
                const u32 a = tpu.readReg32(RegCode::EAX);
                const u32 b = (MOD == 5) ? tpu.readReg32(tpu.nextReg(mem)) : tpu.nextDWord(mem).dword;
                aluMUL(tpu, a, b, isSigned);
                break;
            }
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for MUL.");
        }
    }

}

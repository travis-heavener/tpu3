#include "instructions.hpp"

namespace tpu {

    // Instruction handler methods

    void executeCALL(TPU& tpu, Memory& mem) {
        tpu.setIP( tpu.nextDWord(mem).dword );
    }

    void executeRET(TPU& tpu, Memory&) {
        tpu.setIP( tpu.readReg32(RegCode::RP) );
    }

    void executeJMP(TPU& tpu, Memory& mem) {
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
        const u32 addr = tpu.nextDWord(mem).dword;
        switch (MOD) {
            /* jmp */ case 0: tpu.setIP(addr); break;
            /* jz */  case 1: if (tpu.isFlag(FLAG_ZERO)) tpu.setIP(addr); break;
            /* jnz */ case 2: if (!tpu.isFlag(FLAG_ZERO)) tpu.setIP(addr); break;
            /* jc */  case 3: if (tpu.isFlag(FLAG_CARRY)) tpu.setIP(addr); break;
            /* jnc */ case 4: if (!tpu.isFlag(FLAG_CARRY)) tpu.setIP(addr); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for JMP-like.");
        }
    }

    void executeMOV(TPU& tpu, Memory& mem) {
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
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
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
        const RegCode regA = tpu.nextReg(mem);
        const u32 addr = tpu.nextDWord(mem).dword;
        switch (MOD) {
            case 0: tpu.setReg8( regA, mem.readByte(addr) ); break;
            case 1: tpu.setReg16( regA, mem.readWord(addr).word ); break;
            case 2: tpu.setReg32( regA, mem.readDWord(addr).dword ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for LB/LW/LDW.");
        }
    }

    void executeSB(TPU& tpu, Memory& mem) {
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
        const RegCode regA = tpu.nextReg(mem);
        const u32 addr = tpu.nextDWord(mem).dword;
        switch (MOD) {
            case 0: mem.setByte( addr, tpu.readReg8(regA) ); break;
            case 1: mem.setWord( addr, tpu.readReg16(regA) ); break;
            case 2: mem.setDWord( addr, tpu.readReg32(regA) ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for SB/SW/SDW.");
        }
    }

    void executePUSH(TPU& tpu, Memory& mem) {
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
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
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
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

    void executeBUF(TPU& tpu, Memory& mem) {
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
        switch (MOD) {
            // reg8
            case 0: {
                const u8 n = tpu.readReg8( tpu.nextReg(mem) );
                tpu.setFlag(FLAG_CARRY, false);
                tpu.setFlag(FLAG_PARITY, parity<u8>(n));
                tpu.setFlag(FLAG_ZERO, n == 0);
                tpu.setFlag(FLAG_SIGN, (n & 0x80) > 0);
                tpu.setFlag(FLAG_OVERFLOW, false);
                break;
            }
            // reg16
            case 1: {
                const u16 n = tpu.readReg16( tpu.nextReg(mem) );
                tpu.setFlag(FLAG_CARRY, false);
                tpu.setFlag(FLAG_PARITY, parity<u16>(n));
                tpu.setFlag(FLAG_ZERO, n == 0);
                tpu.setFlag(FLAG_SIGN, (n & 0x8000) > 0);
                tpu.setFlag(FLAG_OVERFLOW, false);
                break;
            }
            // reg32
            case 2: {
                const u32 n = tpu.readReg32( tpu.nextReg(mem) );
                tpu.setFlag(FLAG_CARRY, false);
                tpu.setFlag(FLAG_PARITY, parity<u32>(n));
                tpu.setFlag(FLAG_ZERO, n == 0);
                tpu.setFlag(FLAG_SIGN, (n & 0x8000'0000) > 0);
                tpu.setFlag(FLAG_OVERFLOW, false);
                break;
            }
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for BUF.");
        }
    }

    void executeCMP(TPU& tpu, Memory& mem) {
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            case 0: // reg8, imm8
            case 3: { // reg8, reg8
                const u8 valA = tpu.readReg8(regA);
                const u8 valB = (MOD == 0) ? tpu.nextByte(mem) : tpu.readReg8(tpu.nextReg(mem));
                const u8 result = valA - valB;
                const bool signA = (valA & 0x80) != 0, signB = (valB & 0x80) != 0, resSign = (result & 0x80) != 0;
                tpu.setFlag(FLAG_CARRY, valB > valA);
                tpu.setFlag(FLAG_PARITY, parity<u8>(result));
                tpu.setFlag(FLAG_ZERO, result == 0);
                tpu.setFlag(FLAG_SIGN, resSign);
                tpu.setFlag(FLAG_OVERFLOW, (signA != signB) && (signA != resSign));
                break;
            }
            case 1: // reg16, imm16
            case 4: { // reg16, reg16
                const u16 valA = tpu.readReg16(regA);
                const u16 valB = (MOD == 1) ? tpu.nextWord(mem).word : tpu.readReg16(tpu.nextReg(mem));
                const u16 result = valA - valB;
                const bool signA = (valA & 0x8000) != 0, signB = (valB & 0x8000) != 0, resSign = (result & 0x8000) != 0;
                tpu.setFlag(FLAG_CARRY, valB > valA);
                tpu.setFlag(FLAG_PARITY, parity<u16>(result));
                tpu.setFlag(FLAG_ZERO, result == 0);
                tpu.setFlag(FLAG_SIGN, resSign);
                tpu.setFlag(FLAG_OVERFLOW, (signA != signB) && (signA != resSign));
                break;
            }
            case 2: // reg32, imm32
            case 5: { // reg32, reg32
                const u32 valA = tpu.readReg32(regA);
                const u32 valB = (MOD == 2) ? tpu.nextDWord(mem).dword : tpu.readReg32(tpu.nextReg(mem));
                const u32 result = valA - valB;
                const bool signA = (valA & 0x8000'0000) != 0, signB = (valB & 0x8000'0000) != 0, resSign = (result & 0x8000'0000) != 0;
                tpu.setFlag(FLAG_CARRY, valB > valA);
                tpu.setFlag(FLAG_PARITY, parity<u16>(result));
                tpu.setFlag(FLAG_ZERO, result == 0);
                tpu.setFlag(FLAG_SIGN, resSign);
                tpu.setFlag(FLAG_OVERFLOW, (signA != signB) && (signA != resSign));
                break;
            }
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for CMP.");
        }
    }

    // Macrodef for frequently used structure
    // BITWISE operations ONLY, works similarly to arithmetic
    // but does NOT set CARRY or OVERFLOW flags
    #define BITWISE_EXECUTE_OP(name, op) \
        void execute##name(TPU& tpu, Memory& mem) { \
            const u8 MOD = 0b111 & tpu.nextByte(mem); /* Read MOD byte */ \
            const RegCode regA = tpu.nextReg(mem); \
            switch (MOD) { \
                /* reg8, imm8 */ \
                case 0: { \
                    const u8 n = tpu.readReg8(regA) op tpu.nextByte(mem); \
                    tpu.setReg8(regA, static_cast<u8>(n)); \
                    tpu.setFlag(FLAG_CARRY, false); \
                    tpu.setFlag(FLAG_PARITY, parity<u8>(n)); \
                    tpu.setFlag(FLAG_ZERO, n == 0); \
                    tpu.setFlag(FLAG_SIGN, (n & 0x80) > 0); \
                    tpu.setFlag(FLAG_OVERFLOW, false); \
                    break; \
                } \
                /* reg16, imm16 */ \
                case 1: { \
                    const u16 n = tpu.readReg16(regA) op tpu.nextWord(mem).word; \
                    tpu.setReg16(regA, static_cast<u16>(n)); \
                    tpu.setFlag(FLAG_CARRY, false); \
                    tpu.setFlag(FLAG_PARITY, parity<u16>(n)); \
                    tpu.setFlag(FLAG_ZERO, n == 0); \
                    tpu.setFlag(FLAG_SIGN, (n & 0x8000) > 0); \
                    tpu.setFlag(FLAG_OVERFLOW, false); \
                    break; \
                } \
                /* reg32, imm32 */ \
                case 2: { \
                    const u32 n = tpu.readReg32(regA) op tpu.nextDWord(mem).dword; \
                    tpu.setReg32(regA, static_cast<u32>(n)); \
                    tpu.setFlag(FLAG_CARRY, false); \
                    tpu.setFlag(FLAG_PARITY, parity<u32>(n)); \
                    tpu.setFlag(FLAG_ZERO, n == 0); \
                    tpu.setFlag(FLAG_SIGN, (n & 0x8000'0000) > 0); \
                    tpu.setFlag(FLAG_OVERFLOW, false); \
                    break; \
                } \
                /* reg8, reg8 */ \
                case 3: { \
                    const u8 n = tpu.readReg8(regA) op tpu.readReg8(tpu.nextReg(mem)); \
                    tpu.setReg8(regA, static_cast<u8>(n)); \
                    tpu.setFlag(FLAG_CARRY, false); \
                    tpu.setFlag(FLAG_PARITY, parity<u8>(n)); \
                    tpu.setFlag(FLAG_ZERO, n == 0); \
                    tpu.setFlag(FLAG_SIGN, (n & 0x80) > 0); \
                    tpu.setFlag(FLAG_OVERFLOW, false); \
                    break; \
                } \
                /* reg16, reg16 */ \
                case 4: { \
                    const u16 n = tpu.readReg16(regA) op tpu.readReg16(tpu.nextReg(mem)); \
                    tpu.setReg16(regA, static_cast<u16>(n)); \
                    tpu.setFlag(FLAG_CARRY, false); \
                    tpu.setFlag(FLAG_PARITY, parity<u16>(n)); \
                    tpu.setFlag(FLAG_ZERO, n == 0); \
                    tpu.setFlag(FLAG_SIGN, (n & 0x8000) > 0); \
                    tpu.setFlag(FLAG_OVERFLOW, false); \
                    break; \
                } \
                /* reg32, reg32 */ \
                case 5: { \
                    const u32 n = tpu.readReg32(regA) op tpu.readReg32(tpu.nextReg(mem)); \
                    tpu.setReg32(regA, static_cast<u32>(n)); \
                    tpu.setFlag(FLAG_CARRY, false); \
                    tpu.setFlag(FLAG_PARITY, parity<u32>(n)); \
                    tpu.setFlag(FLAG_ZERO, n == 0); \
                    tpu.setFlag(FLAG_SIGN, (n & 0x8000'0000) > 0); \
                    tpu.setFlag(FLAG_OVERFLOW, false); \
                    break; \
                } \
                default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for " #name "."); \
            } \
        }

    BITWISE_EXECUTE_OP(AND, &);
    BITWISE_EXECUTE_OP(OR, |);
    BITWISE_EXECUTE_OP(XOR, ^);

    #undef BITWISE_EXECUTE_OP

    void executeNOT(TPU& tpu, Memory& mem) {
        const u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            // reg8
            case 0: tpu.setReg8( regA, static_cast<u8>(~tpu.readReg8(regA)) ); break;
            // reg16
            case 1: tpu.setReg16( regA, static_cast<u16>(~tpu.readReg16(regA)) ); break;
            // reg32
            case 2: tpu.setReg32( regA, static_cast<u32>(~tpu.readReg32(regA)) ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for NOT.");
        }
    }

}

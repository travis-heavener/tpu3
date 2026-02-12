#include "instructions.hpp"

#include "arithmetic.hpp"
#include "../tools.hpp"

namespace tpu {

    // Instruction handler methods
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
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = 0b111 & controlByte; // Read MOD bits
        const bool isSigned = 0b1000 & controlByte;
        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            case 0:   // reg8, imm8
            case 3: { // reg8, reg8
                const u8 a = tpu.readReg8(regA);
                const u8 b = (MOD == 3) ? tpu.readReg8(tpu.nextReg(mem)) : tpu.nextByte(mem);
                aluCMP(tpu, a, b, isSigned);
                break;
            }
            case 1:   // reg16, imm16
            case 4: { // reg16, reg16
                const u16 a = tpu.readReg16(regA);
                const u16 b = (MOD == 4) ? tpu.readReg16(tpu.nextReg(mem)) : tpu.nextWord(mem).word;
                aluCMP(tpu, a, b, isSigned);
                break;
            }
            case 2:   // reg32, imm32
            case 5: { // reg32, reg32
                const u32 a = tpu.readReg32(regA);
                const u32 b = (MOD == 5) ? tpu.readReg32(tpu.nextReg(mem)) : tpu.nextDWord(mem).dword;
                aluCMP(tpu, a, b, isSigned);
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
            /* reg8 */  case 0: tpu.setReg8( regA, static_cast<u8>(~tpu.readReg8(regA)) ); break;
            /* reg16 */ case 1: tpu.setReg16( regA, static_cast<u16>(~tpu.readReg16(regA)) ); break;
            /* reg32 */ case 2: tpu.setReg32( regA, static_cast<u32>(~tpu.readReg32(regA)) ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for NOT.");
        }
    }

}

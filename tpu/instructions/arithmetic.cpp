#include "arithmetic.hpp"

#include "../tools.hpp"

#define IS_SIGN_SET_8(arg) ((arg & 0x80) != 0)
#define IS_SIGN_SET_16(arg) ((arg & 0x8000) != 0)
#define IS_SIGN_SET_32(arg) ((arg & 0x8000'0000) != 0)

namespace tpu {

    /********************* ARITHMETIC INSTRUCTIONS *********************/

    void executeADD(TPU& tpu, Memory& mem) {
        const u8 controlByte = tpu.nextByte(mem);
        const u8 MOD = 0b111 & controlByte; // Read MOD bits
        const bool isSigned = 0b1000 & controlByte;

        const RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            case 0:   // reg8, imm8
            case 3: { // reg8, reg8
                const u8 argA = tpu.readReg8(regA);
                const u8 argB = (MOD == 3) ? tpu.readReg8(tpu.nextReg(mem)) : tpu.nextByte(mem);
                if (!isSigned) { // Unsigned
                    // Calculate result
                    const u16 fullSum = static_cast<u16>(argA) + static_cast<u16>(argB);
                    const u8 result = static_cast<u8>(fullSum & 0xFF); // Bit-mask result
                    tpu.setReg8(regA, result);

                    // Flags
                    tpu.setFlag(FLAG_CARRY, fullSum > 0xFF);
                    tpu.setFlag(FLAG_PARITY, parity<u8>(result));
                    tpu.setFlag(FLAG_ZERO, result == 0);
                    tpu.setFlag(FLAG_SIGN, (result & 0x80) > 0);
                    // Overflow flag irrelevant for unsigned arithmetic
                } else { // Signed
                    // Calculate & cast result
                    const s8 sargA = static_cast<s8>(argA), sargB = static_cast<s8>(argB);
                    const s16 fullSum = static_cast<s16>(sargA) + static_cast<s16>(sargB);
                    const s8 result = static_cast<s8>(fullSum & 0xFF); // Bit-mask result
                    tpu.setReg8(regA, static_cast<u8>(result));

                    // Flags
                    const bool signA = IS_SIGN_SET_8(sargA), signB = IS_SIGN_SET_8(sargB), resSign = IS_SIGN_SET_8(result);
                    // Carry flag irrelevant for signed arithmetic
                    tpu.setFlag(FLAG_PARITY, parity<u8>(static_cast<u8>(result)));
                    tpu.setFlag(FLAG_ZERO, result == 0);
                    tpu.setFlag(FLAG_SIGN, (result & 0x80) > 0);
                    tpu.setFlag(FLAG_OVERFLOW, (signA == signB) && (signA != resSign));
                }
                break;
            }
            case 1:   // reg16, imm16
            case 4: { // reg16, reg16
                const u16 argA = tpu.readReg16(regA);
                const u16 argB = (MOD == 4) ? tpu.readReg16(tpu.nextReg(mem)) : tpu.nextWord(mem).word;
                if (!isSigned) { // Unsigned
                    // Calculate result
                    const u32 fullSum = static_cast<u32>(argA) + static_cast<u32>(argB);
                    const u16 result = static_cast<u16>(fullSum & 0xFFFF); // Bit-mask result
                    tpu.setReg16(regA, result);

                    // Flags
                    tpu.setFlag(FLAG_CARRY, fullSum > 0xFFFF);
                    tpu.setFlag(FLAG_PARITY, parity<u16>(result));
                    tpu.setFlag(FLAG_ZERO, result == 0);
                    tpu.setFlag(FLAG_SIGN, (result & 0x8000) > 0);
                    // Overflow flag irrelevant for unsigned arithmetic
                } else { // Signed
                    // Calculate & cast result
                    const s16 sargA = static_cast<s16>(argA), sargB = static_cast<s16>(argB);
                    const s32 fullSum = static_cast<s32>(sargA) + static_cast<s32>(sargB);
                    const s16 result = static_cast<s16>(fullSum & 0xFFFF); // Bit-mask result
                    tpu.setReg16(regA, static_cast<u16>(result));

                    // Flags
                    const bool signA = IS_SIGN_SET_16(sargA), signB = IS_SIGN_SET_16(sargB), resSign = IS_SIGN_SET_16(result);
                    // Carry flag irrelevant for signed arithmetic
                    tpu.setFlag(FLAG_PARITY, parity<u16>(static_cast<u16>(result)));
                    tpu.setFlag(FLAG_ZERO, result == 0);
                    tpu.setFlag(FLAG_SIGN, (result & 0x8000) > 0);
                    tpu.setFlag(FLAG_OVERFLOW, (signA == signB) && (signA != resSign));
                }
                break;
            }
            case 2:   // reg32, imm32
            case 5: { // reg32, reg32
                const u32 argA = tpu.readReg32(regA);
                const u32 argB = (MOD == 5) ? tpu.readReg32(tpu.nextReg(mem)) : tpu.nextDWord(mem).dword;
                if (!isSigned) { // Unsigned
                    // Calculate result
                    const u64 fullSum = static_cast<u64>(argA) + static_cast<u64>(argB);
                    const u32 result = static_cast<u32>(fullSum & 0xFFFF'FFFF); // Bit-mask result
                    tpu.setReg32(regA, result);

                    // Flags
                    tpu.setFlag(FLAG_CARRY, fullSum > 0xFFFF'FFFF);
                    tpu.setFlag(FLAG_PARITY, parity<u32>(result));
                    tpu.setFlag(FLAG_ZERO, result == 0);
                    tpu.setFlag(FLAG_SIGN, (result & 0x8000'0000) > 0);
                    // Overflow flag irrelevant for unsigned arithmetic
                } else { // Signed
                    // Calculate & cast result
                    const s32 sargA = static_cast<s32>(argA), sargB = static_cast<s32>(argB);
                    const s64 fullSum = static_cast<s64>(sargA) + static_cast<s64>(sargB);
                    const s32 result = static_cast<s32>(fullSum & 0xFFFF'FFFF); // Bit-mask result
                    tpu.setReg32(regA, static_cast<u32>(result));

                    // Flags
                    const bool signA = IS_SIGN_SET_32(sargA), signB = IS_SIGN_SET_32(sargB), resSign = IS_SIGN_SET_32(result);
                    // Carry flag irrelevant for signed arithmetic
                    tpu.setFlag(FLAG_PARITY, parity<u32>(static_cast<u32>(result)));
                    tpu.setFlag(FLAG_ZERO, result == 0);
                    tpu.setFlag(FLAG_SIGN, (result & 0x8000'0000) > 0);
                    tpu.setFlag(FLAG_OVERFLOW, (signA == signB) && (signA != resSign));
                }
                break;
            }
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for ADD.");
        }
    }

}

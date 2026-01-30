#include "inst.hpp"

#define regcast static_cast<RegCode>

namespace tpu {

    // Instruction handler methods
    void executeMOV(TPU& tpu, Memory& mem) {
        u8 MOD = 0b111 & tpu.nextByte(mem); // Read MOD byte
        RegCode regA = tpu.nextReg(mem);
        switch (MOD) {
            // reg8, imm8
            case 0: tpu.setReg8( regA, tpu.nextByte(mem) ); break;
            // reg16, imm16
            case 1: tpu.setReg16( regA, tpu.nextWord(mem).word ); break;
            // reg32, imm32
            case 2: tpu.setReg32( regA, tpu.nextDWord(mem).dword ); break;
            // reg8, reg8
            case 3: tpu.setReg8( regA, tpu.readReg8(tpu.nextReg(mem)) ); break;
            // reg16, reg16
            case 4: tpu.setReg16( regA, tpu.readReg16(tpu.nextReg(mem)) ); break;
            // reg32, reg32
            case 5: tpu.setReg32( regA, tpu.readReg32(tpu.nextReg(mem)) ); break;
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for MOV.");
        }
    }

}

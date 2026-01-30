#include "inst.hpp"

namespace tpu {

    // Instruction handler methods
    void executeMOV(TPU& tpu, Memory& mem) {
        u8 MOD = 0b111 & tpu.readNextByte(mem); // Read MOD byte
        switch (MOD) {
            case 0: break; // reg8, imm8
            case 1: break; // reg16, imm16
            case 2: break; // reg32, imm32
            case 3: break; // reg8, reg8
            case 4: break; // reg16, reg16
            case 5: break; // reg32, reg32
            default: throw tpu::InvalidMODBitsException(std::to_string(static_cast<int>(MOD)) + " is invalid for MOV.");
        }
    }

}

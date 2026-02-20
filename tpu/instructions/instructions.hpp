#ifndef __TPU_INSTRUCTIONS_INSTRUCTIONS_HPP
#define __TPU_INSTRUCTIONS_INSTRUCTIONS_HPP

#include "../tools.hpp"
#include "../tpu.hpp"

#include "arithmetic.hpp"
#include "bitwise.hpp"

namespace tpu {

    enum class inst : u8 {
        // Control Instructions
        NOP     = 0x00,
        HLT     = 0x01,
        SYSCALL = 0x02,
        SYSRET  = 0x03,
        CALL    = 0x04,
        RET     = 0x05,
        JMP     = 0x06,
        JZ      = 0x07,
        JC      = 0x08,
        JO      = 0x09,
        JS      = 0x0A,
        JP      = 0x0B,

        // Register & Memory Instructions
        MOV     = 0x10,
        LB      = 0x11,
        SB      = 0x12,
        PUSH    = 0x13,
        POP     = 0x14,

        // Bitwise & Arithmetic Instructions
        CMP     = 0x21,
        AND     = 0x22,
        OR      = 0x23,
        XOR     = 0x24,
        NOT     = 0x25,
        ADD     = 0x2A,
        SUB     = 0x2B,
        MUL     = 0x2C
    };

    // Control Instructions
    void executeSYSCALL(TPU&, Memory&);
    void executeSYSRET(TPU&, Memory&);
    void executeCALL(TPU&, Memory&);
    void executeRET(TPU&, Memory&);
    void executeJMP(TPU&, Memory&);
    void executeJZ(TPU&, Memory&);
    void executeJC(TPU&, Memory&);
    void executeJO(TPU&, Memory&);
    void executeJS(TPU&, Memory&);
    void executeJP(TPU&, Memory&);

    // Register & Memory Instructions
    void executeMOV(TPU&, Memory&);
    void executeLB(TPU&, Memory&);
    void executeSB(TPU&, Memory&);
    void executePUSH(TPU&, Memory&);
    void executePOP(TPU&, Memory&);

    // Bitwise & Arithmetic Instructions
    // See arithmetic.hpp
    void executeADD(TPU&, Memory&);
    void executeSUB(TPU&, Memory&);
    void executeMUL(TPU&, Memory&);

}

#endif

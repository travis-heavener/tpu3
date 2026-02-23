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
        SYSCALL = 0x01,
        SYSRET  = 0x02,
        CALL    = 0x03,
        RET     = 0x04,
        JMP     = 0x05,
        JZ      = 0x06,
        JC      = 0x07,
        JO      = 0x08,
        JS      = 0x09,
        JP      = 0x0A,

        // Kernel protected instructions
        HLT     = 0x15,
        URET    = 0x16,
        SETSYSCALL = 0x17,

        // Register & Memory Instructions
        MOV     = 0x30,
        LB      = 0x31,
        SB      = 0x32,
        PUSH    = 0x33,
        POP     = 0x34,

        // Bitwise & Arithmetic Instructions
        CMP     = 0x61,
        AND     = 0x62,
        OR      = 0x63,
        XOR     = 0x64,
        NOT     = 0x65,
        ADD     = 0x6A,
        SUB     = 0x6B,
        MUL     = 0x6C
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

    // Kernel Protected Instructions
    void executeHLT(TPU&, Memory&);
    void executeURET(TPU&, Memory&);
    void executeSETSYSCALL(TPU&, Memory&);

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

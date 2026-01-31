#ifndef __TPU_INST_HPP
#define __TPU_INST_HPP

#include "tools.hpp"
#include "tpu.hpp"

namespace tpu {

    enum class inst : u8 {
        NOP     = 0x00,
        HLT     = 0x01,
        SYSCALL = 0x02,
        SYSRET  = 0x03,
        CALL    = 0x04,
        RET     = 0x05,
        JMP     = 0x06,
        MOV     = 0x07,
        LB      = 0x08,
        SB      = 0x09,
        PUSH    = 0x0A,
        POP     = 0x0B,
        BUF     = 0x20,
        CMP     = 0x21,
        AND     = 0x22,
        OR      = 0x23,
        XOR     = 0x24,
        NOT     = 0x25
    };

    // Instruction handler methods
    void executeCALL(TPU&, Memory&);
    void executeRET(TPU&, Memory&);
    void executeJMP(TPU&, Memory&);
    void executeMOV(TPU&, Memory&);
    void executeLB(TPU&, Memory&);
    void executeSB(TPU&, Memory&);
    void executePUSH(TPU&, Memory&);
    void executePOP(TPU&, Memory&);
    void executeBUF(TPU&, Memory&);
    void executeCMP(TPU&, Memory&);
    void executeAND(TPU&, Memory&);
    void executeOR(TPU&, Memory&);
    void executeXOR(TPU&, Memory&);
    void executeNOT(TPU&, Memory&);

}

#endif


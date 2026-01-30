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
        POP     = 0x0B
    };

    // Instruction handler methods
    void executeMOV(TPU&, Memory&);

}

#endif


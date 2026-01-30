#ifndef __TPU_INST_HPP
#define __TPU_INST_HPP

#include "tools.hpp"
#include "tpu.hpp"

namespace tpu {

    enum class inst : u8 {
        HLT = 0x0
    };

    // Instruction handler methods
    void executeMOV(TPU&, Memory&);

}

#endif


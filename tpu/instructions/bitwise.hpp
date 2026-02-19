#ifndef __TPU_INSTRUCTIONS_BITWISE_HPP
#define __TPU_INSTRUCTIONS_BITWISE_HPP

#include "../tpu.hpp"

namespace tpu {

    // Bitwise instruction handler methods
    void executeCMP(TPU&, Memory&);
    void executeAND(TPU&, Memory&);
    void executeOR(TPU&, Memory&);
    void executeXOR(TPU&, Memory&);
    void executeNOT(TPU&, Memory&);

}

#endif

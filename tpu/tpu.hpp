#ifndef __TPU_TPU_HPP
#define __TPU_TPU_HPP

#include "memory.hpp"

namespace tpu {
    
    class TPU {
        public:
            TPU();
            ~TPU();

            // Starts the clock, runs until hlt instruction
            void start(Memory& mem);
        private:
            /* TODO - implement flags as a wide register */
    };

}

#endif


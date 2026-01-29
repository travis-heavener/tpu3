#ifndef __TPU_TPU_HPP
#define __TPU_TPU_HPP

#include "memory.hpp"

namespace tpu {

    // Reimplement registers as words
    typedef union reg32 {
        u32 dword;
        struct {
            u16 lword;
            u16 hword;
        };
        struct {
            u8 lbyte;
            u8 hbyte;
            u16 _; // <-- Unused, only need lower bytes
        };
    } reg32;

    typedef union reg16 {
        u16 word;
        struct {
            u8 lbyte;
            u8 hbyte;
        };
    } reg16;

    class TPU {
        public:
            TPU();
            ~TPU();

            // Starts the clock, runs until hlt instruction
            void start(Memory& mem);

            // Debug dumps all registers to stdout
            void dumpRegs() const;
        private:
            Byte readNextByte(Memory& mem);
            Word readNextWord(Memory& mem);
            DWord readNextDWord(Memory& mem);

            reg32 EAX; // Extended accumulator
            reg32 EBX; // Extended base
            reg32 ECX; // Extended counter
            reg32 EDX; // Extended data

            // Address ptrs
            reg32 IP; // Inst ptr
            reg32 SP; // Stack ptr
            reg32 BP; // Base ptr
            reg32 SI; // Source index
            reg32 DI; // Dest index

            // Processor flags
            reg16 FLAGS;
    };

}

#endif


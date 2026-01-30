#ifndef __TPU_TPU_HPP
#define __TPU_TPU_HPP

#include "memory.hpp"

namespace tpu {

    // Reimplement registers as words
    typedef u8 reg8;
    
    typedef union reg16 {
        u16 word;
        struct {
            reg8 lreg8;
            reg8 hreg8;
        };
        struct {
            u8 lbyte;
            u8 hbyte;
        };
    } reg16;
    
    typedef union reg32 {
        u32 dword;
        struct {
            reg16 lreg16;
            reg16 hreg16;
        };
        struct {
            u16 lword;
            u16 hword;
        };
    } reg32;

    class TPU {
        public:
            TPU();
            ~TPU();

            // Starts the clock, runs until hlt instruction
            void start(Memory& mem);

            // Executes the next instruction
            void execute(Memory& mem);

            Byte nextByte(Memory& mem);
            Word nextWord(Memory& mem);
            DWord nextDWord(Memory& mem);

            // Shorthand for getting the next register in memory
            RegCode nextReg(Memory& mem) { return static_cast<RegCode>(nextByte(mem)); };

            // Register getters/setters
            void setReg8(const RegCode, const u8);
            void setReg16(const RegCode, const u16);
            void setReg32(const RegCode, const u32);

            u8 readReg8(const RegCode) const;
            u16 readReg16(const RegCode) const;
            u32 readReg32(const RegCode) const;

            // Debug dumps all registers to stdout
            void dumpRegs() const;
        private:
            reg32 EAX; // Extended accumulator
            reg32 EBX; // Extended base
            reg32 ECX; // Extended counter
            reg32 EDX; // Extended data

            // Address ptrs
            reg32 IP; // Inst ptr
            reg32 ESP; // Stack ptr
            reg32 EBP; // Base ptr
            reg32 ESI; // Source index
            reg32 EDI; // Dest index

            // Processor flags
            reg16 FLAGS;
            bool isInKernelMode; // <-- Single bit, set if in kernel mode, cleared if in user mode
    };

}

#endif

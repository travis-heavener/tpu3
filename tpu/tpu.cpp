#include "tpu.hpp"

#include <cstdio>

namespace tpu {
    
    TPU::TPU() {
        // Reset flags
        EAX = EBX = ECX = EDX = {0};
        IP = SP = BP = SI = DI = {0};

        FLAGS = {0};
    }

    TPU::~TPU() {
        // Clean up
    }

    void TPU::start(Memory&) {
        // Starts executing from memory
    }

    void TPU::dumpRegs() const {
        // 32-bit regs
        std::printf("EAX: 0x%08x\n", EAX.dword);
        std::printf("EBX: 0x%08x\n", EBX.dword);
        std::printf("ECX: 0x%08x\n", ECX.dword);
        std::printf("EDX: 0x%08x\n", EDX.dword);

        std::printf("IP: 0x%08x\n", IP.dword);
        std::printf("SP: 0x%08x\n", SP.dword);
        std::printf("BP: 0x%08x\n", BP.dword);
        std::printf("SI: 0x%08x\n", SI.dword);
        std::printf("DI: 0x%08x\n", DI.dword);

        // 16-bit regs
        std::printf("FLAGS: 0x%04x\n", FLAGS.word);
    }

}


#include <cstdio>
#include <stdexcept>
#include <string>

#include "tpu.hpp"
#include "inst.hpp"

namespace tpu {
    
    TPU::TPU() {
        // Reset flags
        EAX = EBX = ECX = EDX = {0};
        IP = ESP = EBP = ESI = EDI = {0};

        FLAGS = {0};
        isInKernelMode = false;
    }

    TPU::~TPU() {
        // Clean up
    }

    // Starts the clock
    void TPU::start(Memory& mem) {
        // Move IP to first instruction
        this->IP.dword = mem.dword(0).dword;

        // Begin execution
        this->execute(mem);
    }

    // Reads the next instruction at IP
    void TPU::execute(Memory& mem) {
        while (true) {
            // Read next instruction byte
            inst instruction = static_cast<inst>( this->nextByte(mem) );

            // Switch based on instruction
            switch (instruction) {
                case inst::HLT: return;
                case inst::MOV: executeMOV(*this, mem); break;
                default:
                    throw tpu::InvalidInstructionException( std::to_string(static_cast<u8>(instruction)) );
            }

            // TODO - sleep between cycles
        }
    }

    // Reads the next byte at the IP
    Byte TPU::nextByte(Memory& mem) {
        return mem.byte(this->IP.dword++);
    }

    // Reads the next word at the IP
    Word TPU::nextWord(Memory& mem) {
        u32 addr = this->IP.dword;
        this->IP.dword += 2;
        return mem.word(addr);
    }

    // Reads the next dword at the IP
    DWord TPU::nextDWord(Memory& mem) {
        u32 addr = this->IP.dword;
        this->IP.dword += 4;
        return mem.dword(addr);
    }

    // Register getters/setters
    void TPU::setReg8(const RegCode rc, const u8 v) {
        switch (rc) {
            case RegCode::AL: this->EAX.lreg16.lbyte = v; break;
            case RegCode::AH: this->EAX.lreg16.hbyte = v; break;
            case RegCode::BL: this->EBX.lreg16.lbyte = v; break;
            case RegCode::BH: this->EBX.lreg16.hbyte = v; break;
            case RegCode::CL: this->ECX.lreg16.lbyte = v; break;
            case RegCode::CH: this->ECX.lreg16.hbyte = v; break;
            case RegCode::DL: this->EDX.lreg16.lbyte = v; break;
            case RegCode::DH: this->EDX.lreg16.hbyte = v; break;
            default:
                throw InvalidRegCodeException("setReg8: " + std::to_string(static_cast<int>(rc)));
        }
    }
    
    void TPU::setReg16(const RegCode rc, const u16 v) {
        switch (rc) {
            case RegCode::AX: this->EAX.lword = v; break;
            case RegCode::BX: this->EBX.lword = v; break;
            case RegCode::CX: this->ECX.lword = v; break;
            case RegCode::DX: this->EDX.lword = v; break;
            case RegCode::SP: this->ESP.lword = v; break;
            case RegCode::BP: this->EBP.lword = v; break;
            case RegCode::SI: this->ESI.lword = v; break;
            case RegCode::DI: this->EDI.lword = v; break;
            default:
                throw InvalidRegCodeException("setReg16: " + std::to_string(static_cast<int>(rc)));
        }
    }

    void TPU::setReg32(const RegCode rc, const u32 v) {
        switch (rc) {
            case RegCode::EAX: this->EAX.dword = v; break;
            case RegCode::EBX: this->EBX.dword = v; break;
            case RegCode::ECX: this->ECX.dword = v; break;
            case RegCode::EDX: this->EDX.dword = v; break;
            case RegCode::ESP: this->ESP.dword = v; break;
            case RegCode::EBP: this->EBP.dword = v; break;
            case RegCode::ESI: this->ESI.dword = v; break;
            case RegCode::EDI: this->EDI.dword = v; break;
            case RegCode::IP:  this->IP.dword = v;  break;
            default:
                throw InvalidRegCodeException("setReg32: " + std::to_string(static_cast<int>(rc)));
        }
    }

    u8 TPU::readReg8(const RegCode rc) const {
        switch (rc) {
            case RegCode::AL: return this->EAX.lreg16.lbyte;
            case RegCode::AH: return this->EAX.lreg16.hbyte;
            case RegCode::BL: return this->EBX.lreg16.lbyte;
            case RegCode::BH: return this->EBX.lreg16.hbyte;
            case RegCode::CL: return this->ECX.lreg16.lbyte;
            case RegCode::CH: return this->ECX.lreg16.hbyte;
            case RegCode::DL: return this->EDX.lreg16.lbyte;
            case RegCode::DH: return this->EDX.lreg16.hbyte;
            default:
                throw InvalidRegCodeException("readReg8: " + std::to_string(static_cast<int>(rc)));
        }
    }

    u16 TPU::readReg16(const RegCode rc) const {
        switch (rc) {
            case RegCode::AX: return this->EAX.lword;
            case RegCode::BX: return this->EBX.lword;
            case RegCode::CX: return this->ECX.lword;
            case RegCode::DX: return this->EDX.lword;
            case RegCode::SP: return this->ESP.lword;
            case RegCode::BP: return this->EBP.lword;
            case RegCode::SI: return this->ESI.lword;
            case RegCode::DI: return this->EDI.lword;
            default:
                throw InvalidRegCodeException("readReg16: " + std::to_string(static_cast<int>(rc)));
        }
    }

    u32 TPU::readReg32(const RegCode rc) const {
        switch (rc) {
            case RegCode::EAX: return this->EAX.dword;
            case RegCode::EBX: return this->EBX.dword;
            case RegCode::ECX: return this->ECX.dword;
            case RegCode::EDX: return this->EDX.dword;
            case RegCode::ESP: return this->ESP.dword;
            case RegCode::EBP: return this->EBP.dword;
            case RegCode::ESI: return this->ESI.dword;
            case RegCode::EDI: return this->EDI.dword;
            case RegCode::IP:  return this->IP.dword;
            default:
                throw InvalidRegCodeException("setReg32: " + std::to_string(static_cast<int>(rc)));
        }
    }

    // Debug dump all registers to stdout
    void TPU::dumpRegs() const {
        // 32-bit regs
        std::printf("EAX: 0x%08x\n", EAX.dword);
        std::printf("EBX: 0x%08x\n", EBX.dword);
        std::printf("ECX: 0x%08x\n", ECX.dword);
        std::printf("EDX: 0x%08x\n", EDX.dword);

        std::printf("IP:  0x%08x\n", IP.dword);
        std::printf("SP:  0x%08x\n", ESP.dword);
        std::printf("BP:  0x%08x\n", EBP.dword);
        std::printf("SI:  0x%08x\n", ESI.dword);
        std::printf("DI:  0x%08x\n", EDI.dword);

        // 16-bit regs
        std::printf("FLAGS: 0b%016b\n", FLAGS.word);

        // 8-bit regs/flags
        std::printf("MODE: %d\n", (isInKernelMode ? 1 : 0));
    }

}


#include <cstdio>
#include <stdexcept>
#include <string>

#include "tpu.hpp"
#include "inst.hpp"

namespace tpu {
    
    TPU::TPU() {
        // Reset flags
        EAX = EBX = ECX = EDX = {0};
        IP = RP = ESP = EBP = ESI = EDI = {0};

        FLAGS = {0};
        isInKernelMode = false;
    }

    TPU::~TPU() {
        // Clean up
    }

    // Starts the clock
    void TPU::start(Memory& mem, std::atomic<bool>& isExiting) {
        // Move IP to first instruction
        this->IP.dword = mem.readDWord(0).dword;

        // Begin execution
        this->execute(mem, isExiting);
    }

    // Reads the next instruction at IP
    void TPU::execute(Memory& mem, std::atomic<bool>& isExiting) {
        while (!isExiting) {
            // Read next instruction byte
            inst instruction = static_cast<inst>( this->nextByte(mem) );

            // Switch based on instruction
            switch (instruction) {
                case inst::HLT: return;
                case inst::CALL:    executeCALL(*this, mem); break;
                case inst::RET:     executeRET(*this, mem); break;
                case inst::JMP:     executeJMP(*this, mem); break;
                case inst::MOV:     executeMOV(*this, mem); break;
                case inst::LB:      executeLB(*this, mem); break;
                case inst::SB:      executeSB(*this, mem); break;
                case inst::PUSH:    executePUSH(*this, mem); break;
                case inst::POP:     executePOP(*this, mem); break;
                case inst::BUF:     executeBUF(*this, mem); break;
                case inst::CMP:     executeCMP(*this, mem); break;
                case inst::AND:     executeAND(*this, mem); break;
                case inst::OR:      executeOR(*this, mem); break;
                case inst::XOR:     executeXOR(*this, mem); break;
                case inst::NOT:     executeNOT(*this, mem); break;
                default:
                    throw tpu::InvalidInstructionException( std::to_string(static_cast<u8>(instruction)) );
            }

            // TODO - sleep between cycles
        }
    }

    // Reads the next byte at the IP
    Byte TPU::nextByte(Memory& mem) {
        return mem.readByte(this->IP.dword++);
    }

    // Reads the next word at the IP
    Word TPU::nextWord(Memory& mem) {
        u32 addr = this->IP.dword;
        this->IP.dword += 2;
        return mem.readWord(addr);
    }

    // Reads the next dword at the IP
    DWord TPU::nextDWord(Memory& mem) {
        u32 addr = this->IP.dword;
        this->IP.dword += 4;
        return mem.readDWord(addr);
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
            case RegCode::IP:
                throw InvalidRegCodeException("Cannot write to IP register.");
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
                throw InvalidRegCodeException("readReg32: " + std::to_string(static_cast<int>(rc)));
        }
    }

    void TPU::pushByte(Memory& mem, const u8 b) {
        mem.setByte( ESP.dword, b );
        ++ESP.dword;
    }

    void TPU::pushWord(Memory& mem, const u16 w) {
        mem.setWord( ESP.dword, w );
        ESP.dword += 2;
    }

    void TPU::pushDWord(Memory& mem, const u32 dw) {
        mem.setDWord( ESP.dword, dw );
        ESP.dword += 4;
    }

    u8 TPU::popByte(Memory& mem) {
        --ESP.dword;
        return mem.readByte( ESP.dword );
    }

    u16 TPU::popWord(Memory& mem) {
        ESP.dword -= 2;
        return mem.readWord( ESP.dword ).word;
    }

    u32 TPU::popDWord(Memory& mem) {
        ESP.dword -= 4;
        return mem.readDWord( ESP.dword ).dword;
    }

    void TPU::setFlag(const int f, const bool b) {
        if (b)
            FLAGS.word |= (1u << f);
        else
            FLAGS.word &= ~(1u << f);
    }

    // Debug dump all registers to stdout
    void TPU::dumpRegs() const {
        // 32-bit regs
        std::printf("-------------------------=| REG DUMP |=-------------------------\n");
        std::printf("EAX: 0x%08x    AX: 0x%04x    AH: 0x%02x    AL: 0x%02x\n", EAX.dword, EAX.lword, EAX.lreg16.hbyte, EAX.lreg16.lbyte);
        std::printf("EBX: 0x%08x    BX: 0x%04x    BH: 0x%02x    BL: 0x%02x\n", EBX.dword, EBX.lword, EBX.lreg16.hbyte, EBX.lreg16.lbyte);
        std::printf("ECX: 0x%08x    CX: 0x%04x    CH: 0x%02x    CL: 0x%02x\n", ECX.dword, ECX.lword, ECX.lreg16.hbyte, ECX.lreg16.lbyte);
        std::printf("EDX: 0x%08x    DX: 0x%04x    DH: 0x%02x    DL: 0x%02x\n", EDX.dword, EDX.lword, EDX.lreg16.hbyte, EDX.lreg16.lbyte);

        std::printf("IP:  0x%08x\n", IP.dword);
        std::printf("RP:  0x%08x\n", RP.dword);
        std::printf("ESP: 0x%08x    SP: 0x%04x\n", ESP.dword, ESP.lword);
        std::printf("EBP: 0x%08x    BP: 0x%04x\n", EBP.dword, EBP.lword);
        std::printf("ESI: 0x%08x    SI: 0x%04x\n", ESI.dword, ESI.lword);
        std::printf("EDI: 0x%08x    DI: 0x%04x\n", EDI.dword, EDI.lword);

        // 16-bit regs
        std::printf("FLAGS: 0b%016b\n", FLAGS.word);
        std::printf(
            "  CARRY: %d  PARITY: %d  ZERO: %d  SIGN: %d  OVERFLOW: %d\n",
            isFlag(FLAG_CARRY), isFlag(FLAG_PARITY), isFlag(FLAG_ZERO), isFlag(FLAG_SIGN), isFlag(FLAG_OVERFLOW)
        );

        // 8-bit regs/flags
        std::printf("MODE: %d\n", (isInKernelMode ? 1 : 0));
    }

}


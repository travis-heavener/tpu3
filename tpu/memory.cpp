#include "memory.hpp"

namespace tpu {

    Memory::Memory(const u32 allocSize) {
        this->mem = new Byte[allocSize];
        this->_size = allocSize;

        // Zero the memory bank
        this->reset();
    }

    Memory::~Memory() {
        delete[] this->mem;
    }

    // Zero the memory bank
    void Memory::reset() {
        for (u32 i = 0; i < this->_size; ++i)
            this->mem[i] = 0;
    }

    Byte Memory::readByte(const u32 addr) const {
        if (addr >= this->_size) {
            throw MemoryReadOutOfBoundsException(
                "readByte(" + std::to_string(addr) + ") is outside allocated virtual memory (size " + std::to_string(this->_size) + ")"
            );
        }
        return mem[addr];
    }

    Word Memory::readWord(const u32 addr) const {
        if (addr >= this->_size) {
            throw MemoryReadOutOfBoundsException(
                "readByte(" + std::to_string(addr) + ") is outside allocated virtual memory (size " + std::to_string(this->_size) + ")"
            );
        }

        Word w;
        w.bytes[0] = mem[addr];
        w.bytes[1] = mem[addr + 1];
        return w;
    }

    DWord Memory::readDWord(const u32 addr) const {
        if (addr >= this->_size) {
            throw MemoryReadOutOfBoundsException(
                "readByte(" + std::to_string(addr) + ") is outside allocated virtual memory (size " + std::to_string(this->_size) + ")"
            );
        }

        DWord dw;
        dw.bytes[0] = mem[addr];
        dw.bytes[1] = mem[addr + 1];
        dw.bytes[2] = mem[addr + 2];
        dw.bytes[3] = mem[addr + 3];
        return dw;
    }

    void Memory::setByte(const u32 addr, const u8 v) {
        this->mem[addr] = v;
    }
    
    void Memory::setWord(const u32 addr, const u16 v) {
        this->mem[addr] = static_cast<u8>(v & 0xFF);
        this->mem[addr + 1] = static_cast<u8>( (v >> 8u) & 0xFF );
    }

    void Memory::setDWord(const u32 addr, const u32 v) {
        this->mem[addr] = static_cast<u8>(v & 0xFF);
        this->mem[addr + 1] = static_cast<u8>( (v >> 8u) & 0xFF );
        this->mem[addr + 2] = static_cast<u8>( (v >> 16u) & 0xFF );
        this->mem[addr + 3] = static_cast<u8>( (v >> 24u) & 0xFF );
    }

}

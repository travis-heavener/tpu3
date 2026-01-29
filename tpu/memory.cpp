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

    Byte Memory::byte(const u32 addr) const {
        return mem[addr];
    }

    Word Memory::word(const u32 addr) const {
        Word w;
        w.bytes[0] = mem[addr];
        w.bytes[1] = mem[addr + 1];
        return w;
    }

    DWord Memory::dword(const u32 addr) const {
        DWord dw;
        dw.bytes[0] = mem[addr];
        dw.bytes[1] = mem[addr + 1];
        dw.bytes[2] = mem[addr + 2];
        dw.bytes[3] = mem[addr + 3];
        return dw;
    }

}

#ifndef __TPU_MEMORY_HPP
#define __TPU_MEMORY_HPP

#include "tools.hpp"

namespace tpu {

    typedef u8 Byte;

    typedef union Word {
        u8 bytes[2];
        u16 word;
    } Word;

    typedef union DWord {
        u8 bytes[4];
        u16 words[2];
        u32 dword;
    } DWord;

    class Memory {
        public:
            Memory(const u32 allocSize);
            ~Memory();

            void reset();

            Byte* data() { return mem; };
            u32 size() const { return _size; };

            Byte byte(const u32 addr) const;
            Word word(const u32 addr) const;
            DWord dword(const u32 addr) const;
        private:
            Byte* mem;
            u32 _size;
    };

}

#endif

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

            Byte readByte(const u32 addr) const;
            Word readWord(const u32 addr) const;
            DWord readDWord(const u32 addr) const;

            void setByte(const u32 addr, const u8 v);
            void setWord(const u32 addr, const u16 v);
            void setDWord(const u32 addr, const u32 v);
        private:
            Byte* mem;
            u32 _size;
    };

}

#endif

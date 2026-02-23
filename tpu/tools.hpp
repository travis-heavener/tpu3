#ifndef __TPU_TOOLS_HPP
#define __TPU_TOOLS_HPP

#include <charconv>
#include <cstdint>
#include <stdexcept>
#include <string>

#define EXCEPTION_MSG_CLASS(name) \
    class name : public Exception { \
        public: \
            name(const std::string msg) : msg(#name ": " + msg) {}; \
            const char* what() const noexcept { return msg.c_str(); }; \
        private: \
            std::string msg; \
    };

namespace tpu {

    // Exceptions
    class Exception : public std::runtime_error {
        public:
            Exception() : std::runtime_error("") {};
            virtual const char* what() const noexcept { return "Base TPUException"; };
    };

    EXCEPTION_MSG_CLASS(MemoryReadOutOfBoundsException);
    EXCEPTION_MSG_CLASS(InvalidInstructionException);
    EXCEPTION_MSG_CLASS(InvalidMODBitsException);
    EXCEPTION_MSG_CLASS(InvalidRegCodeException);
    EXCEPTION_MSG_CLASS(InvalidSyscallException);
    EXCEPTION_MSG_CLASS(InsufficientModeException);
    EXCEPTION_MSG_CLASS(InvalidAddressException);

    // Data types
    typedef uint64_t u64;
    typedef uint32_t u32;
    typedef uint16_t u16;
    typedef uint8_t  u8;

    typedef int64_t s64;
    typedef int32_t s32;
    typedef int16_t s16;
    typedef int8_t  s8;

    // Register codes for instructions (excluding hidden registers)
    enum class RegCode : u8 {
        EAX = 0x00, AX  = 0x01, AH = 0x02, AL = 0x03,
        EBX = 0x04, BX  = 0x05, BH = 0x06, BL = 0x07,
        ECX = 0x08, CX  = 0x09, CH = 0x0A, CL = 0x0B,
        EDX = 0x0C, DX  = 0x0D, DH = 0x0E, DL = 0x0F,

        IP = 0x10,
        ESP = 0x11, SP = 0x12,
        EBP = 0x13, BP = 0x14,
        ESI = 0x15, SI = 0x16,
        EDI = 0x17, DI = 0x18,
        RP = 0x19
    };

    // String to uints
    // Ref: https://en.cppreference.com/w/cpp/utility/from_chars.html
    template <typename uint>
    uint stou(const std::string& s) {
        uint n;
        auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), n, 10);

        if (ec == std::errc::invalid_argument)
            throw std::invalid_argument("Invalid number passed to stou");

        if (ec == std::errc::result_out_of_range)
            throw std::invalid_argument("Value out of range in stou");

        if (ptr != s.data() + s.size())
            throw std::invalid_argument("Trailing characters passed to stou");

        return n;
    }

    // Flags helpers

    // Returns the parity of the least significant byte
    // See https://en.wikipedia.org/wiki/Parity_flag
    template <typename uint>
    inline constexpr bool parity(const uint _n) {
        const u32 n = static_cast<u32>(_n);
        bool parity = true;
        for (int i = 0; i < 8; ++i)
            if (((1u << i) & n) != 0)
                parity = !parity;
        return parity;
    }

}

#endif

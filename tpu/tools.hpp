#ifndef __TPU_TOOLS_HPP
#define __TPU_TOOLS_HPP

#include <charconv>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace tpu {

    typedef uint64_t u64;
    typedef uint32_t u32;
    typedef uint16_t u16;
    typedef uint8_t  u8;

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

    // Returns the parity of the given byte(s)
    template <typename uint>
    bool parity(uint _n) {
        const u32 n = static_cast<u32>( _n );
        bool parity = true;

        for (int i = 0; i < 32; ++i) {
            if ((n >> i) & 1) {
                parity = !parity;
            }
        }

        return parity;
    }

    // Exceptions
    class Exception : public std::runtime_error {
        public:
            Exception() : std::runtime_error("") {};
            virtual const char* what() const noexcept { return "Base TPUException"; };
    };

    class InvalidInstructionException : public Exception {
        public:
            InvalidInstructionException(const std::string msg) : msg("InvalidInstructionException: " + msg) {};
            const char* what() const noexcept { return msg.c_str(); };
        private:
            std::string msg;
    };

}

#endif

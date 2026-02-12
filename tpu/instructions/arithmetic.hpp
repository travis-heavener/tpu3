#ifndef __TPU_INSTRUCTIONS_ARITHMETIC_HPP
#define __TPU_INSTRUCTIONS_ARITHMETIC_HPP

#include "../tpu.hpp"

namespace tpu {

    // ALUTraits helpers
    template<typename U>
    struct ALUTraits;

    template<>
    struct ALUTraits<u8> {
        static constexpr int nbits = 8;
        using S = s8; using SW = s16;
        using UW = u16;
        static constexpr u8 SIGN_MASK = 0x80;
        static constexpr u8 MAX_MASK = 0xFF;
        static u8 readReg(TPU& tpu, RegCode r) { return tpu.readReg8(r); }
        static void setReg(TPU& tpu, RegCode r, u8 v) { tpu.setReg8(r, v); }
    };

    template<>
    struct ALUTraits<u16> {
        static constexpr int nbits = 16;
        using S = s16; using SW = s32;
        using UW = u32;
        static constexpr u16 SIGN_MASK = 0x8000;
        static constexpr u16 MAX_MASK = 0xFFFF;
        static u16 readReg(TPU& tpu, RegCode r) { return tpu.readReg16(r); }
        static void setReg(TPU& tpu, RegCode r, u16 v) { tpu.setReg16(r, v); }
    };

    template<>
    struct ALUTraits<u32> {
        static constexpr int nbits = 32;
        using S = s32; using SW = s64;
        using UW = u64;
        static constexpr u32 SIGN_MASK = 0x8000'0000;
        static constexpr u32 MAX_MASK = 0xFFFF'FFFF;
        static u32 readReg(TPU& tpu, RegCode r) { return tpu.readReg32(r); }
        static void setReg(TPU& tpu, RegCode r, u32 v) { tpu.setReg32(r, v); }
    };

    /*********************************************************************************************/
    /*************************** Arithmetic instruction helper methods ***************************/
    /*********************************************************************************************/

    template<typename U> // Unsigned type
    void aluADD(TPU& tpu, const U a, const U b, const RegCode dest, const bool isSigned) {
        using T = ALUTraits<U>;
        using S = typename T::S; // Signed type
        using SW = typename T::SW; // Signed wider type
        using UW = typename T::UW; // Wider type

        if (!isSigned) { // Unsigned
            const UW full = static_cast<UW>(a) + static_cast<UW>(b);
            const U result = static_cast<U>(full & T::MAX_MASK);

            T::setReg(tpu, dest, result);

            // Flags
            tpu.setFlag(FLAG_CARRY, full > T::MAX_MASK);
            tpu.setFlag(FLAG_PARITY, parity<U>(result));
            tpu.setFlag(FLAG_ZERO, result == 0);
            tpu.setFlag(FLAG_SIGN, (result & T::SIGN_MASK) != 0);
            // Overflow flag irrelevant for unsigned arithmetic
        } else { // Signed
            const S sa = static_cast<S>(a);
            const S sb = static_cast<S>(b);
            const SW full = static_cast<SW>(sa) + static_cast<SW>(sb);
            const S result = static_cast<S>(full & T::MAX_MASK);

            T::setReg(tpu, dest, static_cast<U>(result));

            // Flags
            // Carry flag irrelevant for signed arithmetic
            tpu.setFlag(FLAG_PARITY, parity<U>(static_cast<U>(result)));
            tpu.setFlag(FLAG_ZERO, result == 0);
            tpu.setFlag(FLAG_SIGN, (static_cast<U>(result) & T::SIGN_MASK) != 0);
            tpu.setFlag(FLAG_OVERFLOW, ((sa ^ result) & (sb ^ result) & T::SIGN_MASK) != 0 );
        }
    }

    template<typename U> // Unsigned type
    void aluSUB(TPU& tpu, const U a, const U b, const RegCode dest, const bool isSigned) {
        using T = ALUTraits<U>;
        using S = typename T::S; // Signed type
        using UW = typename T::UW; // Wider type

        if (!isSigned) { // Unsigned
            const UW full = static_cast<UW>(a) - static_cast<UW>(b);
            const U result = static_cast<U>(full & T::MAX_MASK);

            T::setReg(tpu, dest, result);

            // Flags
            tpu.setFlag(FLAG_CARRY, a < b);
            tpu.setFlag(FLAG_PARITY, parity<U>(result));
            tpu.setFlag(FLAG_ZERO, result == 0);
            tpu.setFlag(FLAG_SIGN, (result & T::SIGN_MASK) != 0);
            // Overflow flag irrelevant for unsigned arithmetic
        } else { // Signed
            const S sa = static_cast<S>(a);
            const S sb = static_cast<S>(b);
            const S result = static_cast<S>(sa - sb);

            T::setReg(tpu, dest, static_cast<U>(result));

            // Flags
            // Carry flag irrelevant for signed arithmetic
            tpu.setFlag(FLAG_PARITY, parity<U>(static_cast<U>(result)));
            tpu.setFlag(FLAG_ZERO, result == 0);
            tpu.setFlag(FLAG_SIGN, (static_cast<U>(result) & T::SIGN_MASK) != 0);
            tpu.setFlag(FLAG_OVERFLOW, ((sa ^ sb) & (sa ^ result) & T::SIGN_MASK) != 0 );
        }
    }

    // NOTE: aluCMP and aluSUB are effectively the same for signed and unsigned arithmetic
    //       except that aluSUB stores the result in `dest`, aluCMP discards it
    template<typename U> // Unsigned type
    void aluCMP(TPU& tpu, const U a, const U b, const bool isSigned) {
        using T = ALUTraits<U>;
        using S = typename T::S; // Signed type
        using UW = typename T::UW; // Wider type

        if (!isSigned) { // Unsigned
            const UW full = static_cast<UW>(a) - static_cast<UW>(b);
            const U result = static_cast<U>(full & T::MAX_MASK);

            // Flags
            tpu.setFlag(FLAG_CARRY, a < b);
            tpu.setFlag(FLAG_PARITY, parity<U>(result));
            tpu.setFlag(FLAG_ZERO, result == 0);
            tpu.setFlag(FLAG_SIGN, (result & T::SIGN_MASK) != 0);
            // Overflow flag irrelevant for unsigned arithmetic
        } else { // Signed
            const S sa = static_cast<S>(a);
            const S sb = static_cast<S>(b);
            const S result = static_cast<S>(sa - sb);

            // Flags
            // Carry flag irrelevant for signed arithmetic
            tpu.setFlag(FLAG_PARITY, parity<U>(static_cast<U>(result)));
            tpu.setFlag(FLAG_ZERO, result == 0);
            tpu.setFlag(FLAG_SIGN, (static_cast<U>(result) & T::SIGN_MASK) != 0);
            tpu.setFlag(FLAG_OVERFLOW, ((sa ^ sb) & (sa ^ result) & T::SIGN_MASK) != 0 );
        }
    }

    template<typename U> // Unsigned type
    void aluMUL(TPU& tpu, const U a, const U b, const bool isSigned) {
        using T = ALUTraits<U>;
        using S = typename T::S; // Signed type
        using UW = typename T::UW; // Wider type
        using SW = typename T::SW; // Signed-wider type

        if (!isSigned) { // Unsigned
            const UW result = static_cast<UW>(a) * static_cast<UW>(b);
            const U low  = static_cast<U>(result & T::MAX_MASK);
            const U high = static_cast<U>(result >> T::nbits);

            // Handle the dest reg by the size of the arguments
            if (T::nbits == 8) {
                tpu.setReg16(RegCode::AX, result);
            } else if (T::nbits == 16) {
                tpu.setReg32(RegCode::EAX, result);
            } else {
                tpu.setReg32(RegCode::EAX, low); // Lower half
                tpu.setReg32(RegCode::EDX, high); // Upper half
            }

            // Flags
            tpu.setFlag(FLAG_CARRY, high != 0);
            tpu.setFlag(FLAG_OVERFLOW, high != 0);
        } else { // Signed
            const SW result = static_cast<SW>( static_cast<S>(a) ) * static_cast<SW>( static_cast<S>(b) );
            const S lowSigned  = static_cast<S>(result);
            const S highSigned = static_cast<S>(result >> T::nbits);

            // Handle the dest reg by the size of the arguments
            if (T::nbits == 8) {
                tpu.setReg16(RegCode::AX, static_cast<UW>(result));
            } else if (T::nbits == 16) {
                tpu.setReg32(RegCode::EAX, static_cast<UW>(result));
            } else {
                tpu.setReg32(RegCode::EAX, static_cast<U>(lowSigned)); // Lower half
                tpu.setReg32(RegCode::EDX, static_cast<U>(highSigned)); // Upper half
            }

            // Flags
            // Check if sign is preserved
            tpu.setFlag(FLAG_CARRY, static_cast<SW>(static_cast<S>(result)) != result );
            tpu.setFlag(FLAG_OVERFLOW, static_cast<SW>(static_cast<S>(result)) != result );
        }
    }

}

#endif

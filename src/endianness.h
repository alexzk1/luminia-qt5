#pragma once

#include <iostream>
#include <iomanip>
#include <cstdint>

namespace endianness
{
    //https://codereview.stackexchange.com/questions/45675/checking-endianness-at-compile-time
    enum hl_endianness : uint32_t
    {
        HL_LITTLE_ENDIAN   = 0x00000001,
        HL_BIG_ENDIAN      = 0x01000000,
        HL_PDP_ENDIAN      = 0x00010000,
        HL_UNKNOWN_ENDIAN  = 0xFFFFFFFF
    };

    constexpr inline hl_endianness getEndianOrder()
    {
        constexpr uint32_t test = 1 & 0xFFFFFFFF;

        if (test == HL_LITTLE_ENDIAN || test == HL_BIG_ENDIAN || test == HL_PDP_ENDIAN)
            return static_cast<hl_endianness>(test);

        return HL_UNKNOWN_ENDIAN;
    }

    constexpr hl_endianness endianness = getEndianOrder();


    template<typename T>
    T identity(T v)
    {
        return v;
    }

    template <class T>
    inline T swap_(T v);

    // 16 bits values ------
    template <>
    inline uint16_t swap_(uint16_t v)
    {
        return ((v & 0xFF) << 8) | ((v & 0xFF00) >> 8);
    }

    // 32 bits values ------
    template <>
    inline uint32_t swap_(uint32_t v)
    {
        //return ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v & 0xFF0000) >> 8) | ((v & 0xFF000000) >> 24);
        return __builtin_bswap32(v);
    }

    template <>
    inline uint64_t swap_(uint64_t src)
    {
        return __builtin_bswap64(src);
    }

    template<typename T, hl_endianness HOST, hl_endianness REMOTE>
    struct en_swap
    {
        static T conv(T v)
        {
            return swap_(v);
        }
    };

    template<typename T>
    struct en_swap<T, hl_endianness::HL_BIG_ENDIAN, hl_endianness::HL_BIG_ENDIAN>
    {
        static T conv(T v)
        {
            return v;
        }
    };

    template<typename T>
    struct en_swap<T, hl_endianness::HL_LITTLE_ENDIAN, hl_endianness::HL_LITTLE_ENDIAN>
    {
        static T conv(T v)
        {
            return v;
        }
    };
    //PDP_endian should fail compilation, dont deal with it yet

    template<typename T>
    T to_big(T v)
    {
        switch (endianness)
        {
            case hl_endianness::HL_LITTLE_ENDIAN :
                return en_swap<T, hl_endianness::HL_LITTLE_ENDIAN, hl_endianness::HL_BIG_ENDIAN>::conv(v);
            case hl_endianness::HL_BIG_ENDIAN :
                return en_swap<T, hl_endianness::HL_BIG_ENDIAN, hl_endianness::HL_BIG_ENDIAN>::conv(v);
        }
    }

    template<typename T>
    T to_little(T v)
    {
        switch (endianness)
        {
            case hl_endianness::HL_LITTLE_ENDIAN :
                return en_swap<T, hl_endianness::HL_LITTLE_ENDIAN, hl_endianness::HL_LITTLE_ENDIAN>::conv(v);
            case hl_endianness::HL_BIG_ENDIAN :
                return en_swap<T, hl_endianness::HL_BIG_ENDIAN, hl_endianness::HL_LITTLE_ENDIAN>::conv(v);
        }
    }

    template <class T>
    typename std::enable_if<std::is_integral<T>::value, T>::type le2cpu(T val)
    {
        if (endianness::HL_LITTLE_ENDIAN == endianness::endianness)
            return val;
        return endianness::swap_<T>(val);
    }
}

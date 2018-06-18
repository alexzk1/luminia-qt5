#ifndef FILE_LOADER_H
#define FILE_LOADER_H
#include <QFile>
#include <vector>
#include <type_traits>
#include "endianness.h"

namespace utility
{
    //allows properly read things according to strict-aliasing

    template <class T>
    qint64 readFromFile(QFile& file, T& out, size_t sizeLimit = std::numeric_limits<size_t>::max())
    {
        sizeLimit = std::min(sizeLimit, sizeof (T));
        std::vector<char> buf;
        buf.resize(sizeLimit);
        qint64 r = file.read(buf.data(), static_cast<decltype (r)>(buf.size()));
        if (r == static_cast<decltype (r)>(buf.size()))
            memcpy(&out, buf.data(), buf.size());
        return r;
    }

    template  <class T>
    typename std::enable_if<std::is_integral<T>::value, T>::type readIntegralFromFile(QFile& file)
    {
        T out;
        readFromFile<T>(file, out);
        return out;
    }

    //need for DDS, microsoft always assume little-endian, butt program may be compiled on big-endian...
    template <class T>
    typename std::enable_if<std::is_integral<T>::value, T>::type le2cpu(T val)
    {
        if (endianness::HL_LITTLE_ENDIAN == endianness::endianness)
            return val;
        return endianness::swap_<T>(val);
    }
}
#endif // FILE_LOADER_H

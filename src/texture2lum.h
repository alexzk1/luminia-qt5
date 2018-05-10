#ifndef TEXTURE2LUM_H
#define TEXTURE2LUM_H
#include <vector>

namespace base64
{
#include "libbase64.h"
    std::vector<char> encode_base64(const std::vector<char>& src)
    {
        //lets try dumb way 1st...
        std::vector<char> result;
        size_t sz = static_cast<size_t>(100 + src.size() * 4 / 3.);
        result.resize(sz);
        base64_encode(src.data(), src.size(), result.data(), &sz, 0);
        result.resize(sz);
        return result;
    }
}



#endif // TEXTURE2LUM_H

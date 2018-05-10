#ifdef _DEBUG
#include <iostream>
#include "texture2lum.h"
#include <QDebug>
#include <algorithm>
#include <vector>


using vchr_t = std::vector<char>;

size_t static countDiffs(const vchr_t& v1, const vchr_t & v2)
{
    size_t mis_counter = 0;
    std::mismatch(v1.begin(), v1.end(), v2.begin(), [&mis_counter](auto a, auto b)
    {
        if ( a != b)
            ++mis_counter;
        return true;
    });
    return mis_counter;
}


static void error_not_same(const QString& pref, const vchr_t& v1, const vchr_t & v2)
{
    if (v1.size() != v2.size())
    {
        qDebug() << pref << " have different size. FAILED.";
        exit(1);
    }
    auto dif = countDiffs(v1, v2);
    if (dif)
    {
        qDebug() << pref << " are different (" << dif << "). FAILED.";
        exit(2);
    }
    qDebug() << pref << " are same. PASSED.";
}



void static testTexCompressor()
{
    using namespace utility;
    qDebug() << "Testing texture pack/encode";
    std::vector<char> src(10 * 1024 * 1024, 0);
    std::generate(src.begin(), src.end(), std::rand);
    qDebug() << "Got " << src.size() << " bytes of randoms.";

    {
        qDebug() << "Testing base64 enc/dec ...";
        auto b64 = tex_encode_base64(src, false);
        qDebug() << "base64 size: " << b64.size();
        auto dec = tex_decode(std::string(b64.data(), b64.size()));
        error_not_same("Source and Decoded encoded base64", src, dec);
    }

    {
        auto packed = tex_lz4_encode_base64(src);
        qDebug() << "Got " << packed.size() << " bytes of compressed + base64. Going to decode it.";


        auto decoded = tex_decode(std::string(packed.data(), packed.size()));
        qDebug() << "Got " << decoded.size() << " bytes of decoded.";

        error_not_same("Source and Decompressed", src, decoded);
    }
}


void execTests()
{
    testTexCompressor();



    std::cout.flush();
    std::cerr.flush();
    exit(0);
}
#endif

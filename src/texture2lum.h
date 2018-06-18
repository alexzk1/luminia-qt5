#ifndef TEXTURE2LUM_H
#define TEXTURE2LUM_H
#include <vector>
#include <lz4.h>
#include <memory.h>
#include <string>
#include "file_loader.h"
#include <QString>
#include <QByteArray>

#define COMPRESSED_TEX_SIGN "%COMPR%"
#define messageMaxBytes (1048576)
namespace utility
{
#include "libbase64.h"
    std::vector<char> inline tex_encode_base64(const std::vector<char>& src, bool compressed)
    {
        const static std::string sign(COMPRESSED_TEX_SIGN);
        //lets try dumb way 1st...
        std::vector<char> result;
        auto sz = static_cast<size_t>(100 + src.size() * 4 / 3.);
        result.resize(sz + sign.length());
        auto delta = (compressed) ? sign.length() : 0;
        if (compressed)
            memcpy(result.data(), sign.data(), sign.length());

        base64_encode(src.data(), src.size(), result.data() + delta, &sz, 0);
        result.resize(sz + delta);
        return result;
    }

    std::vector<char> inline tex_lz4_encode_base64(const std::vector<char>& src)
    {
        //need to use blocks, because lib has same ints which may overflow...

        LZ4_stream_t* const lz4Stream = LZ4_createStream();
        const size_t cmpBufBytes = LZ4_COMPRESSBOUND(messageMaxBytes);
        std::vector<char> buffer(cmpBufBytes + 4096);

        std::vector<char> result;
        result.reserve(cmpBufBytes);

        for (size_t pos_in = 0, sz_in = src.size(); pos_in < sz_in; pos_in += messageMaxBytes)
        {
            const size_t curr_in_sz = std::min<size_t>(messageMaxBytes, sz_in - pos_in);
            const char *in = src.data() + pos_in;

            const int cmpBytes = LZ4_compress_fast_continue(lz4Stream, in, buffer.data(), int(curr_in_sz), int(buffer.size()), 1);

            if (cmpBytes <= 0)
                break;

            const auto sz = static_cast<uint32_t>(cmpBytes);

            size_t res_off = result.size();
            result.resize(res_off + size_t( cmpBytes ) + 4);
            utility::le2cpu(sz); //ensure writting LE
            memcpy(result.data() + res_off, &cmpBytes, 4);
            utility::le2cpu(sz);
            memcpy(result.data() + res_off + 4, buffer.data(), size_t(cmpBytes));
        }
        LZ4_freeStream(lz4Stream);

        //if compression is too bad, then dont store it, ignore
        if (result.size() > 0.9 * src.size())
            return tex_encode_base64(src, false);

        return tex_encode_base64(result, true);
    }

    std::vector<char> inline tex_decode(const std::string& src)
    {
        std::vector<char> result;
        const static std::string sign(COMPRESSED_TEX_SIGN);
        const bool compressed = memcmp(src.data(), sign.data(), sign.size()) == 0;
        size_t delta = ((compressed) ? sign.size() : 0);
        const char *psrc = src.data() + delta;
        std::vector<char> decoded;
        decoded.resize(src.size());
        if (base64_decode(psrc, src.size() - delta, decoded.data(), &delta, 0))
        {
            decoded.resize(delta);
            if (!compressed)
                return decoded;


            LZ4_streamDecode_t* const lz4StreamDecode = LZ4_createStreamDecode();
            size_t shrink_delta = 0;

            for (size_t outOffset = 0, srcOffset = 0;;)
            {
                //extracting prior stored compressed block size
                uint32_t cmpBytes = 0;
                memcpy(&cmpBytes, decoded.data() + srcOffset, 4);
                srcOffset += 4;
                cmpBytes = le2cpu(cmpBytes); //ensuring it is LE byte order

                if (shrink_delta > messageMaxBytes)
                    shrink_delta -= messageMaxBytes; //"virtual" resize, we already have enough space there
                else
                    result.resize(result.size() + messageMaxBytes);

                int decodedSize = LZ4_decompress_safe_continue(lz4StreamDecode, decoded.data() + srcOffset, result.data() + outOffset, int(cmpBytes), messageMaxBytes);
                shrink_delta += decltype(shrink_delta)(messageMaxBytes - std::max(0, decodedSize)); //that is extra space we have in target buffer

                if (decodedSize < 1)
                    break;

                srcOffset += cmpBytes;
                outOffset += decltype(outOffset)(decodedSize);

            }

            LZ4_freeStreamDecode(lz4StreamDecode);
            if (shrink_delta)
                result.resize(result.size() - shrink_delta);
        }

        return result;
    }
}

#undef COMPRESSED_TEX_SIGN
#undef messageMaxBytes


#endif // TEXTURE2LUM_H

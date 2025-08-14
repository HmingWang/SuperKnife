#pragma once
#include "openssl.h"

std::string base64_encode(const std::string &in, const bool wrap = false);
std::string base64_decode(const std::string &in);

namespace x::crypto
{
    class Base64
    {
    public:
        class Encoder
        {
        public:
            static std::string encode(const Bytes &in, const bool wrap = false);
        };

        class Decoder
        {
        public:
            static Bytes decode(const std::string_view in);
        };
    };

}
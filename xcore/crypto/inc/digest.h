#pragma once
#include "openssl.h"

namespace x::crypto
{
    class Digest
    {

    public:
        class Generator
        {
        public:
            static Digest sm3();
            static Digest sha1();
            static Digest sha256();
            static Digest sha512();
            static Digest md5();
        };

        Digest(EVP_MD_ptr md) : m_md(std::move(md)) {}

        Bytes hash_file(std::string_view filename);
        Bytes hash(Bytes data);
        EVP_MD* get_EVP_MD() const {return m_md.get();}

    private:
        EVP_MD_ptr m_md;
    };
}
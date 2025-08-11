#pragma once

#include "pkey.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "bytes.h"

namespace x::crypto
{
    class SM2 : public PKey
    {
    public:
        Bytes encrypt(const Bytes &plaintext);
        Bytes decrypt(const Bytes &ciphertext);
        Bytes sign( const Bytes &message);
        bool verify( const Bytes &message, const Bytes &signature);
    };
}
#pragma once
#include "openssl.h"

namespace x::crypto
{

    class Cipher
    {
    public:
        enum class Algorithm
        {
            AES_128,
            AES_192,
            AES_256,
            SM4
        };
        enum class Mode
        {
            ECB,
            CBC,
            CFB,
            OFB,
            GCM,
            CTR
        };
        struct Params
        {
            Algorithm algorithm;
            Mode mode;
            size_t key_size;
            size_t iv_size;
            size_t block_size;
            size_t tag_size; // Only used for GCM mode
        };

    public:
        class Generator
        {
        public:
            static Cipher maker(Algorithm algo, Mode mode);

        private:
            static Params get_default_params(Algorithm algo, Mode mode);
            static EVP_CIPHER_ptr get_EVP_CIPHER(Algorithm algo, Mode mode);
        };

        Cipher(EVP_CIPHER_ptr c, Params parm) : m_cipher(std::move(c)), m_key{}, m_iv{}, m_params(parm) {}
        void set_key_iv(const Bytes &key, const Bytes &iv = {});
        Bytes encrypto(const Bytes &plaintext);
        Bytes encrypto(const Bytes &plaintext, Bytes &tag, const Bytes &aad = {});
        Bytes decrypto(const Bytes &ciphertext, const Bytes &tag = {}, const Bytes &aad = {});

        EVP_CIPHER *get_EVP_CIPHER() const { return m_cipher.get(); }
        const Params &get_params() const { return m_params; }

    private:
        EVP_CIPHER_ptr m_cipher;
        Bytes m_key;
        Bytes m_iv;
        Params m_params;

        void validate_key_iv();
        Bytes generate_random_iv();
    };

}
#include "sm2.h"
#include <openssl/err.h>
#include <openssl/provider.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/obj_mac.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>

using namespace std;

void handleOpenSSLError(const string &message)
{
    char errBuf[256];
    ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
    throw runtime_error(message + ": " + errBuf);
}

// ==================== SM2KeyPair Implementation ====================

bool SM2KeyPair::generateKeyPair()
{
    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx)
        return false;

    if (EVP_PKEY_keygen_init(ctx.get()) <= 0)
        return false;
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_sm2) <= 0)
        return false;

    EVP_PKEY *key = nullptr;
    if (EVP_PKEY_keygen(ctx.get(), &key) <= 0)
        return false;

    if (m_privateKey)
        EVP_PKEY_free(m_privateKey);
    if (m_publicKey)
        EVP_PKEY_free(m_publicKey);

    m_privateKey = key;
    if (EVP_PKEY_up_ref(key) != 1)
    {
        EVP_PKEY_free(key);
        m_privateKey = nullptr;
        return false;
    }
    m_publicKey = key;

    return true;
}

// ==================== SM2Certificate Implementation ====================

bool SM2Certificate::createSelfSigned(SM2KeyPair &keyPair, const std::string &subject, int validDays)
{
    return Cert::createSelfSigned(keyPair, EVP_sm3(), subject, validDays);
}

SM2Certificate SM2Certificate::signedCertificate(CertReq &req, SM2KeyPair &caKeyPair, int validDays)
{
    return SM2Certificate(Cert::signedCertificate(req, caKeyPair, EVP_sm3(), validDays));
}

// ==================== SM2Crypto Implementation ====================

vector<unsigned char> SM2Crypto::encrypt(EVP_PKEY *publicKey,
                                         const vector<unsigned char> &plaintext)
{
    if (!publicKey || plaintext.empty())
        return {};

    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(publicKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx)
        handleOpenSSLError("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_encrypt_init(ctx.get()) <= 0)
        handleOpenSSLError("Failed to initialize encryption");

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx.get(), nullptr, &outlen, plaintext.data(), plaintext.size()) <= 0)
        handleOpenSSLError("Failed to get output buffer size");

    vector<unsigned char> ciphertext(outlen);
    if (EVP_PKEY_encrypt(ctx.get(), ciphertext.data(), &outlen, plaintext.data(), plaintext.size()) <= 0)
        handleOpenSSLError("Failed to encrypt data");

    ciphertext.resize(outlen);
    return ciphertext;
}

vector<unsigned char> SM2Crypto::decrypt(EVP_PKEY *privateKey,
                                         const vector<unsigned char> &ciphertext)
{
    if (!privateKey || ciphertext.empty())
        return {};

    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(privateKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx)
        handleOpenSSLError("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_decrypt_init(ctx.get()) <= 0)
        handleOpenSSLError("Failed to initialize decryption");

    size_t outlen;
    if (EVP_PKEY_decrypt(ctx.get(), nullptr, &outlen, ciphertext.data(), ciphertext.size()) <= 0)
        handleOpenSSLError("Failed to get output buffer size");

    vector<unsigned char> plaintext(outlen);
    if (EVP_PKEY_decrypt(ctx.get(), plaintext.data(), &outlen, ciphertext.data(), ciphertext.size()) <= 0)
        handleOpenSSLError("Failed to decrypt data");

    plaintext.resize(outlen);
    return plaintext;
}

vector<unsigned char> SM2Crypto::sign(EVP_PKEY *privateKey,
                                      const vector<unsigned char> &message)
{
    if (!privateKey || message.empty())
        return {};

    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx)
        handleOpenSSLError("Failed to create EVP_MD_CTX");

    if (EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr, privateKey) <= 0)
        handleOpenSSLError("Failed to initialize signing");

    size_t siglen;
    if (EVP_DigestSign(md_ctx.get(), nullptr, &siglen, message.data(), message.size()) <= 0)
        handleOpenSSLError("Failed to get signature length");

    vector<unsigned char> signature(siglen);
    if (EVP_DigestSign(md_ctx.get(), signature.data(), &siglen, message.data(), message.size()) <= 0)
        handleOpenSSLError("Failed to sign data");

    signature.resize(siglen);
    return signature;
}

bool SM2Crypto::verify(EVP_PKEY *publicKey,
                       const vector<unsigned char> &message,
                       const vector<unsigned char> &signature)
{
    if (!publicKey || message.empty() || signature.empty())
        return false;

    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx)
        handleOpenSSLError("Failed to create EVP_MD_CTX");

    if (EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr, publicKey) <= 0)
        handleOpenSSLError("Failed to initialize verification");

    return EVP_DigestVerify(md_ctx.get(), signature.data(), signature.size(),
                            message.data(), message.size()) == 1;
}

vector<unsigned char> SM2Crypto::stringToVector(const string &str)
{
    return vector<unsigned char>(str.begin(), str.end());
}

string SM2Crypto::vectorToString(const vector<unsigned char> &vec)
{
    return string(vec.begin(), vec.end());
}

string SM2Crypto::toHex(const vector<unsigned char> &data)
{
    ostringstream oss;
    oss << hex << setfill('0');
    for (unsigned char byte : data)
    {
        oss << setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}




#include "sm2.h"
#include "openssl.h"

using namespace x::crypto;

// ==================== SM2Crypto Implementation ====================

Bytes SM2::encrypt(const Bytes &plaintext)
{
  if (plaintext.empty())
    return {};

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr));
  OSSL_ASSERT_PTR(ctx);
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt_init(ctx.get()));

  size_t outlen;
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt(ctx.get(), nullptr, &outlen, plaintext.c_cptr(), plaintext.size()));

  Bytes ciphertext(outlen);
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt(ctx.get(), ciphertext.c_ptr(), &outlen, plaintext.c_cptr(), plaintext.size()));
  return std::move(ciphertext);
}

Bytes SM2::decrypt(const Bytes &ciphertext)
{
  if (ciphertext.empty())
    return {};

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr));
  OSSL_ASSERT_PTR(ctx);

  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt_init(ctx.get()));

  size_t outlen;
  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt(ctx.get(), nullptr, &outlen, ciphertext.c_cptr(), ciphertext.size()));

  Bytes plaintext(outlen);
  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt(ctx.get(), plaintext.c_ptr(), &outlen, ciphertext.c_cptr(), ciphertext.size()));

  return std::move(plaintext);
}

vector<unsigned char> SM2Crypto::sign(EVP_PKEY *privateKey,
                                      const vector<unsigned char> &message)
{
  if (!privateKey || message.empty())
    return {};

  unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(),
                                                            &EVP_MD_CTX_free);
  if (!md_ctx)
    handleOpenSSLError("Failed to create EVP_MD_CTX");

  if (EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr,
                         privateKey) <= 0)
    handleOpenSSLError("Failed to initialize signing");

  size_t siglen;
  if (EVP_DigestSign(md_ctx.get(), nullptr, &siglen, message.data(),
                     message.size()) <= 0)
    handleOpenSSLError("Failed to get signature length");

  vector<unsigned char> signature(siglen);
  if (EVP_DigestSign(md_ctx.get(), signature.data(), &siglen, message.data(),
                     message.size()) <= 0)
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

  unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(),
                                                            &EVP_MD_CTX_free);
  if (!md_ctx)
    handleOpenSSLError("Failed to create EVP_MD_CTX");

  if (EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr,
                           publicKey) <= 0)
    handleOpenSSLError("Failed to initialize verification");

  return EVP_DigestVerify(md_ctx.get(), signature.data(), signature.size(),
                          message.data(), message.size()) == 1;
}

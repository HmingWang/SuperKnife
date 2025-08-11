#include "sm2.h"
#include "bytes.h"
#include "openssl.h"

using namespace x::crypto;

// ==================== SM2Crypto Implementation ====================

Bytes SM2::encrypt(const Bytes &plaintext) {
  if (plaintext.empty())
    return {};

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr));
  OSSL_ASSERT_PTR(ctx);
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt_init(ctx.get()));

  size_t outlen;
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt(ctx.get(), nullptr, &outlen,
                                    plaintext.c_cptr(), plaintext.size()));

  Bytes ciphertext(outlen);
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt(ctx.get(), ciphertext.c_ptr(), &outlen,
                                    plaintext.c_cptr(), plaintext.size()));
  return std::move(ciphertext);
}

Bytes SM2::decrypt(const Bytes &ciphertext) {
  if (ciphertext.empty())
    return {};

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr));
  OSSL_ASSERT_PTR(ctx);

  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt_init(ctx.get()));

  size_t outlen;
  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt(ctx.get(), nullptr, &outlen,
                                    ciphertext.c_cptr(), ciphertext.size()));

  Bytes plaintext(outlen);
  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt(ctx.get(), plaintext.c_ptr(), &outlen,
                                    ciphertext.c_cptr(), ciphertext.size()));

  return std::move(plaintext);
}

Bytes SM2::sign(const Bytes &message) {
  if (message.empty())
    return {};

  EVP_MD_CTX_ptr md_ctx(EVP_MD_CTX_new());
  OSSL_ASSERT_PTR(md_ctx);

  OSSL_ASSERT_FUNC(EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr,
                                      get_EVP_PKEY()));

  size_t siglen;
  OSSL_ASSERT_FUNC(EVP_DigestSign(md_ctx.get(), nullptr, &siglen,
                                  message.c_cptr(), message.size()));
  Bytes signature(siglen);
  OSSL_ASSERT_FUNC(EVP_DigestSign(md_ctx.get(), signature.c_ptr(), &siglen,
                                  message.c_cptr(), message.size()));

  return std::move(signature);
}

bool SM2::verify(const Bytes &message, const Bytes &signature) {
  if (message.empty() || signature.empty())
    return false;

  EVP_MD_CTX_ptr md_ctx(EVP_MD_CTX_new());
  OSSL_ASSERT_PTR(md_ctx);

  OSSL_ASSERT_FUNC(EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(),
                                        nullptr, get_EVP_PKEY()));

  return EVP_DigestVerify(md_ctx.get(), signature.c_cptr(), signature.size(),
                          message.c_cptr(), message.size()) == 1;
}

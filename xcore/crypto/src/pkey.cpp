#include "pkey.h"
#include "file.h"
#include "openssl.h"
#include <cstddef>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string_view>

using namespace x::crypto;

KeyPair KeyPair::Generator::sm2() {
  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_SM2, nullptr));
  EVP_PKEY *key = nullptr;

  OSSL_ASSERT_PTR(ctx);
  OSSL_ASSERT_FUNC(EVP_PKEY_CTX_set1_id(ctx.get(), "1234567812345678", 16));
  OSSL_ASSERT_FUNC(EVP_PKEY_keygen_init(ctx.get()));
  OSSL_ASSERT_FUNC(EVP_PKEY_keygen(ctx.get(), &key));

  return std::move(KeyPair(EVP_PKEY_ptr(key)));
}

void KeyPair::save_public(std::string_view filename) {
  OSSL_ASSERT_PTR(m_pkey);
  BIO_ptr bio(BIO_new_file(filename.data(), "w"));
  OSSL_ASSERT_PTR(bio);
  OSSL_ASSERT_FUNC(PEM_write_bio_PUBKEY(bio.get(), m_pkey.get()));
}

void KeyPair::save_private(std::string_view filename, std::string_view passwd) {
  OSSL_ASSERT_PTR(m_pkey);
  BIO_ptr bio(BIO_new_file(filename.data(), "w"));
  OSSL_ASSERT_PTR(bio);
  OSSL_ASSERT_FUNC(PEM_write_bio_PrivateKey(
      bio.get(), m_pkey.get(), passwd.empty() ? nullptr : EVP_aes_256_cbc(),
      (unsigned char *)passwd.data(), passwd.size(), nullptr, nullptr));
}

KeyPair KeyPair::Generator::load_private(std::string_view filename,
                                         std::string_view passwd) {
  BIO_ptr bio(BIO_new_file(filename.data(), "r"));
  OSSL_ASSERT_PTR(bio);

  EVP_PKEY_ptr key(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr,
                                           (void *)passwd.data()));

  OSSL_ASSERT_PTR(key);
  return std::move(KeyPair(std::move(key)));
}

KeyPair KeyPair::Generator::load_public(std::string_view filename) {
  BIO_ptr bio(BIO_new_file(filename.data(), "r"));
  OSSL_ASSERT_PTR(bio);

  EVP_PKEY_ptr key(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));
  OSSL_ASSERT_PTR(key);
  return std::move(KeyPair(std::move(key)));
}

bool KeyPair::has_public_key() const {
  int type = EVP_PKEY_get_base_id(get_EVP_PKEY());

  if (type == EVP_PKEY_RSA || type == EVP_PKEY_RSA_PSS) {
    BIGNUM *n = nullptr;
    if (EVP_PKEY_get_bn_param(get_EVP_PKEY(), OSSL_PKEY_PARAM_RSA_N, &n)) {
      BN_free(n);
      return true;
    }
    return false;
  } else { // EC / SM2 等
    size_t len = 0;
    if (EVP_PKEY_get_octet_string_param(get_EVP_PKEY(), OSSL_PKEY_PARAM_PUB_KEY,
                                        nullptr, 0, &len)) {
      return len > 0;
    }
    return false;
  }
}

bool KeyPair::has_private_key() const {
  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr);
  if (!ctx)
    return false;

  int ret = EVP_PKEY_check(ctx); // 只要返回 1，就表示有私钥且合法
  EVP_PKEY_CTX_free(ctx);
  return ret == 1;
}

bool KeyPair::match() const {

  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr);
  if (!ctx)
    return false;
  int ok = EVP_PKEY_pairwise_check(ctx);
  EVP_PKEY_CTX_free(ctx);
  return ok == 1;
}

// ==================== Crypto Implementation ====================

Bytes KeyPair::encrypt(const Bytes &plaintext) {
  if (plaintext.empty())
    return {};

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr));
  OSSL_ASSERT_PTR(ctx);
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt_init(ctx.get()));

  size_t outlen;
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt(ctx.get(), nullptr, &outlen,
                                    (const unsigned char *)plaintext.c_cptr(),
                                    plaintext.size()));

  Bytes ciphertext(outlen);
  OSSL_ASSERT_FUNC(EVP_PKEY_encrypt(
      ctx.get(), (unsigned char *)ciphertext.c_ptr(), &outlen,
      (const unsigned char *)plaintext.c_cptr(), plaintext.size()));
  ciphertext.resize(outlen);
  return std::move(ciphertext);
}

Bytes KeyPair::decrypt(const Bytes &ciphertext) {
  if (ciphertext.empty())
    return {};

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr));
  OSSL_ASSERT_PTR(ctx);

  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt_init(ctx.get()));

  size_t outlen;
  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt(ctx.get(), nullptr, &outlen,
                                    (const unsigned char *)ciphertext.c_cptr(),
                                    ciphertext.size()));

  Bytes plaintext(outlen);
  OSSL_ASSERT_FUNC(EVP_PKEY_decrypt(
      ctx.get(), (unsigned char *)plaintext.c_ptr(), &outlen,
      (const unsigned char *)ciphertext.c_cptr(), ciphertext.size()));
  plaintext.resize(outlen);
  return std::move(plaintext);
}

Bytes KeyPair::sign(const Bytes &message) {
  if (message.empty())
    return {};

  EVP_MD_CTX_ptr md_ctx(EVP_MD_CTX_new());
  OSSL_ASSERT_PTR(md_ctx);

  OSSL_ASSERT_FUNC(EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr,
                                      get_EVP_PKEY()));

  size_t siglen;
  OSSL_ASSERT_FUNC(EVP_DigestSign(md_ctx.get(), nullptr, &siglen,
                                  (const unsigned char *)message.c_cptr(),
                                  message.size()));
  Bytes signature(siglen);
  OSSL_ASSERT_FUNC(
      EVP_DigestSign(md_ctx.get(), (unsigned char *)signature.c_ptr(), &siglen,
                     (const unsigned char *)message.c_cptr(), message.size()));
  signature.resize(siglen);

  return std::move(signature);
}

Bytes x::crypto::KeyPair::sign_file(std::string_view filename) {
  File f(filename, File::OpenMode::BinaryReadWrite);

  EVP_MD_CTX_ptr md_ctx(EVP_MD_CTX_new());
  OSSL_ASSERT_PTR(md_ctx);
  OSSL_ASSERT_FUNC(EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr,
                                      get_EVP_PKEY()));
  Bytes buffer(4096);
  while (f.good()) {
    auto read_bytes = f.read_binary(buffer);
    if (read_bytes > 0)
      OSSL_ASSERT_FUNC(
          EVP_DigestSignUpdate(md_ctx.get(), buffer.c_cptr(), read_bytes));
  }
  size_t siglen;
  OSSL_ASSERT_FUNC(EVP_DigestSignFinal(md_ctx.get(), nullptr, &siglen));
  Bytes signature(siglen);
  OSSL_ASSERT_FUNC(
      EVP_DigestSignFinal(md_ctx.get(), signature.c_ui8(), &siglen));
  signature.resize(siglen);

  return std::move(signature);
}

bool KeyPair::verify(const Bytes &message, const Bytes &signature) {
  if (message.empty() || signature.empty())
    return false;

  EVP_MD_CTX_ptr md_ctx(EVP_MD_CTX_new());
  OSSL_ASSERT_PTR(md_ctx);

  OSSL_ASSERT_FUNC(EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(),
                                        nullptr, get_EVP_PKEY()));

  return EVP_DigestVerify(
             md_ctx.get(), (const unsigned char *)signature.c_cptr(),
             signature.size(), (const unsigned char *)message.c_cptr(),
             message.size()) == 1;
}

bool KeyPair::verify_file(std::string_view filename, const Bytes &signature) {
  File f(filename, File::OpenMode::BinaryReadWrite);

  EVP_MD_CTX_ptr md_ctx(EVP_MD_CTX_new());
  OSSL_ASSERT_PTR(md_ctx);

  OSSL_ASSERT_FUNC(EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(),
                                        nullptr, get_EVP_PKEY()));

  Bytes buffer(4096);
  while (f.good()) {
    auto read_bytes = f.read_binary(buffer);
    if (read_bytes > 0)
      OSSL_ASSERT_FUNC(
          EVP_DigestVerifyUpdate(md_ctx.get(), buffer.c_cptr(), read_bytes));
  }

  return EVP_DigestVerifyFinal(md_ctx.get(), signature.c_cui8(),
                               signature.size()) == 1;
}

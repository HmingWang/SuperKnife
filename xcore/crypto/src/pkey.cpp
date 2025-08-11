#include "pkey.h"
#include "openssl.h"
#include <cstddef>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string_view>

using namespace x::crypto;

PKey PKey::Generator::sm2() {
  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_SM2, nullptr));
  EVP_PKEY *key = nullptr;

  OSSL_ASSERT_PTR(ctx);
  OSSL_ASSERT_FUNC(EVP_PKEY_CTX_set1_id(ctx.get(), "1234567812345678", 16));
  OSSL_ASSERT_FUNC(EVP_PKEY_keygen_init(ctx.get()));
  OSSL_ASSERT_FUNC(EVP_PKEY_keygen(ctx.get(), &key));

  return std::move(PKey(EVP_PKEY_ptr(key)));
}

void PKey::save_public(std::string_view filename) {
  OSSL_ASSERT_PTR(m_pkey);
  BIO_ptr bio(BIO_new_file(filename.data(), "w"));
  OSSL_ASSERT_PTR(bio);
  OSSL_ASSERT_FUNC(PEM_write_bio_PUBKEY(bio.get(), m_pkey.get()));
}

void PKey::save_private(std::string_view filename, std::string_view passwd) {
  OSSL_ASSERT_PTR(m_pkey);
  BIO_ptr bio(BIO_new_file(filename.data(), "w"));
  OSSL_ASSERT_PTR(bio);
  OSSL_ASSERT_FUNC(PEM_write_bio_PrivateKey(
      bio.get(), m_pkey.get(), passwd.empty() ? nullptr : EVP_aes_256_cbc(),
      (unsigned char *)passwd.data(), passwd.size(), nullptr, nullptr));
}

PKey PKey::Generator::load_private(std::string_view filename,
                                   std::string_view passwd) {
  BIO_ptr bio(BIO_new_file(filename.data(), "r"));
  OSSL_ASSERT_PTR(bio);

  EVP_PKEY_ptr key(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr,
                                           (void *)passwd.data()));

  OSSL_ASSERT_PTR(key);
  return std::move(PKey(std::move(key)));
}

PKey PKey::Generator::load_public(std::string_view filename) {
  BIO_ptr bio(BIO_new_file(filename.data(), "r"));
  OSSL_ASSERT_PTR(bio);

  EVP_PKEY_ptr key(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));
  OSSL_ASSERT_PTR(key);
  return std::move(PKey(std::move(key)));
}

bool PKey::has_public_key() const {
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

bool PKey::has_private_key() const {
  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr);
  if (!ctx)
    return false;

  int ret = EVP_PKEY_check(ctx); // 只要返回 1，就表示有私钥且合法
  EVP_PKEY_CTX_free(ctx);
  return ret == 1;
}

bool PKey::match() const {

  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(get_EVP_PKEY(), nullptr);
  if (!ctx)
    return false;
  int ok = EVP_PKEY_pairwise_check(ctx);
  EVP_PKEY_CTX_free(ctx);
  return ok == 1;
}

#include "pkey.h"
#include "openssl.h"
#include <cstddef>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string_view>

namespace x::crypto {

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

void PKey::save_private(std::string_view filename,
                             std::string_view passwd) {
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

} // namespace x::crypto

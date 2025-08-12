#include "csr.h"
#include "pkey.h"
#include "openssl.h"
#include "utils.h"

namespace x::crypto {

CSR CSR::Generator::create(KeyPair &key, std::string_view subject) {

  X509_REQ_ptr req(X509_REQ_new());
  OSSL_ASSERT_PTR(req);
  OSSL_ASSERT_FUNC(X509_REQ_set_pubkey(req.get(), key.get_EVP_PKEY()));

  // 设置主题名称
  X509_NAME *name = X509_REQ_get_subject_name(req.get());
  Utils::set_subject_name(name,subject);

  // 签名请求
  int res=X509_REQ_sign(req.get(), key.get_EVP_PKEY(), EVP_sm3());
  OSSL_ASSERT_FUNC(res);

  return std::move(CSR(std::move(req)));
}

void CSR::save(std::string_view filename) {
  OSSL_ASSERT_PTR(m_req);
  BIO_ptr bio(BIO_new_file(filename.data(), "w"));
  OSSL_ASSERT_PTR(bio);
  OSSL_ASSERT_FUNC(PEM_write_bio_X509_REQ(bio.get(), m_req.get()));
}

} // namespace x::crypto

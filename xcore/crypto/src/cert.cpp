#include "cert.h"
#include "openssl.h"
#include "osslexception.h"


namespace x::crypto{


Cert Cert::Generator::create_self_signed(PKey &key, std::string_view subject, const EVP_MD *md,int vaild_days){
    X509_ptr cert(X509_new());
  OSSL_ASSERT_PTR(cert);
  OSSL_ASSERT_FUNC(X509_set_version(cert.get(), 2));
OSSL_ASSERT_FUNC(ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), std::rand()) );

OSSL_ASSERT_PTR(X509_gmtime_adj(X509_get_notBefore(cert.get()), 0));
OSSL_ASSERT_PTR(X509_gmtime_adj(X509_get_notAfter(cert.get()), vaild_days * 24 * 3600));

  // 设置主题名称
  X509_NAME *name = X509_get_subject_name(cert.get());
  for (auto &r : String(subject).split(",")) {
    auto p = r.split("=");
    auto k = p[0].trim();
    auto v = p[1].trim();
    OSSL_ASSERT_FUNC(X509_NAME_add_entry_by_txt(
        name, k.to_cstr(), MBSTRING_ASC,
        reinterpret_cast<const unsigned char *>(v.to_cstr()), -1, -1, 0));
  }
OSSL_ASSERT_FUNC(X509_set_issuer_name(cert.get(), name));
OSSL_ASSERT_FUNC(X509_set_pubkey(cert.get(), key.get_raw()));
OSSL_ASSERT_PTR(md);

  // 添加扩展
  addExtension(cert.get(), NID_basic_constraints, "critical,CA:TRUE");
  addExtension(cert.get(), NID_key_usage, "critical,keyCertSign,cRLSign");
  addExtension(cert.get(), NID_subject_key_identifier, "hash");

  if (!X509_sign(cert.get(), keyPair.getPrivateKey(), md)) {
    print_openssl_errors();
    cerr << "Failed to sign certificate with SM3" << endl;
    return false;
  }

  if (m_cert)
    X509_free(m_cert);
  m_cert = cert.release();

  return true;

}


}

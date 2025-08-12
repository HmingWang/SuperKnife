#include "cert.h"
#include "openssl.h"
#include "pkey.h"
#include "utils.h"
#include <openssl/x509.h>

using namespace x::crypto;

Cert Cert::Generator::create_self_signed(KeyPair &key, std::string_view subject,
                                         const EVP_MD *md, int vaild_days) {
  X509_ptr cert(X509_new());
  OSSL_ASSERT_PTR(cert);
  OSSL_ASSERT_FUNC(X509_set_version(cert.get(), 2));
  OSSL_ASSERT_FUNC(
      ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), std::rand()));

  OSSL_ASSERT_PTR(X509_gmtime_adj(X509_get_notBefore(cert.get()), 0));
  OSSL_ASSERT_PTR(
      X509_gmtime_adj(X509_get_notAfter(cert.get()), vaild_days * 24 * 3600));

  // 设置主题名称
  X509_NAME *name = X509_get_subject_name(cert.get());
  Utils::set_subject_name(name, subject);
  OSSL_ASSERT_FUNC(X509_set_issuer_name(cert.get(), name));
  OSSL_ASSERT_FUNC(X509_set_pubkey(cert.get(), key.get_EVP_PKEY()));
  OSSL_ASSERT_PTR(md);

  Utils::add_extensions(cert.get(), NID_basic_constraints, "critical,CA:TRUE");
  Utils::add_extensions(cert.get(), NID_key_usage,
                        "critical,keyCertSign,cRLSign");
  Utils::add_extensions(cert.get(), NID_subject_key_identifier, "hash");

  OSSL_ASSERT_FUNC(X509_sign(cert.get(), key.get_EVP_PKEY(), md));

  return std::move(Cert(std::move(cert)));
}

Cert Cert::Generator::create_from_csr(CSR &req, KeyPair &cakey, Cert &caCert,
                                      const EVP_MD *md, int vaild_days) {
  X509_ptr cert(X509_new());
  OSSL_ASSERT_PTR(cert);
  OSSL_ASSERT_FUNC(X509_set_version(cert.get(), 2));
  OSSL_ASSERT_FUNC(
      ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), std::rand()));
  OSSL_ASSERT_PTR(X509_gmtime_adj(X509_get_notBefore(cert.get()), 0));
  OSSL_ASSERT_PTR(
      X509_gmtime_adj(X509_get_notAfter(cert.get()), vaild_days * 24 * 3600));

  // 设置主题名称 (从请求中复制)
  OSSL_ASSERT_FUNC(X509_set_subject_name(
      cert.get(), X509_REQ_get_subject_name(req.get_X509_REQ())));

  // 设置颁发者名称 (CA的名称)
  OSSL_ASSERT_FUNC(X509_set_issuer_name(
      cert.get(), X509_get_subject_name(caCert.get_X509())));
  OSSL_ASSERT_FUNC(
      X509_set_pubkey(cert.get(), X509_REQ_get_pubkey(req.get_X509_REQ())));

  // 添加扩展
  Utils::add_extensions(cert.get(), NID_basic_constraints, "critical,CA:FALSE");
  Utils::add_extensions(cert.get(), NID_key_usage,
                        "critical,digitalSignature,keyEncipherment");
  Utils::add_extensions(cert.get(), NID_ext_key_usage, "serverAuth,clientAuth");
  Utils::add_extensions(cert.get(), NID_subject_key_identifier, "hash");
  Utils::add_extensions(cert.get(), NID_authority_key_identifier,
                        "keyid:always");

  // 签名
  OSSL_ASSERT_FUNC(X509_sign(cert.get(), cakey.get_EVP_PKEY(), md));

  return std::move(Cert(std::move(cert)));
}
Cert Cert::Generator::load(std::string_view filename) {
  BIO_ptr bio(BIO_new_file(filename.data(), "r"));
  OSSL_ASSERT_PTR(bio);

  X509_ptr cert(PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr));
  OSSL_ASSERT_PTR(cert);
  return std::move(Cert(std::move(cert)));
}
void Cert::save(std::string_view filename) const {
  BIO_ptr bio(BIO_new_file(filename.data(), "w"));
  OSSL_ASSERT_PTR(bio);

  OSSL_ASSERT_FUNC(PEM_write_bio_X509(bio.get(), get_X509()));
}
void Cert::print_info() const {
  print_basic();
  print_extensions();
  print_finger_points();
}
void Cert::print_basic() const {
  BIO_ptr bio(BIO_new(BIO_s_file()));
  BIO_set_fp(bio.get(), stdout, BIO_NOCLOSE);

  std::cout << "=== X.509 Certificate Basic Information ===" << std::endl;

  // 打印主题名称
  X509_NAME *subject = X509_get_subject_name(get_X509());
  std::cout << "Subject: ";
  X509_NAME_print_ex(bio.get(), subject, 0, XN_FLAG_ONELINE);
  std::cout << std::endl;

  // 打印颁发者名称
  X509_NAME *issuer = X509_get_issuer_name(get_X509());
  std::cout << "Issuer:  ";
  X509_NAME_print_ex(bio.get(), issuer, 0, XN_FLAG_ONELINE);
  std::cout << std::endl;

  // 打印序列号
  ASN1_INTEGER *serial = X509_get_serialNumber(get_X509());
  BIGNUM *bn = ASN1_INTEGER_to_BN(serial, nullptr);
  char *hex = BN_bn2hex(bn);
  std::cout << "Serial Number (hex): " << hex << std::endl;
  OPENSSL_free(hex);
  BN_free(bn);

  // 打印有效期
  std::cout << "Validity:" << std::endl;
  ASN1_TIME *notBefore = X509_get_notBefore(get_X509());
  std::cout << "  Not Before: ";
  ASN1_TIME_print(bio.get(), notBefore);
  std::cout << std::endl;

  ASN1_TIME *notAfter = X509_get_notAfter(get_X509());
  std::cout << "  Not After:  ";
  ASN1_TIME_print(bio.get(), notAfter);
  std::cout << std::endl;

  // 打印公钥信息
  std::cout << "Public Key: ";

  EVP_PKEY *pkey = X509_get_pubkey(get_X509());
  int nid = EVP_PKEY_get_id(pkey);
  if (nid != EVP_PKEY_KEYMGMT) { // 不是provider密钥
    const char *name = OBJ_nid2sn(nid);
  }
  const char *type_name = EVP_PKEY_get0_type_name(pkey);
  std::cout << type_name << " (";
  int bits = EVP_PKEY_bits(pkey);
  std::cout << bits << " bits)";
  std::cout << std::endl;

  // 打印签名算法
  std::cout << "Signature Algorithm: ";
  const char *sig_alg = OBJ_nid2ln(X509_get_signature_nid(get_X509()));
  std::cout << (sig_alg ? sig_alg : "Unknown") << std::endl;
}
void Cert::print_extensions() const {
  BIO_ptr bio(BIO_new(BIO_s_file()));
  BIO_set_fp(bio.get(), stdout, BIO_NOCLOSE);

  std::cout << "\n=== X.509 Certificate Extensions ===" << std::endl;

  int ext_count = X509_get_ext_count(get_X509());
  if (ext_count <= 0) {
    std::cout << "No extensions found." << std::endl;
    return;
  }

  for (int i = 0; i < ext_count; i++) {
    X509_EXTENSION *ext = X509_get_ext(get_X509(), i);
    ASN1_OBJECT *obj = X509_EXTENSION_get_object(ext);

    std::cout << std::left << std::setw(30) << OBJ_nid2ln(OBJ_obj2nid(obj));

    BIO_ptr ext_bio(BIO_new(BIO_s_mem()));
    if (X509V3_EXT_print(ext_bio.get(), ext, 0, 0)) {
      BUF_MEM *buf;
      BIO_get_mem_ptr(ext_bio.get(), &buf);
      std::string ext_value(buf->data, buf->length);
      // 替换换行符为空格
      std::replace(ext_value.begin(), ext_value.end(), '\n', ' ');
      std::cout << ext_value;
    } else {
      // 如果扩展无法打印，使用原始数据
      ASN1_STRING *data = X509_EXTENSION_get_data(ext);
      BIO_write(ext_bio.get(), data->data, data->length);
      BUF_MEM *buf;
      BIO_get_mem_ptr(ext_bio.get(), &buf);
      std::cout << "[DER encoded data] " << std::string(buf->data, buf->length);
    }
    std::cout << std::endl;
  }
}
void Cert::print_finger_points() const {

  std::cout << "\n=== X.509 Certificate Fingerprints ===" << std::endl;

  // 计算各种哈希算法的指纹
  const EVP_MD *digests[] = {EVP_sha1(), EVP_sha256(), EVP_sha512(), EVP_md5(),
                             EVP_sm3()};

  const char *digest_names[] = {"SHA-1", "SHA-256", "SHA-512", "MD5", "SM3"};

  unsigned char md[EVP_MAX_MD_SIZE];
  unsigned int n;

  for (size_t i = 0; i < sizeof(digests) / sizeof(digests[0]); i++) {
    if (X509_digest(get_X509(), digests[i], md, &n)) {
      std::cout << std::left << std::setw(10) << digest_names[i] << ": ";
      for (unsigned int j = 0; j < n; j++) {
        printf("%02X", md[j]);
        if (j != n - 1)
          printf(":");
      }
      std::cout << std::endl;
    }
  }
}

KeyPair Cert::get_public_key() const {
  return std::move(KeyPair(std::move(EVP_PKEY_ptr(X509_get_pubkey(get_X509())))));
}

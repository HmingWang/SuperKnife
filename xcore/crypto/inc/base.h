#pragma once
// ssl 基类

#include "headers.h"
#include "openssl.h"

class CertificateException : public std::runtime_error {
public:
  explicit CertificateException(const std::string &msg)
      : std::runtime_error(msg) {}
};

class Base {
protected:
  // 自定义删除器用于智能指针管理OpenSSL资源
  struct EVP_PKEY_Deleter {
    void operator()(EVP_PKEY *p) { EVP_PKEY_free(p); }
  };
  struct X509_Deleter {
    void operator()(X509 *p) { X509_free(p); }
  };
  struct BIO_Deleter {
    void operator()(BIO *p) { BIO_free(p); }
  };
  struct X509_REQ_Deleter {
    void operator()(X509_REQ *p) { X509_REQ_free(p); }
  };
  struct X509_NAME_Deleter {
    void operator()(X509_NAME *p) { X509_NAME_free(p); }
  };
  struct EC_GROUP_Deleter {
    void operator()(EC_GROUP *p) { EC_GROUP_free(p); }
  };

  using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;
  using X509_ptr = std::unique_ptr<X509, X509_Deleter>;
  using BIO_ptr = std::unique_ptr<BIO, BIO_Deleter>;
  using X509_REQ_ptr = std::unique_ptr<X509_REQ, X509_REQ_Deleter>;
  using X509_NAME_ptr = std::unique_ptr<X509_NAME, X509_NAME_Deleter>;

  using EC_GROUP_ptr = std::unique_ptr<EC_GROUP, EC_GROUP_Deleter>;

  void print_openssl_errors();

  bool setSubjectFromString(X509_NAME *name, const std::string &subjectStr);
  std::vector<std::pair<std::string, std::string>>
  parseSubjectString(const std::string &subject);
};

class KeyPair : public Base {
public:
  KeyPair() : m_privateKey(nullptr), m_publicKey(nullptr) {}
  ~KeyPair() {
    if (m_privateKey)
      EVP_PKEY_free(m_privateKey);
    if (m_publicKey)
      EVP_PKEY_free(m_publicKey);
  }

  virtual bool generateKeyPair() = 0;

  bool savePrivateKey(const std::string &filename,
                      const std::string &passphrase = "");
  bool savePublicKey(const std::string &filename);
  bool loadPrivateKey(const std::string &filename,
                      const std::string &passphrase = "");
  bool loadPublicKey(const std::string &filename);
  EVP_PKEY *getPrivateKey() const { return m_privateKey; }
  EVP_PKEY *getPublicKey() const { return m_publicKey; }

protected:
  EVP_PKEY *m_privateKey;
  EVP_PKEY *m_publicKey;
};

class CertReq : public Base {
public:
  CertReq() : m_req(nullptr) {}
  ~CertReq() {
    if (m_req)
      X509_REQ_free(m_req);
  }
  X509_REQ *getCertificateRequest() const { return m_req; };
  bool createCertificateRequest(KeyPair &keyPair, const std::string &subject);
  bool saveCertificateRequest(const std::string &file);

protected:
  X509_REQ *m_req;
};

class Cert : public Base {
public:
  Cert() : m_cert(nullptr) {}
  Cert(X509 *cert) : m_cert(cert) {}
  ~Cert() {
    if (m_cert)
      X509_free(m_cert);
  }

  X509 *getX509() const { return m_cert; }
  EVP_PKEY *getPublicKey() const;

  bool createSelfSigned(KeyPair &keyPair, const EVP_MD *md,
                        const std::string &subject, int validDays = 365);
  bool saveCertificate(const std::string &filename);
  bool loadCertificate(const std::string &filename);
  // 以自身为CA签发证书
  Cert signedCertificate(CertReq &req, KeyPair &caKeyPair, const EVP_MD *md,
                         int validDays = 365);
  void printCertificate();

protected:
  X509 *m_cert;

private:
  void printBasicInfo();
  void printExtensions();
  void printFingerPrints();
  void addExtension(int nid, const char *value);
};

class Crypto : public Base {};

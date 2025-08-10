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
  void addExtension(X509* cert,int nid, const char *value);
};

class Crypto : public Base {};


namespace x::crypto{


  class Base{
    public:

OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_PKEY);
OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_PKEY_CTX);
OPENSSL_UNIQ_PTR_WHIT_DELETER(X509);
OPENSSL_UNIQ_PTR_WHIT_DELETER(X509_REQ);
//OPENSSL_UNIQ_PTR_WHIT_DELETER(X509_NAME);//名字不要用unique_ptr;
OPENSSL_UNIQ_PTR_WHIT_DELETER(EC_GROUP);
OPENSSL_UNIQ_PTR_WHIT_DELETER(BIO);
OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_CIPHER);


  };


}

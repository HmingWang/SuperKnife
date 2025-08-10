#pragma once
#include "pkey.h"
#include "openssl.h"
#include "csr.h"
namespace x::crypto {



class Cert {

public:
  Cert(X509_ptr cer) : m_cert(std::move(cer)) {}
  class Generator {
  public:
    static Cert create_self_signed(PKey &key, std::string_view subject, const EVP_MD *md,
                                   int vaild_days = 365);
    static Cert create_from_csr(CSR &req, PKey &cakey, Cert &caCert,
                                int vaild_days = 365);
    static Cert load(std::string_view filename);
  };

  void save();
  void print_info();

private:
  X509_ptr m_cert;
};

} // namespace x::crypto

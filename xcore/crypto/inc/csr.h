#pragma once
#include "keypair.h"
#include "openssl.h"

namespace x::crypto {

class CSR {
public:
  class Generator {
  public:
    static CSR create(KeyPair &key, std::string_view subject);
  };

public:
  CSR(X509_REQ_ptr req) : m_req(std::move(req)) {}
  void save(std::string_view filename);

private:
  X509_REQ_ptr m_req;
};
} // namespace x::crypto

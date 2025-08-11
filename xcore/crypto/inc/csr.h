#pragma once
#include "openssl.h"
#include "pkey.h"

namespace x::crypto {

class CSR {
public:
  class Generator {
  public:
    static CSR create(PKey &key, std::string_view subject);
  };

public:
  CSR(X509_REQ_ptr req) : m_req(std::move(req)) {}
  void save(std::string_view filename);
  X509_REQ *get_X509_REQ() const { return m_req.get(); }

private:
  X509_REQ_ptr m_req;
};
} // namespace x::crypto

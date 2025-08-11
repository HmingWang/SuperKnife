#pragma once
#include "exceptions.h"
#include "headers.h"
#include "openssl.h"
#include <openssl/err.h>

namespace x::crypto {

class PKey {
public:
  class Generator {
  public:
    static PKey sm2();
    static PKey load_public(std::string_view filename);
    static PKey load_private(std::string_view filename,std::string_view passwd="");
  };

public:
  PKey(EVP_PKEY_ptr k) : m_pkey(std::move(k)) {}
  void save_public(std::string_view filename);
  void save_private(std::string_view filename, std::string_view passwd = "");
  EVP_PKEY* get_EVP_PKEY()const{return m_pkey.get();}


private:
  EVP_PKEY_ptr m_pkey; // uniq ptr with deleter
};

} // namespace x::crypto

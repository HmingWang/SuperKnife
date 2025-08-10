#pragma once
#include "exceptions.h"
#include "headers.h"
#include "openssl.h"
#include <openssl/err.h>

namespace x::crypto {

class KeyPair {
public:
  class Generator {
  public:
    static KeyPair sm2();
    static KeyPair load_public_from_file(std::string_view filename);
    static KeyPair load_private_from_file(std::string_view filename,std::string_view passwd="");
  };

public:
  KeyPair(EVP_PKEY_ptr k) : m_pkey(std::move(k)) {}
  void save_public(std::string_view filename);
  void save_private(std::string_view filename, std::string_view passwd = "");
  EVP_PKEY* get_raw()const{return m_pkey.get();}


private:
  EVP_PKEY_ptr m_pkey; // uniq ptr with deleter
};

} // namespace x::crypto

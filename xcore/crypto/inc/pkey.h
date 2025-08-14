#pragma once
#include "exceptions.h"
#include "headers.h"
#include "openssl.h"
#include <openssl/err.h>

namespace x::crypto
{

  class KeyPair
  {
  public:
    class Generator
    {
    public:
      static KeyPair sm2();
      static KeyPair load_public(std::string_view filename);
      static KeyPair load_private(std::string_view filename,
                                  std::string_view passwd = "");
    };

  public:
    KeyPair(EVP_PKEY_ptr k) : m_pkey(std::move(k)) {}
    void save_public(std::string_view filename);
    void save_private(std::string_view filename, std::string_view passwd = "");
    EVP_PKEY *get_EVP_PKEY() const { return m_pkey.get(); }
    bool has_private_key() const;
    bool has_public_key() const;
    bool match() const; // 检查密钥对匹配

    // 加密解密
    Bytes encrypt(const Bytes &plaintext);
    Bytes decrypt(const Bytes &ciphertext);
    Bytes sign(const Bytes &message);
    Bytes sign_file(std::string_view filename);
    bool verify(const Bytes &message, const Bytes &signature);
    bool verify(std::string_view filename,const Bytes& signature);

  private:
    EVP_PKEY_ptr m_pkey; // uniq ptr with deleter
  };

} // namespace x::crypto

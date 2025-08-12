#pragma once
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/provider.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <memory>
#include <openssl/core_names.h>

#include "osslexception.h"

#define OPENSSL_UNIQ_PTR_WHIT_DELETER(name)                                    \
                                                                               \
  struct name##_Deleter {                                                      \
    void operator()(name *p) { name##_free(p); }                               \
  };                                                                           \
  using name##_ptr = std::unique_ptr<name, name##_Deleter>

OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_PKEY);
OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_PKEY_CTX);
OPENSSL_UNIQ_PTR_WHIT_DELETER(X509);
OPENSSL_UNIQ_PTR_WHIT_DELETER(X509_REQ);
//OPENSSL_UNIQ_PTR_WHIT_DELETER(X509_NAME);//名字不要用unique_ptr;
OPENSSL_UNIQ_PTR_WHIT_DELETER(EC_GROUP);
OPENSSL_UNIQ_PTR_WHIT_DELETER(BIO);
OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_CIPHER);
OPENSSL_UNIQ_PTR_WHIT_DELETER(X509_EXTENSION);
OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_MD_CTX);
OPENSSL_UNIQ_PTR_WHIT_DELETER(EVP_MD);


#define OSSL_ASSERT_FUNC(func)                                                 \
  do {                                                                         \
    if (func <=0) {                                                        \
      throw CryptoException(                                                  \
          std::format("OpenSSL Assert Function Failed:{}:{}:{}", __FILE__,     \
                      __LINE__, #func));                                       \
    }                                                                          \
  } while (0)

#define OSSL_ASSERT_PTR(ptr)                                                   \
  do {                                                                         \
    if (ptr == nullptr) {                                                      \
      throw CryptoException(std::format("OpenSSL Assert Pointer Failed:{}",   \
                                         __FILE__, __LINE__, #ptr));           \
    }                                                                          \
  } while (0)

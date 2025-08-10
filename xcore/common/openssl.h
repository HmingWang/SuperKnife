#pragma once
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/provider.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include "exceptions.h"
#include "xstring.h"
#include <memory>

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

class OpenSSLException : public Exception {
public:
  OpenSSLException(std::string_view msg) : Exception(msg) {}
  virtual String what() noexcept {
    printErrorStack();
    return std::format("[OpenSSL异常]:{}", msg);
  }
  void printErrorStack() {
    unsigned long err_code;
    const char *file, *data, *func;
    int line, flags;

    while ((err_code = ERR_get_error_all(&file, &line, &func, &data, &flags))) {
      printf("Error: %s\n", ERR_error_string(err_code, NULL));
      printf("  Location: %s:%d:%s\n", file, line, func);
      if (data && (flags & ERR_TXT_STRING)) {
        printf("  Additional data: %s\n", data);
      }
    }
  }
};

#define OSSL_ASSERT_FUNC(func)                                                 \
  do {                                                                         \
    if (!(func == 1)) {                                                        \
      throw OpenSSLException(                                                  \
          std::format("OpenSSL Assert Function Failed:{}:{}:{}", __FILE__,     \
                      __LINE__, #func));                                       \
    }                                                                          \
  } while (0)

#define OSSL_ASSERT_PTR(ptr)                                                   \
  do {                                                                         \
    if (ptr == nullptr) {                                                      \
      throw OpenSSLException(std::format("OpenSSL Assert Pointer Failed:{}",   \
                                         __FILE__, __LINE__, #ptr));           \
    }                                                                          \
  } while (0)

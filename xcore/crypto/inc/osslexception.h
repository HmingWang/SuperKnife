#pragma once

#include "exceptions.h"
#include "openssl.h"

class CryptoException : public Exception {
public:
  CryptoException(std::string_view msg) : Exception(std::format("[OSSL异常]:",msg)) {}
  virtual const char* what() const noexcept override {
    printErrorStack();
    return msg;
  }
  void printErrorStack() const{
    std::cerr << "!!!Error:";
    unsigned long err_code;
    const char *file, *data, *func;
    int line, flags;

    while ((err_code = ERR_get_error_all(&file, &line, &func, &data, &flags))) {
      printf("OPENSSL异常: %s\n", ERR_error_string(err_code, NULL));
      printf("  位置: %s:%d:%s\n", file, line, func);
      if (data && (flags & ERR_TXT_STRING)) {
        printf("  附加信息: %s\n", data);
      }
    }
  }
};

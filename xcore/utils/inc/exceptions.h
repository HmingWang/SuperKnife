#pragma once
#include "headers.h"

#include "xstring.h"
#include <charconv>

class Exception : public std::exception {
public:
  Exception() = default;
  Exception(std::string_view m) : msg(std::format("[基类异常]:{}", m)) {}
  virtual const char *what() const noexcept override { return msg; }

protected:
  String msg;
};

class SystemException : public Exception {
public:
  SystemException(std::string_view msg)
      : Exception(std::format("[系统异常]:{}", msg)) {}
  virtual const char *what() const noexcept override { return msg; }
};

class DataException : public Exception {
public:
  DataException(std::string_view msg)
      : Exception(std::format("[数据异常]:{}", msg)) {}
  virtual const char *what() const noexcept override { return msg; }
};

class TransException : public Exception {
public:
  TransException(std::string_view msg)
      : Exception(std::format("[业务异常]:{}", msg)) {}
  virtual const char *what() const noexcept override { return msg; }
};

class StatusException : public Exception {
public:
  StatusException(std::string_view msg)
      : Exception(std::format("[状态异常]:{}", msg)) {}
  virtual const char *what() const noexcept override { return msg; }
};

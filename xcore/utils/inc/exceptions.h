#pragma once
#include "headers.h"

#include "xstring.h"

class Exception : public std::exception
{
public:
    Exception() = default;
    Exception(std::string_view m) : msg(m) {}
    virtual String what() noexcept
    {
        return std::format("[基类异常]:{}", msg);
    }

protected:
    String msg;
};

class SystemException : public Exception
{
public:
    SystemException(std::string_view msg) : Exception(msg) {}
    virtual String what() noexcept
    {
        return std::format("[系统异常]:{}", msg);
    }
};

class DataException : public Exception
{
public:
    DataException(std::string_view msg) : Exception(msg) {}
    virtual String what() noexcept
    {
        return std::format("[数据异常]:{}", msg);
    }
};

class TransException : public Exception
{
public:
    TransException(std::string_view msg) : Exception(msg) {}
    virtual String what() noexcept
    {
        return std::format("[业务异常]:{}", msg);
    }
};

class StatusException : public Exception
{
public:
    StatusException(std::string_view msg) : Exception(msg) {}
    virtual String what() noexcept
    {
        return std::format("[计费状态异常]:{}", msg);
    }
};
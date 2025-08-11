#pragma once

#include "bytes.h"
#include "container.h"

// 定义SQL宏，用于拼接SQL语句，首尾防御空格
#define SQL(...) " " #__VA_ARGS__ " "

/// @brief 字符串类
class String : public Container<std::string>
{

    inline static std::string blank = " \t\n\r\f\v";

public:
    class Utils
    {
    public:
        static String from_bytes(const Bytes &b);
    };

    // 构造函数
    String() = default;
    String(std::string_view s);
    String(const char *s);
    String(const String &s);
    String(const std::string &s);
    String(size_t size);
    String(char c, size_t size);

    String &operator=(const String &str);
    // String &operator=(std::string_view str);
    // String &operator=(const std::string &str);

    // 运算符重载
    friend std::ostream &operator<<(std::ostream &os, const String &str);
    bool operator!=(const String &str) const;
    bool operator!=(const char *str) const;
    bool operator==(const String &str) const;
    // bool operator==(std::string_view str) const;
    bool operator==(const char *str) const;
    String operator+(const String &str) const;
    String &operator+=(const String &str);

    // 隐式类型转换
    operator const char *() const;
    operator std::string_view() const;

    // 函数转换区
    Bytes to_bytes() const;
    const char *to_cstr() const;
    std::string to_string() const;
    int to_int() const;

    // String 类工具函数
    String &trim(std::string_view = blank);
    String &ltrim(std::string_view = blank);
    String &rtrim(std::string_view = blank);
    // 字符串分割，按字符串分割
    std::vector<String> split(std::string_view d, bool trim = false);
    // 大小写转换
    String &to_upper();
    String &to_lower();

    // 取子串
    String left(size_t n);
    String right(size_t n);
    String substr(size_t pos, size_t n);

    char &at(size_t i);

    struct Hash
    {
        std::size_t operator()(const String &prefix) const
        {
            return std::hash<std::string>{}(prefix.to_string());
        }
    };


};

/// @brief 定义 String 的格式化输出, 用于 std::format
template <>
struct std::formatter<String>
{
    std::formatter<std::string> formatter;
    constexpr auto parse(std::format_parse_context &context)
    {
        return formatter.parse(context);
    }

    auto format(const String &singleValue, std::format_context &context) const
    {
        return formatter.format(singleValue.to_string(), context);
    }
};

/// @brief 定义 String 的哈希函数, 用于 std::unordered_map
template <>
struct std::hash<String>
{
    std::size_t operator()(const String &s) const noexcept
    {
        return std::hash<std::string>{}(s.to_string());
    }
};

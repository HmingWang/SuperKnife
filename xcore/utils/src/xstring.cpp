#include "xstring.h"

String::String(std::string_view s) : Container(std::string(s))
{
}

String::String(const char *s) : Container(std::string(s))
{
}

String::String(const String &s) : Container(s)
{
}

String::String(const std::string &s) : Container(s)
{
}

String::String(size_t size) : Container(std::string(size, 0))
{
}

String::String(char c, size_t size) : Container(std::string(size, c))
{
}

// String::String(Bytes b) : String(from_bytes(b))
// {
// }

String &String::operator=(const String &str)
{
    _data = str.to_string();
    return *this;
}

String::operator const char *() const
{
    return _data.c_str();
}

String::operator std::string_view() const
{
    return std::string_view(_data);
}

// String::String(const char *s) : Container(std::string(s))
// {
// }

Bytes String::to_bytes() const
{
    Bytes b(_data.size());
    std::transform(_data.begin(), _data.end(), b.begin(), [](char c)
                   { return std::byte(c); });
    return std::move(b);
}

String String::Utils::from_bytes(const Bytes& b)
{
    String s(b.length());
    std::transform(b.cbegin(), b.cend(), s.begin(), [](std::byte c)
                   { return char(c); });
    return std::move(s);
}

const char *String::to_cstr() const
{
    
    return _data.c_str();
}

std::string String::to_string() const
{
    return _data;
}

int String::to_int() const
{
    return std::stoi(_data);
}

bool String::operator!=(const String &str) const
{
    return _data != str._data;
}

bool String::operator!=(const char *str) const
{
    return _data != str;
}

bool String::operator==(const String &str) const
{
    return _data == str._data;
}

// bool String::operator==(std::string_view str) const
// {
//     return _data == str;
// }

bool String::operator==(const char *str) const
{
    return _data == str;
}

String String::operator+(const String &str) const
{
    return _data + str.to_string();
}

String &String::operator+=(const String &str)
{
    _data = _data + str.to_string();
    return *this;
}

// String &String::operator=(std::string_view str)
// {
//     _data = str;
//     return *this;
// }

// String &String::operator=(const std::string &str)
// {
//     _data = str;
//     return *this;
// }

String &String::trim(std::string_view t)
{
    ltrim(t);
    rtrim(t);
    return *this;
}

String &String::ltrim(std::string_view t)
{
    _data.erase(0, _data.find_first_not_of(t));
    return *this;
}

String &String::rtrim(std::string_view t)
{
    _data.erase(_data.find_last_not_of(t) + 1);
    return *this;
}

std::vector<String> String::split(std::string_view delimiter, bool trim)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    String token;
    std::vector<String> res;

    while ((pos_end = _data.find(delimiter, pos_start)) != std::string::npos)
    {
        token = _data.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if (trim)
            token.trim();
        if (token.length() > 0)
            res.push_back(token);
    }

    token = _data.substr(pos_start);
    if (trim)
        token.trim();
    if (token.length() > 0)
        res.push_back(token);
    return res;
}

String &String::to_upper()
{
    std::transform(_data.begin(), _data.end(), _data.begin(), ::toupper);
    return *this;
}

String &String::to_lower()
{
    std::transform(_data.begin(), _data.end(), _data.begin(), ::tolower);
    return *this;
}

String String::left(size_t n)
{
    return std::move(std::string(_data.substr(0, n)));
}

String String::right(size_t n)
{
    return std::move(std::string(_data.substr(_data.length() - n)));
}

String String::substr(size_t pos, size_t n)
{
    return _data.substr(pos, n);
}

char &String::at(size_t i)
{
    return _data.at(i);
}

std::ostream &operator<<(std::ostream &os, const String &str)
{
    os << str._data.c_str();
    return os;
}

#include "bytes.h"
#include "xstring.h"
#include <sstream>

Bytes::Bytes(size_t size) : Container(std::vector<std::byte>(size))
{
}

Bytes::Bytes(std::byte *data, size_t size)
{
    _data = std::vector<std::byte>(data, data + size);
}

void Bytes::push_back(std::byte b)
{
    _data.push_back(b);
}

void Bytes::push_back(uint8 b)
{
    push_back(std::byte(b));
}

std::string Bytes::to_string()
{
    return String::Utils::from_bytes(*this).to_string();
}

std::string Bytes::to_hex_string()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (auto& byte : get())
    {
        oss << std::setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}

std::vector<unsigned char *> Bytes::to_uint8()
{
    return std::vector<unsigned char *>();
}

std::byte *Bytes::data()
{
    return reinterpret_cast<std::byte *>(_data.data());
}

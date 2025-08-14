#include "bytes.h"
#include "xstring.h"
#include <sstream>
#include "base64.h"

Bytes::Bytes(size_t size) : Container(std::vector<std::byte>(size))
{
}

Bytes::Bytes(std::byte *data, size_t size)
{
    _data = std::vector<std::byte>(data, data + size);
}

Bytes Bytes::from_b64(std::string_view b64)
{
    return std::move(x::crypto::Base64::Decoder::decode(b64));
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
    return String::from_bytes(*this).to_string();
}

std::string Bytes::to_hex_string()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (auto &byte : get())
    {
        oss << std::setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}

std::string Bytes::to_HEX_string()
{
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');
    for (auto &byte : get())
    {
        oss << std::setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}

std::string Bytes::to_b64()
{
    return std::move(x::crypto::Base64::Encoder::encode(*this));
}

#include "bytes.h"

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

void Bytes::push_back(byte b)
{
    push_back(std::byte(b));
}

std::byte *Bytes::data()
{
    return reinterpret_cast<std::byte *>(_data.data());
}


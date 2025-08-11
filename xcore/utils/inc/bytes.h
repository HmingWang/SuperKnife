#pragma once

#include "headers.h"
#include "container.h"


class Bytes : public Container<std::vector<std::byte>>
{
public:
    Bytes() = default;
    Bytes(size_t size);
    Bytes(std::byte *data, size_t size);

    void push_back(std::byte b);
    void push_back(uint8 b);
    std::string to_string();
    std::string to_hex_string();
    std::vector<unsigned char*> to_uint8();

    std::byte* data();
};
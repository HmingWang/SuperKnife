#pragma once

#include "headers.h"
#include "container.h"


class Bytes : public Container<std::vector<std::byte>>
{
public:
    Bytes() = default;
    Bytes(size_t size);
    Bytes(std::byte *data, size_t size);
    //列表初始化
    Bytes(std::initializer_list<uint8_t> list) {
        reserve(list.size());
        for (auto v : list) {
            push_back(v);
        }
    }

    void push_back(std::byte b);
    void push_back(uint8 b);
    std::string to_string();
    std::string to_hex_string();
};

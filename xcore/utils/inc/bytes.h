#pragma once

#include "headers.h"
#include "container.h"

class Bytes : public Container<std::vector<std::byte>>
{
public:
    // 构造函数
    explicit Bytes(size_t size);
    Bytes(std::byte *data, size_t size);
    // 列表初始化
    Bytes(std::initializer_list<uint8_t> list)
    {
        reserve(list.size());
        for (auto v : list)
        {
            push_back(v);
        }
    }

    // 静态函数
    
    /// @brief 从base64字符串转换
    /// @param b64 
    /// @return 
    static Bytes from_b64(std::string_view b64);

    void push_back(std::byte b);
    void push_back(uint8 b);
    std::string to_string();
    
    /// @brief 转为小写风格hex
    /// @return 
    std::string to_hex_string();
    /// @brief 转为大写风格hex
    /// @return 
    std::string to_HEX_string();
    /// @brief 转为base64字符串
    /// @return 
    std::string to_b64();
};

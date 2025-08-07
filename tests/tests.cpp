#include "catch2/catch_all.hpp"
#include "base64.h"
#include <string>
#include <iostream>

TEST_CASE("Catch2 Avaliable test")
{
    int i = 0;
    REQUIRE(i == 0);
}

TEST_CASE("Base64 Encode/Decode")
{
    std::string original = "我是中国人名的儿子我深情的爱着我的祖国和人民😘";
    std::string encoded = base64_encode(original,true);
    std::cout<<"Encoded: " << encoded << std::endl;
    std::string decoded = base64_decode(encoded);

    REQUIRE(original == decoded);
}

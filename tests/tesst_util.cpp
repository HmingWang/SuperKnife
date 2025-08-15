#include <catch2/catch_all.hpp>

#include "bytes.h"
#include <string>
#include <vector>
#include "xstring.h"

TEST_CASE("test bytes util")
{
    std::string a = "this is a test";
    std::vector<char> b(a.begin(), a.end());
    String c(a);
    std::cout << c.to_bytes().to_hex_string() << std::endl;
    std::cout << String("this is a test").to_bytes().to_hex_string() << std::endl;
    Bytes test = String("test a case").to_bytes();

    std::cout << test.to_b64();
    REQUIRE( Bytes::from_b64(test.to_b64())==test);
}
#include "catch2/catch_all.hpp"
#include "base64.h"
#include <string>
#include <iostream>
#include "sm4.h"
#include <vector>

using namespace std;

TEST_CASE("Catch2 Avaliable test")
{
    int i = 0;
    REQUIRE(i == 0);
}

TEST_CASE("Base64 Encode/Decode")
{
    std::string original = "我是中国人名的儿子我深情的爱着我的祖国和人民";
    std::cout<<original<<endl;
    std::string encoded_nl = base64_encode(original, true);
    std::string encoded = base64_encode(original, false);

    std::cout << "Encoded: " << encoded << std::endl;
    std::cout << "Encoded newline: " << encoded << std::endl;

    std::string decoded = base64_decode(encoded);
    std::string decoded_nl = base64_decode(encoded_nl);

    std::cout << "decode: " << decoded << std::endl;
    std::cout << "decode newline: " << decoded_nl << std::endl;

    REQUIRE(original == decoded);
    REQUIRE(original == decoded_nl);
}

TEST_CASE("SM4 ")
{
    std::vector<unsigned char> key = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

    std::vector<unsigned char> iv = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    std::string original = "test case";

    std::vector<unsigned char> keybin(key.begin(), key.end());
    std::vector<unsigned char> plaintext(original.begin(), original.end());
    std::vector<unsigned char> ciphertext = sm4_encrypt(plaintext, key, iv);
    std::cout << "Plaintext: " << original << std::endl;
    std::cout << "Ciphertext (hex): ";

    for (auto c : ciphertext)
    {
        printf("%02x", c);
    }
    std::cout << std::endl;

    // 解密
    std::vector<unsigned char> decrypted = sm4_decrypt(ciphertext, key, iv);
    std::string decrypted_str(decrypted.begin(), decrypted.end());

    std::cout << "Decrypted: " << decrypted_str << std::endl;

    REQUIRE(original == decrypted_str);
}
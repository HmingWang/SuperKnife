#include <catch2/catch_all.hpp>
#include "sm2.h"

TEST_CASE("test sm2")
{
    // 1. 生成密钥对和证书
    SM2KeyPair keyPair;
    if (!keyPair.generateKeyPair())
    {
        std::cerr << "Failed to generate SM2 key pair" << std::endl;
        return 1;
    }

    // 保存密钥
    if (!keyPair.savePrivateKey("sm2_private.pem", "mypassword"))
    {
        std::cerr << "Failed to save private key" << std::endl;
        return 1;
    }

    if (!keyPair.savePublicKey("sm2_public.pem"))
    {
        std::cerr << "Failed to save public key" << std::endl;
        return 1;
    }

    // 创建自签名证书
    SM2Certificate cert;
    if (!cert.createSelfSigned(keyPair, "/C=CN/O=My Organization/CN=SM2 Test Certificate"))
    {
        std::cerr << "Failed to create self-signed certificate" << std::endl;
        return 1;
    }

    if (!cert.saveCertificate("sm2_cert.pem"))
    {
        std::cerr << "Failed to save certificate" << std::endl;
        return 1;
    }

    std::cout << "SM2 key pair and certificate generated successfully." << std::endl;

    // 2. 加密解密演示
    SM2Crypto crypto;
    std::string message = "This is a secret message to be encrypted with SM2.";

    // 加密
    auto ciphertext = crypto.encrypt(keyPair.getPublicKey(),
                                     SM2Crypto::stringToVector(message));
    std::cout << "\nCiphertext (hex): " << SM2Crypto::toHex(ciphertext) << std::endl;

    // 解密
    auto decrypted = crypto.decrypt(keyPair.getPrivateKey(), ciphertext);
    std::cout << "Decrypted: " << SM2Crypto::vectorToString(decrypted) << std::endl;

    // 3. 签名验签演示
    auto signature = crypto.sign(keyPair.getPrivateKey(),
                                 SM2Crypto::stringToVector(message));
    std::cout << "\nSignature (hex): " << SM2Crypto::toHex(signature) << std::endl;

    // 验证签名
    bool verified = crypto.verify(keyPair.getPublicKey(),
                                  SM2Crypto::stringToVector(message),
                                  signature);
    std::cout << "Signature verification: " << (verified ? "SUCCESS" : "FAILED") << std::endl;

    // 测试篡改后的验签
    if (!signature.empty())
        signature[0] ^= 0x01; // 修改签名第一个字节
    verified = crypto.verify(keyPair.getPublicKey(),
                             SM2Crypto::stringToVector(message),
                             signature);
    std::cout << "Tampered signature verification: " << (verified ? "SUCCESS" : "FAILED") << std::endl;
    REQUIRE(verified);
}
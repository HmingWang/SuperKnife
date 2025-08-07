#include <catch2/catch_all.hpp>
#include "sm2.h"
#include <iostream>

using namespace std;

TEST_CASE("test sm2"){




        // 1. 生成密钥对
        SM2KeyPair keyPair;
        if (!keyPair.generateKeyPair()) {
            cerr << "Failed to generate SM2 key pair" << endl;
            return ;
        }

        // 保存密钥
        if (!keyPair.savePrivateKey("sm2_private.pem", "mypassword")) {
            cerr << "Failed to save private key" << endl;
            return ;
        }

        if (!keyPair.savePublicKey("sm2_public.pem")) {
            cerr << "Failed to save public key" << endl;
            return ;
        }

        // 2. 创建自签名证书
        SM2Certificate cert;
        string subject = "/C=CN/O=My Organization/CN=SM2 Test Certificate";
        if (!cert.createSelfSigned(keyPair, subject, 365)) {
            cerr << "Failed to create self-signed certificate" << endl;
            return ;
        }

        if (!cert.saveCertificate("sm2_cert.pem")) {
            cerr << "Failed to save certificate" << endl;
            return ;
        }

        cout << "SM2 key pair and certificate generated successfully." << endl;

        // 3. 测试加密解密
        string message = "Test message for SM2 encryption";
        auto ciphertext = SM2Crypto::encrypt(keyPair.getPublicKey(),
                                           SM2Crypto::stringToVector(message));
        cout << "Ciphertext: " << SM2Crypto::toHex(ciphertext) << endl;

        auto decrypted = SM2Crypto::decrypt(keyPair.getPrivateKey(), ciphertext);
        cout << "Decrypted: " << SM2Crypto::vectorToString(decrypted) << endl;

        // 4. 测试签名验签
        auto signature = SM2Crypto::sign(keyPair.getPrivateKey(),
                                       SM2Crypto::stringToVector(message));
        cout << "Signature: " << SM2Crypto::toHex(signature) << endl;

        bool verified = SM2Crypto::verify(keyPair.getPublicKey(),
                                        SM2Crypto::stringToVector(message),
                                        signature);
        cout << "Signature verified: " << boolalpha << verified << endl;
        REQUIRE(verified);

      }

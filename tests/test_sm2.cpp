#include <catch2/catch_all.hpp>
#include "sm2.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <openssl/applink.c>

using namespace std;

TEST_CASE("sm2 keypair")
{

    std::filesystem::path keyPath = "key.pem";
    std::filesystem::path pubPath = "pub.pem";

    std::filesystem::remove(keyPath);
    std::filesystem::remove(pubPath);

    SM2KeyPair key;
    key.generateKeyPair();
    key.savePrivateKey("key.pem");
    key.savePublicKey("pub.pem");

    ifstream file("key.pem");
    std::stringstream buffer;
    buffer << file.rdbuf(); // 读取整个文件到缓冲区
    std::string contents = buffer.str();
    std::cout << contents << std::endl;
    file.close();
    REQUIRE(contents.length() > 0);

    ifstream file1("pub.pem");
    std::stringstream buffer1;
    buffer1 << file1.rdbuf(); // 读取整个文件到缓冲区
    std::string contents1 = buffer1.str();
    std::cout << contents1 << std::endl;
    file1.close();
    REQUIRE(contents1.length() > 0);

    REQUIRE(key.loadPrivateKey("key.pem"));
    REQUIRE(key.loadPublicKey("pub.pem"));
}

TEST_CASE("sm2 cert")
{

    std::filesystem::path filePath = "cert.pem";
    std::filesystem::remove(filePath);

    SM2KeyPair key;
    REQUIRE(key.loadPrivateKey("key.pem"));
    REQUIRE(key.loadPublicKey("pub.pem"));

    SM2Certificate cert;
    REQUIRE(cert.createSelfSigned(key, "CN=Sample Cert, OU=R&D, O=Company Ltd., L=Dublin 4, S=Dublin, C=IE"));
    REQUIRE(cert.saveCertificate("cert.pem"));

    ifstream file("cert.pem");
    std::stringstream buffer;
    buffer << file.rdbuf(); // 读取整个文件到缓冲区
    std::string contents = buffer.str();
    std::cout << contents << std::endl;
    file.close();
    REQUIRE(contents.length() > 0);
    REQUIRE(cert.loadCertificate("cert.pem"));

    cert.printCertificate();
}
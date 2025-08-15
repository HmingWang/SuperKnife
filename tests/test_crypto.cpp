#include "catch2/catch_test_macros.hpp"
#include "exceptions.h"
#include "pkey.h"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <exception>
#include "cert.h"
#include "digest.h"
#include "cipher.h"

using namespace x::crypto;
using namespace std;

TEST_CASE("0.Prerequisite")
{
#ifdef _WIN32
  system("chcp 65001");
  system("del *.pem");
#endif
}

TEST_CASE("test keypair")
{

  try
  {
    KeyPair key = KeyPair::Generator::sm2();
    key.save_public("pub.pem");
    key.save_private("priv.pem", "123456");
    REQUIRE(key.has_private_key());
    REQUIRE(key.has_public_key());
    REQUIRE(key.match());

    KeyPair key_priv = KeyPair::Generator::load_private("priv.pem", "123456");
    KeyPair key_pub = KeyPair::Generator::load_public("pub.pem");
    REQUIRE(key_priv.has_private_key());
    REQUIRE(key_priv.has_public_key()); // 私钥带有公钥信息
    REQUIRE(key_pub.has_public_key());
    REQUIRE_FALSE(key_pub.has_private_key());
    REQUIRE(key_priv.match());
    REQUIRE_FALSE(key_pub.match()); // 只载入公钥无法匹配
  }
  catch (std::exception &e)
  {
    std::cerr << e.what();
    REQUIRE(false);
  }
}

TEST_CASE("sm2 encrypto/decrypto")
{

  KeyPair sm2pkey = KeyPair::Generator::sm2();

  std::string plaintxt = "我是中国人";
  Bytes txt = sm2pkey.encrypt(String(plaintxt).to_bytes());
  std::cout << "明文：" << plaintxt << std::endl;
  std::cout << "加密：" << txt.to_hex_string() << std::endl;
  Bytes txt2 = sm2pkey.decrypt(txt);
  std::cout << "解密：" << txt2.to_string() << std::endl;
  REQUIRE(plaintxt == txt2.to_string());
}

TEST_CASE("sm2 sign/verify")
{

  KeyPair sm2pkey = KeyPair::Generator::sm2();

  std::string plaintxt = "我是中国人";
  Bytes plain = String(plaintxt).to_bytes();
  Bytes txt = sm2pkey.sign(plain,Digest::Generator::sm3());
  std::cout << "明文：" << plaintxt << std::endl;
  std::cout << "Sign:" << txt.to_hex_string() << std::endl;
  REQUIRE(sm2pkey.verify(plain,Digest::Generator::sm3(), txt));
}

TEST_CASE("test cer")
{
  try
  {
    KeyPair ca = KeyPair::Generator::sm2();
    Cert caCer = Cert::Generator::create_self_signed(ca, "CN=test", EVP_sm3());
    KeyPair test = KeyPair::Generator::sm2();
    CSR csr = CSR::Generator::create(test, "CN=test1");

    Cert testCer = Cert::Generator::create_from_csr(csr, ca, caCer, EVP_sm3());
    caCer.save("ca.pem");
    testCer.save("test.pem");

    Cert caCerLoad = Cert::Generator::load("ca.pem");
    KeyPair caPUB = caCerLoad.get_public_key();

    std::string text = "我是小朋友";
    Bytes plain = String(text).to_bytes();
    Bytes sign = ca.sign(plain,Digest::Generator::sm3());
    std::cout << "[sign]:" << sign.to_hex_string() << endl;
    // REQUIRE(caPUB.has_public_key());
    REQUIRE(ca.verify(plain, Digest::Generator::sm3(),sign));
  }
  catch (Exception &e)
  {
    std::cerr << e.what();
    REQUIRE(false);
  }
}

TEST_CASE("test md5")
{
  system("echo 111> test.txt");
  Digest d = Digest::Generator::md5();
  Bytes bin = d.hash_file("test.txt");
  std::cout << "MD5 File:" << bin.to_hex_string() << endl;

  Bytes bin1 = d.hash({0x31, 0x31, 0x31, 0x0d, 0x0a});
  std::cout << "MD5 Char:" << bin1.to_hex_string() << endl;
  REQUIRE(bin.to_hex_string() == bin1.to_hex_string());
}

TEST_CASE("test sm3")
{
  system("echo 111> test.txt");
  Digest d = Digest::Generator::sm3();
  Bytes bin = d.hash_file("test.txt");
  std::cout << "SM3 File:" << bin.to_hex_string() << endl;

  Bytes bin1 = d.hash({0x31, 0x31, 0x31, 0x0d, 0x0a});
  std::cout << "SM3 Char:" << bin1.to_hex_string() << endl;
  REQUIRE(bin.to_hex_string() == bin1.to_hex_string());
}

TEST_CASE("test sha256")
{
  system("echo 111> test.txt");
  Digest d = Digest::Generator::sha256();
  Bytes bin = d.hash_file("test.txt");
  std::cout << "SHA256 File:" << bin.to_hex_string() << endl;

  Bytes bin1 = d.hash({0x31, 0x31, 0x31, 0x0d, 0x0a});
  std::cout << "SHA256 Char:" << bin1.to_hex_string() << endl;
  REQUIRE(bin.to_hex_string() == bin1.to_hex_string());
}

TEST_CASE("test sm4")
{
  Cipher sm4 = Cipher::Generator::maker(Cipher::Algorithm::SM4, Cipher::Mode::CBC);
  sm4.set_key_iv(String("1234567812345678").to_bytes());
  std::string plaintxt = "你好我是你啊手动阀阿斯顿发顺丰阿斯弗啊额外人情味若";
  auto ciphertxt = sm4.encrypto(String(plaintxt).to_bytes());
  std::cout << "明文：" << plaintxt << std::endl;
  std::cout << "密文：" << ciphertxt.to_hex_string() << std::endl;
  std::string dectxt = sm4.decrypto(ciphertxt).to_string();
  std::cout << "解密：" << dectxt << std::endl;
  REQUIRE(plaintxt == dectxt);
}

TEST_CASE("test sm4 gcm")
{
  Cipher sm4 = Cipher::Generator::maker(Cipher::Algorithm::SM4, Cipher::Mode::GCM);
  sm4.set_key_iv(String("1234567812345678").to_bytes());
  std::string plaintxt = "案说法的萨芬";
  String aad = "addasdfadf手动阀第三方o";
  Bytes tag(sm4.get_params().tag_size);
  auto ciphertxt = sm4.encrypto(String(plaintxt).to_bytes(), tag, aad.to_bytes());
  std::cout << "明文：" << plaintxt << std::endl;
  std::cout << "AAD:" << aad << std::endl;
  std::cout << "密文：" << ciphertxt.to_hex_string() << std::endl;
  std::cout << "TAG:" << tag.to_hex_string() << std::endl;
  std::string dectxt = sm4.decrypto(ciphertxt, tag, (aad).to_bytes()).to_string();
  std::cout << "解密：" << dectxt << std::endl;
  REQUIRE(plaintxt == dectxt);
}

TEST_CASE("test sm4 ecb")
{
  Cipher sm4 = Cipher::Generator::maker(Cipher::Algorithm::SM4, Cipher::Mode::ECB);
  sm4.set_key_iv(String("1234567812345678").to_bytes());
  std::string plaintxt = "案说法的萨芬";
  auto ciphertxt = sm4.encrypto(String(plaintxt).to_bytes());
  std::cout << "明文：" << plaintxt << std::endl;
  std::cout << "密文：" << ciphertxt.to_hex_string() << std::endl;
  std::string dectxt = sm4.decrypto(ciphertxt).to_string();
  std::cout << "解密：" << dectxt << std::endl;
  REQUIRE(plaintxt == dectxt);
}
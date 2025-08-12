#include "bytes.h"
#include "catch2/catch_test_macros.hpp"
#include "cert.h"
#include "csr.h"
#include "exceptions.h"
#include "pkey.h"
#include <catch2/catch_all.hpp>
#include <cstddef>
#include <openssl/evp.h>

using namespace x::crypto;
TEST_CASE("test cer") {
  try {
    KeyPair ca = KeyPair::Generator::sm2();
    Cert caCer = Cert::Generator::create_self_signed(ca, "CN=test", EVP_sm3());
    KeyPair test=KeyPair::Generator::sm2();
    CSR csr=CSR::Generator::create(test, "CN=test1");

    Cert testCer=Cert::Generator::create_from_csr(csr, ca, caCer, EVP_sm3());
    caCer.save("ca.pem");
    testCer.save("test.pem");

    Cert caCerLoad=Cert::Generator::load("ca.pem");
    KeyPair caPUB=caCerLoad.get_public_key();

    std::string text="我是小朋友";
    Bytes plain=String(text).to_bytes();
    Bytes sign=ca.sign(plain);
    std::cout<<"[sign]:"<<sign.to_hex_string();
    // REQUIRE(caPUB.has_public_key());
    REQUIRE(ca.verify(plain, sign));


  } catch (Exception &e) {
    std::cerr << e.what();
    REQUIRE(false);
  }

    system("rm *.pem");

}

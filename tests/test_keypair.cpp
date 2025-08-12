#include "catch2/catch_test_macros.hpp"
#include "exceptions.h"
#include "pkey.h"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <exception>

using namespace x::crypto;
TEST_CASE("test keypair") {

  system("rm *.pem");

  try {
    KeyPair key = KeyPair::Generator::sm2();
    key.save_public("pub.pem");
    key.save_private("priv.pem", "123456");
    REQUIRE(key.has_private_key());
    REQUIRE(key.has_public_key());
    REQUIRE(key.match());

    KeyPair key_priv = KeyPair::Generator::load_private("priv.pem", "123456");
    KeyPair key_pub = KeyPair::Generator::load_public("pub.pem");
    REQUIRE(key_priv.has_private_key());
    REQUIRE(key_priv.has_public_key());//私钥带有公钥信息
    REQUIRE(key_pub.has_public_key());
    REQUIRE_FALSE(key_pub.has_private_key());
    REQUIRE(key_priv.match());
    REQUIRE_FALSE(key_pub.match());//只载入公钥无法匹配

  } catch (std::exception &e) {
    std::cerr << e.what();
    REQUIRE(false);
  }
}

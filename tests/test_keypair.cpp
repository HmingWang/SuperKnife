#include "exceptions.h"
#include "keypair.h"
#include <catch2/catch_all.hpp>

using namespace x::crypto;
TEST_CASE("test keypair") {

    KeyPair key = KeyPair::Generator::sm2();
    key.save_public("pub.pem");
    key.save_private("priv.pem", "123456");

    KeyPair key_priv=KeyPair::Generator::load_private_from_file("priv.pem","123456");
    KeyPair key_pub=KeyPair::Generator::load_public_from_file("pub.pem");

    // REQUIRE_THROWS(key.loadPrivateKey("priv.pkey", "123"));
    // REQUIRE_NOTHROW(key.loadPrivateKey("priv.pkey", "123456"));
    // REQUIRE_NOTHROW(key.loadPublicKey("pub.pkey"));

}

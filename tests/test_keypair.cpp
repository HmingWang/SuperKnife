#include "exceptions.h"
#include "pkey.h"
#include <catch2/catch_all.hpp>
#include <cstdlib>

using namespace x::crypto;
TEST_CASE("test keypair") {

    system("rm *.pem");

    PKey key = PKey::Generator::sm2();
    key.save_public("pub.pem");
    key.save_private("priv.pem", "123456");

    PKey key_priv=PKey::Generator::load_private("priv.pem","123456");
    PKey key_pub=PKey::Generator::load_public("pub.pem");

    // REQUIRE_THROWS(key.loadPrivateKey("priv.pkey", "123"));
    // REQUIRE_NOTHROW(key.loadPrivateKey("priv.pkey", "123456"));
    // REQUIRE_NOTHROW(key.loadPublicKey("pub.pkey"));

}

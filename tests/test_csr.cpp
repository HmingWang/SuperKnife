#include "catch2/catch_test_macros.hpp"
#include "exceptions.h"
#include "pkey.h"
#include "csr.h"
#include <catch2/catch_all.hpp>

using namespace x::crypto;
TEST_CASE("test csr") {
  try{
    PKey key = PKey::Generator::sm2();
    CSR csr=CSR::Generator::create(key, "C=cn,CN=test,O=test unit");
    csr.save("csr111.pem");
  }catch(Exception&e){
    e.what();
    REQUIRE(false);
  }
}

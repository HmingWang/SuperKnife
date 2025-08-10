#include "exceptions.h"
#include "keypair.h"
#include "csr.h"
#include <catch2/catch_all.hpp>

using namespace x::crypto;
TEST_CASE("test csr") {
  try{
    KeyPair key = KeyPair::Generator::sm2();
    CSR csr=CSR::Generator::create(key, "C=cn,CN=test,O=test unit");
    csr.save("csr111.pem");
  }catch(Exception&e){
    e.what();
  }
}

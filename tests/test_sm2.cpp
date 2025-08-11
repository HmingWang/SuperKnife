#include "bytes.h"
#include "catch2/catch_all.hpp"
#include "catch2/catch_test_macros.hpp"
#include "pkey.h"
#include <iostream>

using namespace x::crypto;

TEST_CASE("sm2 encrypto/decrypto"){

  PKey sm2pkey=PKey::Generator::sm2();

  std::string plaintxt="我是中国人";
  Bytes txt= sm2pkey.encrypt(String(plaintxt).to_bytes());
  std::cout<<"明文："<<plaintxt<<std::endl;
  std::cout<<"加密："<<txt.to_hex_string()<<std::endl;
  Bytes txt2=sm2pkey.decrypt(txt);
  std::cout<<"解密："<<txt2.to_string()<<std::endl;
  REQUIRE(plaintxt==txt2.to_string());

}

TEST_CASE("sm2 sign/verify"){

  PKey sm2pkey=PKey::Generator::sm2();

  std::string plaintxt="我是中国人";
  Bytes txt= sm2pkey.sign(String(plaintxt).to_bytes());
  std::cout<<"明文："<<plaintxt<<std::endl;
  std::cout<<"Sign："<<txt.to_hex_string()<<std::endl;
  REQUIRE(sm2pkey.verify(String(plaintxt).to_bytes(),txt));

}

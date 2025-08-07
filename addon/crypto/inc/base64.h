#include<openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string>


std::string base64_encode(const std::string &in, const bool wrap = false);
std::string base64_decode(const std::string &in);

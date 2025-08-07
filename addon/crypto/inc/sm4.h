#pragma once
#include <vector>

std::vector<unsigned char> sm4_encrypt(const std::vector<unsigned char> &plaintext,
                                       const std::vector<unsigned char> &key,
                                       const std::vector<unsigned char> &iv);
                                       
std::vector<unsigned char> sm4_decrypt(const std::vector<unsigned char> &ciphertext,
                                       const std::vector<unsigned char> &key,
                                       const std::vector<unsigned char> &iv);
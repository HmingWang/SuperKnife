#pragma once

#include "base.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

class SM2KeyPair : public KeyPair
{
public:
    bool generateKeyPair() override;
};

class SM2Certificate : public Cert
{
public:
    bool createSelfSigned(SM2KeyPair &keyPair,
                          const std::string &subject,
                          int validDays = 365);
    SM2Certificate signedCertificate(CertReq &req, SM2KeyPair &caKeyPair, int validDays = 365);
};

class SM2Crypto : public Crypto
{
public:
    static std::vector<unsigned char> encrypt(EVP_PKEY *publicKey,
                                              const std::vector<unsigned char> &plaintext);
    static std::vector<unsigned char> decrypt(EVP_PKEY *privateKey,
                                              const std::vector<unsigned char> &ciphertext);
    static std::vector<unsigned char> sign(EVP_PKEY *privateKey,
                                           const std::vector<unsigned char> &message);
    static bool verify(EVP_PKEY *publicKey,
                       const std::vector<unsigned char> &message,
                       const std::vector<unsigned char> &signature);

    static std::vector<unsigned char> stringToVector(const std::string &str);
    static std::string vectorToString(const std::vector<unsigned char> &vec);
    static std::string toHex(const std::vector<unsigned char> &data);
};

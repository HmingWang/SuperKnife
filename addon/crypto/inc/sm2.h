#ifndef SM2_CRYPTO_H
#define SM2_CRYPTO_H

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <string>
#include <vector>
#include <memory>

class SM2KeyPair {
public:
    SM2KeyPair();
    ~SM2KeyPair();
    
    bool generateKeyPair();
    bool savePrivateKey(const std::string& filename, const std::string& passphrase = "");
    bool savePublicKey(const std::string& filename);
    bool loadPrivateKey(const std::string& filename, const std::string& passphrase = "");
    bool loadPublicKey(const std::string& filename);
    
    EVP_PKEY* getPrivateKey() const { return m_privateKey; }
    EVP_PKEY* getPublicKey() const { return m_publicKey; }

private:
    EVP_PKEY* m_privateKey;
    EVP_PKEY* m_publicKey;
};

class SM2Certificate {
public:
    SM2Certificate();
    ~SM2Certificate();
    
    bool createSelfSigned(SM2KeyPair& keyPair, 
                        const std::string& subject,
                        int validDays = 365);
    bool saveCertificate(const std::string& filename);
    bool loadCertificate(const std::string& filename);
    
    X509* getX509() const { return m_cert; }
    EVP_PKEY* getPublicKey() const;

private:
    X509* m_cert;
};

class SM2Crypto {
public:
    enum class CipherMode {
        CBC,
        ECB,
        GCM
    };

    SM2Crypto();
    ~SM2Crypto();
    
    // 加密/解密
    std::vector<unsigned char> encrypt(EVP_PKEY* publicKey, 
                                     const std::vector<unsigned char>& plaintext);
    std::vector<unsigned char> decrypt(EVP_PKEY* privateKey, 
                                     const std::vector<unsigned char>& ciphertext);
    
    // 签名/验签
    std::vector<unsigned char> sign(EVP_PKEY* privateKey, 
                                  const std::vector<unsigned char>& message);
    bool verify(EVP_PKEY* publicKey, 
               const std::vector<unsigned char>& message,
               const std::vector<unsigned char>& signature);
    
    // 辅助方法
    static std::vector<unsigned char> stringToVector(const std::string& str);
    static std::string vectorToString(const std::vector<unsigned char>& vec);
    static std::string toHex(const std::vector<unsigned char>& data);

private:
    void initialize();
    void cleanup();
    
    bool m_initialized;
};

#endif // SM2_CRYPTO_H
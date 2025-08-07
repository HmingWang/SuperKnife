#include "sm2.h"
#include <openssl/err.h>
#include <openssl/provider.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <sstream>
#include <stdexcept>

// 自定义删除器
struct EVP_PKEY_Deleter { void operator()(EVP_PKEY* p) { EVP_PKEY_free(p); } };
struct X509_Deleter { void operator()(X509* x) { X509_free(x); } };
struct BIO_Deleter { void operator()(BIO* b) { BIO_free(b); } };
struct OSSL_PROVIDER_Deleter { void operator()(OSSL_PROVIDER* p) { OSSL_PROVIDER_unload(p); } };

// 错误处理
void handleOpenSSLError(const std::string& message) {
    char errBuf[256];
    ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
    throw std::runtime_error(message + ": " + errBuf);
}

// ==================== SM2KeyPair 实现 ====================

SM2KeyPair::SM2KeyPair() : m_privateKey(nullptr), m_publicKey(nullptr) {}

SM2KeyPair::~SM2KeyPair() {
    if (m_privateKey) EVP_PKEY_free(m_privateKey);
    if (m_publicKey) EVP_PKEY_free(m_publicKey);
}

bool SM2KeyPair::generateKeyPair() {
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx) return false;

    if (EVP_PKEY_keygen_init(ctx.get()) <= 0) return false;
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_sm2) <= 0) return false;

    EVP_PKEY* key = nullptr;
    if (EVP_PKEY_keygen(ctx.get(), &key) <= 0) return false;

    // 生成成功后，设置成员变量
    if (m_privateKey) EVP_PKEY_free(m_privateKey);
    if (m_publicKey) EVP_PKEY_free(m_publicKey);
    
    m_privateKey = key;
    m_publicKey = EVP_PKEY_up_ref(key); // 增加引用计数
    
    return true;
}

bool SM2KeyPair::savePrivateKey(const std::string& filename, const std::string& passphrase) {
    if (!m_privateKey) return false;
    
    std::unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio) return false;
    
    const EVP_CIPHER* cipher = nullptr;
    if (!passphrase.empty()) {
        cipher = EVP_aes_256_cbc();
    }
    
    return PEM_write_bio_PrivateKey(bio.get(), m_privateKey, cipher, 
                                  (unsigned char*)passphrase.data(), 
                                  passphrase.size(), nullptr, nullptr) == 1;
}

bool SM2KeyPair::savePublicKey(const std::string& filename) {
    if (!m_publicKey) return false;
    
    std::unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio) return false;
    
    return PEM_write_bio_PUBKEY(bio.get(), m_publicKey) == 1;
}

bool SM2KeyPair::loadPrivateKey(const std::string& filename, const std::string& passphrase) {
    std::unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio) return false;
    
    EVP_PKEY* key = PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, 
                                          (void*)passphrase.data());
    if (!key) return false;
    
    if (m_privateKey) EVP_PKEY_free(m_privateKey);
    if (m_publicKey) EVP_PKEY_free(m_publicKey);
    
    m_privateKey = key;
    m_publicKey = EVP_PKEY_up_ref(key);
    
    return true;
}

bool SM2KeyPair::loadPublicKey(const std::string& filename) {
    std::unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio) return false;
    
    EVP_PKEY* key = PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr);
    if (!key) return false;
    
    if (m_publicKey) EVP_PKEY_free(m_publicKey);
    m_publicKey = key;
    
    return true;
}

// ==================== SM2Certificate 实现 ====================

SM2Certificate::SM2Certificate() : m_cert(nullptr) {}

SM2Certificate::~SM2Certificate() {
    if (m_cert) X509_free(m_cert);
}

bool SM2Certificate::createSelfSigned(SM2KeyPair& keyPair, 
                                    const std::string& subject,
                                    int validDays) {
    std::unique_ptr<X509, X509_Deleter> cert(X509_new());
    if (!cert) return false;
    
    // 设置版本
    X509_set_version(cert.get(), 2); // X509v3
    
    // 设置序列号
    ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), 1);
    
    // 设置有效期
    X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
    X509_gmtime_adj(X509_get_notAfter(cert.get()), validDays * 24 * 3600);
    
    // 设置公钥
    X509_set_pubkey(cert.get(), keyPair.getPublicKey());
    
    // 设置名称
    X509_NAME* name = X509_get_subject_name(cert.get());
    
    // 简单解析subject (格式如 "/C=CN/O=Org/CN=Common Name")
    size_t start = 0;
    while (start < subject.size()) {
        size_t eqPos = subject.find('=', start);
        if (eqPos == std::string::npos) break;
        
        size_t nextSlash = subject.find('/', eqPos);
        if (nextSlash == std::string::npos) nextSlash = subject.size();
        
        std::string field = subject.substr(start, eqPos - start);
        std::string value = subject.substr(eqPos + 1, nextSlash - eqPos - 1);
        
        X509_NAME_add_entry_by_txt(name, field.c_str(), MBSTRING_ASC, 
                                 (const unsigned char*)value.c_str(), -1, -1, 0);
        
        start = nextSlash + 1;
    }
    
    X509_set_issuer_name(cert.get(), name);
    
    // 使用SM3withSM2签名
    if (!X509_sign(cert.get(), keyPair.getPrivateKey(), EVP_sm3())) 
        return false;
    
    if (m_cert) X509_free(m_cert);
    m_cert = cert.release();
    
    return true;
}

bool SM2Certificate::saveCertificate(const std::string& filename) {
    if (!m_cert) return false;
    
    std::unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio) return false;
    
    return PEM_write_bio_X509(bio.get(), m_cert) == 1;
}

bool SM2Certificate::loadCertificate(const std::string& filename) {
    std::unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio) return false;
    
    X509* cert = PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr);
    if (!cert) return false;
    
    if (m_cert) X509_free(m_cert);
    m_cert = cert;
    
    return true;
}

EVP_PKEY* SM2Certificate::getPublicKey() const {
    if (!m_cert) return nullptr;
    return X509_get_pubkey(m_cert);
}

// ==================== SM2Crypto 实现 ====================

SM2Crypto::SM2Crypto() : m_initialized(false) {
    initialize();
}

SM2Crypto::~SM2Crypto() {
    cleanup();
}

void SM2Crypto::initialize() {
    if (m_initialized) return;
    
    // 加载OpenSSL提供者
    std::unique_ptr<OSSL_PROVIDER, OSSL_PROVIDER_Deleter> default_provider(
        OSSL_PROVIDER_load(nullptr, "default"));
    if (!default_provider) {
        handleOpenSSLError("Failed to load default provider");
    }
    
    std::unique_ptr<OSSL_PROVIDER, OSSL_PROVIDER_Deleter> legacy_provider(
        OSSL_PROVIDER_load(nullptr, "legacy"));
    // legacy提供者加载失败不一定是错误
    
    m_initialized = true;
}

void SM2Crypto::cleanup() {
    if (!m_initialized) return;
    
    EVP_cleanup();
    ERR_free_strings();
    m_initialized = false;
}

std::vector<unsigned char> SM2Crypto::encrypt(EVP_PKEY* publicKey, 
                                            const std::vector<unsigned char>& plaintext) {
    if (!publicKey || plaintext.empty()) return {};
    
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(publicKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx) handleOpenSSLError("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_encrypt_init(ctx.get()) <= 0)
        handleOpenSSLError("Failed to initialize encryption");

    // 获取输出缓冲区大小
    size_t outlen;
    if (EVP_PKEY_encrypt(ctx.get(), nullptr, &outlen, plaintext.data(), plaintext.size()) <= 0)
        handleOpenSSLError("Failed to get output buffer size");

    std::vector<unsigned char> ciphertext(outlen);
    if (EVP_PKEY_encrypt(ctx.get(), ciphertext.data(), &outlen, plaintext.data(), plaintext.size()) <= 0)
        handleOpenSSLError("Failed to encrypt data");

    ciphertext.resize(outlen);
    return ciphertext;
}

std::vector<unsigned char> SM2Crypto::decrypt(EVP_PKEY* privateKey, 
                                            const std::vector<unsigned char>& ciphertext) {
    if (!privateKey || ciphertext.empty()) return {};
    
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(privateKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx) handleOpenSSLError("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_decrypt_init(ctx.get()) <= 0)
        handleOpenSSLError("Failed to initialize decryption");

    // 获取输出缓冲区大小
    size_t outlen;
    if (EVP_PKEY_decrypt(ctx.get(), nullptr, &outlen, ciphertext.data(), ciphertext.size()) <= 0)
        handleOpenSSLError("Failed to get output buffer size");

    std::vector<unsigned char> plaintext(outlen);
    if (EVP_PKEY_decrypt(ctx.get(), plaintext.data(), &outlen, ciphertext.data(), ciphertext.size()) <= 0)
        handleOpenSSLError("Failed to decrypt data");

    plaintext.resize(outlen);
    return plaintext;
}

std::vector<unsigned char> SM2Crypto::sign(EVP_PKEY* privateKey, 
                                         const std::vector<unsigned char>& message) {
    if (!privateKey || message.empty()) return {};
    
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx) handleOpenSSLError("Failed to create EVP_MD_CTX");

    // 使用SM3作为摘要算法
    if (EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr, privateKey) <= 0)
        handleOpenSSLError("Failed to initialize signing");

    // 获取签名长度
    size_t siglen;
    if (EVP_DigestSign(md_ctx.get(), nullptr, &siglen, message.data(), message.size()) <= 0)
        handleOpenSSLError("Failed to get signature length");

    std::vector<unsigned char> signature(siglen);
    if (EVP_DigestSign(md_ctx.get(), signature.data(), &siglen, message.data(), message.size()) <= 0)
        handleOpenSSLError("Failed to sign data");

    signature.resize(siglen);
    return signature;
}

bool SM2Crypto::verify(EVP_PKEY* publicKey, 
                      const std::vector<unsigned char>& message,
                      const std::vector<unsigned char>& signature) {
    if (!publicKey || message.empty() || signature.empty()) return false;
    
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx) handleOpenSSLError("Failed to create EVP_MD_CTX");

    // 使用SM3作为摘要算法
    if (EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr, publicKey) <= 0)
        handleOpenSSLError("Failed to initialize verification");

    return EVP_DigestVerify(md_ctx.get(), signature.data(), signature.size(), 
                           message.data(), message.size()) == 1;
}

std::vector<unsigned char> SM2Crypto::stringToVector(const std::string& str) {
    return std::vector<unsigned char>(str.begin(), str.end());
}

std::string SM2Crypto::vectorToString(const std::vector<unsigned char>& vec) {
    return std::string(vec.begin(), vec.end());
}

std::string SM2Crypto::toHex(const std::vector<unsigned char>& data) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned char byte : data) {
        oss << std::setw(2) << (int)byte;
    }
    return oss.str();
}
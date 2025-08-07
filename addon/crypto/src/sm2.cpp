#include "sm2.h"
#include <openssl/err.h>
#include <openssl/provider.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/obj_mac.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

// Custom deleters
struct EVP_PKEY_Deleter { void operator()(EVP_PKEY* p) { EVP_PKEY_free(p); } };
struct X509_Deleter { void operator()(X509* x) { X509_free(x); } };
struct BIO_Deleter { void operator()(BIO* b) { BIO_free(b); } };
struct OSSL_PROVIDER_Deleter { void operator()(OSSL_PROVIDER* p) { OSSL_PROVIDER_unload(p); } };

void handleOpenSSLError(const string& message) {
    char errBuf[256];
    ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
    throw runtime_error(message + ": " + errBuf);
}

// ==================== SM2KeyPair Implementation ====================

SM2KeyPair::SM2KeyPair() : m_privateKey(nullptr), m_publicKey(nullptr) {}

SM2KeyPair::~SM2KeyPair() {
    if (m_privateKey) EVP_PKEY_free(m_privateKey);
    if (m_publicKey) EVP_PKEY_free(m_publicKey);
}

bool SM2KeyPair::generateKeyPair() {
    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx) return false;

    if (EVP_PKEY_keygen_init(ctx.get()) <= 0) return false;
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_sm2) <= 0) return false;

    EVP_PKEY* key = nullptr;
    if (EVP_PKEY_keygen(ctx.get(), &key) <= 0) return false;

    if (m_privateKey) EVP_PKEY_free(m_privateKey);
    if (m_publicKey) EVP_PKEY_free(m_publicKey);

    m_privateKey = key;
    if (EVP_PKEY_up_ref(key) != 1) {
        EVP_PKEY_free(key);
        m_privateKey = nullptr;
        return false;
    }
    m_publicKey = key;

    return true;
}

bool SM2KeyPair::savePrivateKey(const string& filename, const string& passphrase) {
    if (!m_privateKey) return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio) return false;

    const EVP_CIPHER* cipher = !passphrase.empty() ? EVP_aes_256_cbc() : nullptr;

    return PEM_write_bio_PrivateKey(bio.get(), m_privateKey, cipher,
                                  (unsigned char*)passphrase.data(),
                                  passphrase.size(), nullptr, nullptr) == 1;
}

bool SM2KeyPair::savePublicKey(const string& filename) {
    if (!m_publicKey) return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio) return false;

    return PEM_write_bio_PUBKEY(bio.get(), m_publicKey) == 1;
}

bool SM2KeyPair::loadPrivateKey(const string& filename, const string& passphrase) {
    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio) return false;

    EVP_PKEY* key = PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr,
                                          (void*)passphrase.data());
    if (!key) return false;

    if (m_privateKey) EVP_PKEY_free(m_privateKey);
    if (m_publicKey) EVP_PKEY_free(m_publicKey);

    m_privateKey = key;
    if (EVP_PKEY_up_ref(key) != 1) {
        EVP_PKEY_free(key);
        m_privateKey = nullptr;
        return false;
    }
    m_publicKey = key;

    return true;
}

bool SM2KeyPair::loadPublicKey(const string& filename) {
    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio) return false;

    EVP_PKEY* key = PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr);
    if (!key) return false;

    if (m_publicKey) EVP_PKEY_free(m_publicKey);
    m_publicKey = key;

    return true;
}

// ==================== SM2Certificate Implementation ====================

SM2Certificate::SM2Certificate() : m_cert(nullptr) {}

SM2Certificate::~SM2Certificate() {
    if (m_cert) X509_free(m_cert);
}

std::vector<std::pair<std::string, std::string>> SM2Certificate::parseSubjectString(const std::string& subject) {
    std::vector<std::pair<std::string, std::string>> result;

    if (subject.empty()) {
        std::cerr << "Empty subject string" << std::endl;
        return result;
    }

    size_t start = (subject[0] == '/') ? 1 : 0;

    while (start < subject.size()) {
        size_t eq_pos = subject.find('=', start);
        if (eq_pos == std::string::npos || eq_pos == subject.size() - 1) {
            std::cerr << "Invalid subject format: missing value after '='" << std::endl;
            break;
        }

        size_t next_slash = subject.find('/', eq_pos);
        if (next_slash == std::string::npos) {
            next_slash = subject.size();
        }

        std::string field = subject.substr(start, eq_pos - start);
        std::string value = subject.substr(eq_pos + 1, next_slash - eq_pos - 1);

        // 验证字段和值非空
        if (field.empty() || value.empty()) {
            std::cerr << "Empty field or value in subject" << std::endl;
            start = next_slash + 1;
            continue;
        }

        // 转换标准字段名
        static const std::map<std::string, std::string> fieldMap = {
            {"C", "countryName"},
            {"ST", "stateOrProvinceName"},
            {"L", "localityName"},
            {"O", "organizationName"},
            {"OU", "organizationalUnitName"},
            {"CN", "commonName"}
        };

        auto it = fieldMap.find(field);
        if (it != fieldMap.end()) {
            field = it->second;
        }

        result.emplace_back(field, value);
        start = next_slash + 1;
    }

    return result;
}

bool SM2Certificate::setSubjectFromString(X509* cert, const std::string& subjectStr) {
    if (!cert) {
        std::cerr << "Invalid certificate pointer" << std::endl;
        return false;
    }

    X509_NAME* name = X509_get_subject_name(cert);
    if (!name) {
        std::cerr << "Failed to get subject name from certificate" << std::endl;
        return false;
    }

    auto subjectFields = parseSubjectString(subjectStr);
    if (subjectFields.empty()) {
        std::cerr << "No valid fields found in subject" << std::endl;
        return false;
    }

    // 必须包含的必填字段
  static const std::set<std::string> requiredFields = {
    "countryName", "organizationName", "commonName"
};

    std::set<std::string> presentFields;
    for (const auto& [field, value] : subjectFields) {
        presentFields.insert(field);

        if (!X509_NAME_add_entry_by_txt(name, field.c_str(), MBSTRING_ASC,
                                      reinterpret_cast<const unsigned char*>(value.c_str()),
                                      -1, -1, 0)) {
            std::cerr << "Failed to add field: " << field << "=" << value << std::endl;
            ERR_print_errors_fp(stderr);
            return false;
        }
    }

    // 检查必填字段
    for (const auto& reqField : requiredFields) {
        if (presentFields.find(reqField) == presentFields.end()) {
            std::cerr << "Missing required field in subject: " << reqField << std::endl;
            return false;
        }
    }

    return true;
}

bool SM2Certificate::createSelfSigned(SM2KeyPair& keyPair,
                                    const string& subject,
                                    int validDays) {
    if (!keyPair.getPrivateKey() || !keyPair.getPublicKey()) {
        cerr << "Invalid key pair" << endl;
        return false;
    }

        // 验证密钥类型
    if (EVP_PKEY_base_id(keyPair.getPrivateKey()) != EVP_PKEY_EC) {
        std::cerr << "Private key is not EC key" << std::endl;
        return false;
    }

     // 验证SM2曲线
    const EC_KEY* ec_key = EVP_PKEY_get0_EC_KEY(keyPair.getPrivateKey());
    int nid = EC_GROUP_get_curve_name(EC_KEY_get0_group(ec_key));
    if (nid != NID_sm2) {
        std::cerr << "Key is not using SM2 curve (NID: " << nid << ")" << std::endl;
        return false;
    }

    unique_ptr<X509, X509_Deleter> cert(X509_new());
    if (!cert) {
        cerr << "Failed to create X509 structure" << endl;
        return false;
    }

    if (!X509_set_version(cert.get(), 2)) {
        cerr << "Failed to set version" << endl;
        return false;
    }

    if (!ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), 1)) {
        cerr << "Failed to set serial number" << endl;
        return false;
    }

    if (!X509_gmtime_adj(X509_get_notBefore(cert.get()), 0) ||
        !X509_gmtime_adj(X509_get_notAfter(cert.get()), validDays * 24 * 3600)) {
        cerr << "Failed to set validity period" << endl;
        return false;
    }

    if (!X509_set_pubkey(cert.get(), keyPair.getPublicKey())) {
        cerr << "Failed to set public key" << endl;
        return false;
    }

    if (!setSubjectFromString(cert.get(), subject)) {
        ERR_print_errors_fp(stderr);
        cerr << "Failed to set subject: " << subject << endl;
        return false;
    }

    if (!X509_set_issuer_name(cert.get(), X509_get_subject_name(cert.get()))) {
        cerr << "Failed to set issuer name" << endl;
        return false;
    }

        // 签名前验证SM3可用性
    const EVP_MD* md = EVP_sm3();
    if (!md) {
        std::cerr << "SM3 digest not available" << std::endl;
        return false;
    }

    if (!X509_sign(cert.get(), keyPair.getPrivateKey(), EVP_sm3())) {
        ERR_print_errors_fp(stderr);
        cerr << "Failed to sign certificate with SM3" << endl;
        return false;
    }

    if (m_cert) X509_free(m_cert);
    m_cert = cert.release();
    return true;
}

bool SM2Certificate::saveCertificate(const string& filename) {
    if (!m_cert) return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio) return false;

    return PEM_write_bio_X509(bio.get(), m_cert) == 1;
}

bool SM2Certificate::loadCertificate(const string& filename) {
    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
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

// ==================== SM2Crypto Implementation ====================

vector<unsigned char> SM2Crypto::encrypt(EVP_PKEY* publicKey,
                                       const vector<unsigned char>& plaintext) {
    if (!publicKey || plaintext.empty()) return {};

    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(publicKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx) handleOpenSSLError("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_encrypt_init(ctx.get()) <= 0)
        handleOpenSSLError("Failed to initialize encryption");

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx.get(), nullptr, &outlen, plaintext.data(), plaintext.size()) <= 0)
        handleOpenSSLError("Failed to get output buffer size");

    vector<unsigned char> ciphertext(outlen);
    if (EVP_PKEY_encrypt(ctx.get(), ciphertext.data(), &outlen, plaintext.data(), plaintext.size()) <= 0)
        handleOpenSSLError("Failed to encrypt data");

    ciphertext.resize(outlen);
    return ciphertext;
}

vector<unsigned char> SM2Crypto::decrypt(EVP_PKEY* privateKey,
                                       const vector<unsigned char>& ciphertext) {
    if (!privateKey || ciphertext.empty()) return {};

    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(privateKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx) handleOpenSSLError("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_decrypt_init(ctx.get()) <= 0)
        handleOpenSSLError("Failed to initialize decryption");

    size_t outlen;
    if (EVP_PKEY_decrypt(ctx.get(), nullptr, &outlen, ciphertext.data(), ciphertext.size()) <= 0)
        handleOpenSSLError("Failed to get output buffer size");

    vector<unsigned char> plaintext(outlen);
    if (EVP_PKEY_decrypt(ctx.get(), plaintext.data(), &outlen, ciphertext.data(), ciphertext.size()) <= 0)
        handleOpenSSLError("Failed to decrypt data");

    plaintext.resize(outlen);
    return plaintext;
}

vector<unsigned char> SM2Crypto::sign(EVP_PKEY* privateKey,
                                    const vector<unsigned char>& message) {
    if (!privateKey || message.empty()) return {};

    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx) handleOpenSSLError("Failed to create EVP_MD_CTX");

    if (EVP_DigestSignInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr, privateKey) <= 0)
        handleOpenSSLError("Failed to initialize signing");

    size_t siglen;
    if (EVP_DigestSign(md_ctx.get(), nullptr, &siglen, message.data(), message.size()) <= 0)
        handleOpenSSLError("Failed to get signature length");

    vector<unsigned char> signature(siglen);
    if (EVP_DigestSign(md_ctx.get(), signature.data(), &siglen, message.data(), message.size()) <= 0)
        handleOpenSSLError("Failed to sign data");

    signature.resize(siglen);
    return signature;
}

bool SM2Crypto::verify(EVP_PKEY* publicKey,
                      const vector<unsigned char>& message,
                      const vector<unsigned char>& signature) {
    if (!publicKey || message.empty() || signature.empty()) return false;

    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx) handleOpenSSLError("Failed to create EVP_MD_CTX");

    if (EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr, publicKey) <= 0)
        handleOpenSSLError("Failed to initialize verification");

    return EVP_DigestVerify(md_ctx.get(), signature.data(), signature.size(),
                           message.data(), message.size()) == 1;
}

vector<unsigned char> SM2Crypto::stringToVector(const string& str) {
    return vector<unsigned char>(str.begin(), str.end());
}

string SM2Crypto::vectorToString(const vector<unsigned char>& vec) {
    return string(vec.begin(), vec.end());
}

string SM2Crypto::toHex(const vector<unsigned char>& data) {
    ostringstream oss;
    oss << hex << setfill('0');
    for (unsigned char byte : data) {
        oss << setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}

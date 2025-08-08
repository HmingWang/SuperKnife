#include "sm2.h"
#include <openssl/err.h>
#include <openssl/provider.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/obj_mac.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using namespace std;

void print_openssl_errors()
{
    BIO *bio = BIO_new(BIO_s_mem());
    ERR_print_errors(bio);
    char *buf;
    long len = BIO_get_mem_data(bio, &buf);
    std::cerr << "OpenSSL errors:\n"
              << std::string(buf, len) << std::endl;
    BIO_free(bio);
}

// Custom deleters
struct EVP_PKEY_Deleter
{
    void operator()(EVP_PKEY *p) { EVP_PKEY_free(p); }
};
struct X509_Deleter
{
    void operator()(X509 *x) { X509_free(x); }
};
struct BIO_Deleter
{
    void operator()(BIO *b) { BIO_free(b); }
};
using BIO_ptr = std::unique_ptr<BIO, BIO_Deleter>;
struct OSSL_PROVIDER_Deleter
{
    void operator()(OSSL_PROVIDER *p) { OSSL_PROVIDER_unload(p); }
};

void handleOpenSSLError(const string &message)
{
    char errBuf[256];
    ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
    throw runtime_error(message + ": " + errBuf);
}

// ==================== SM2KeyPair Implementation ====================

SM2KeyPair::SM2KeyPair() : m_privateKey(nullptr), m_publicKey(nullptr) {}

SM2KeyPair::~SM2KeyPair()
{
    if (m_privateKey)
        EVP_PKEY_free(m_privateKey);
    if (m_publicKey)
        EVP_PKEY_free(m_publicKey);
}

bool SM2KeyPair::generateKeyPair()
{
    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx)
        return false;

    if (EVP_PKEY_keygen_init(ctx.get()) <= 0)
        return false;
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_sm2) <= 0)
        return false;

    EVP_PKEY *key = nullptr;
    if (EVP_PKEY_keygen(ctx.get(), &key) <= 0)
        return false;

    if (m_privateKey)
        EVP_PKEY_free(m_privateKey);
    if (m_publicKey)
        EVP_PKEY_free(m_publicKey);

    m_privateKey = key;
    if (EVP_PKEY_up_ref(key) != 1)
    {
        EVP_PKEY_free(key);
        m_privateKey = nullptr;
        return false;
    }
    m_publicKey = key;

    return true;
}

bool SM2KeyPair::savePrivateKey(const string &filename, const string &passphrase)
{
    if (!m_privateKey)
        return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio)
        return false;

    const EVP_CIPHER *cipher = !passphrase.empty() ? EVP_aes_256_cbc() : nullptr;

    return PEM_write_bio_PrivateKey(bio.get(), m_privateKey, cipher,
                                    (unsigned char *)passphrase.data(),
                                    passphrase.size(), nullptr, nullptr) == 1;
}

bool SM2KeyPair::savePublicKey(const string &filename)
{
    if (!m_publicKey)
        return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio)
        return false;

    return PEM_write_bio_PUBKEY(bio.get(), m_publicKey) == 1;
}

bool SM2KeyPair::loadPrivateKey(const string &filename, const string &passphrase)
{
    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio)
        return false;

    EVP_PKEY *key = PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr,
                                            (void *)passphrase.data());
    if (!key)
        return false;

    if (m_privateKey)
        EVP_PKEY_free(m_privateKey);
    if (m_publicKey)
        EVP_PKEY_free(m_publicKey);

    m_privateKey = key;
    if (EVP_PKEY_up_ref(key) != 1)
    {
        EVP_PKEY_free(key);
        m_privateKey = nullptr;
        return false;
    }
    m_publicKey = key;

    return true;
}

bool SM2KeyPair::loadPublicKey(const string &filename)
{
    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio)
        return false;

    EVP_PKEY *key = PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr);
    if (!key)
        return false;

    if (m_publicKey)
        EVP_PKEY_free(m_publicKey);
    m_publicKey = key;

    return true;
}

// ==================== SM2Certificate Implementation ====================

SM2Certificate::SM2Certificate() : m_cert(nullptr) {}

SM2Certificate::~SM2Certificate()
{
    if (m_cert)
        X509_free(m_cert);
}

vector<pair<string, string>> SM2Certificate::parseSubjectString(const string &subject)
{
    vector<pair<string, string>> result;

    size_t start = 0;
    if (!subject.empty() && subject[0] == ',')
    {
        start = 1;
    }

    while (start < subject.size())
    {
        size_t eq_pos = subject.find('=', start);
        if (eq_pos == string::npos)
            break;

        size_t next_slash = subject.find('/', eq_pos);
        if (next_slash == string::npos)
        {
            next_slash = subject.size();
        }

        string field = subject.substr(start, eq_pos - start);
        string value = subject.substr(eq_pos + 1, next_slash - eq_pos - 1);

        static const map<string, string> fieldMap = {
            {"C", "countryName"},
            {"ST", "stateOrProvinceName"},
            {"L", "localityName"},
            {"O", "organizationName"},
            {"OU", "organizationalUnitName"},
            {"CN", "commonName"}};

        auto it = fieldMap.find(field);
        if (it != fieldMap.end())
        {
            field = it->second;
        }

        result.emplace_back(field, value);
        start = next_slash + 1;
    }

    return result;
}

bool SM2Certificate::setSubjectFromString(X509 *cert, const string &subjectStr)
{
    if (!cert)
        return false;

    X509_NAME *name = X509_get_subject_name(cert);
    if (!name)
        return false;

    // if (X509_NAME_add_entry_by_txt(name, "countryName", MBSTRING_ASC,
    //                                (const unsigned char *)"CN", -1, -1, 0) != 1)
    // {
    //     return false;
    // }

    // if (X509_NAME_add_entry_by_txt(name, "organizationName", MBSTRING_ASC,
    //                                (const unsigned char *)"SM2 Organization", -1, -1, 0) != 1)
    // {
    //     return false;
    // }

    // if (X509_NAME_add_entry_by_txt(name, "commonName", MBSTRING_ASC,
    //                                (const unsigned char *)"test ca", -1, -1, 0) != 1)
    // {
    //     return false;
    // }
    // return true;

    auto subjectFields = parseSubjectString(subjectStr);
    if (subjectFields.empty())
        return false;

    for (const auto &[field, value] : subjectFields)
    {
        if (!X509_NAME_add_entry_by_txt(name, field.c_str(), MBSTRING_ASC,
                                        reinterpret_cast<const unsigned char *>(value.c_str()),
                                        -1, -1, 0))
        {
            return false;
        }
    }

    return true;
}

bool SM2Certificate::createSelfSigned(SM2KeyPair &keyPair,
                                      const string &subject,
                                      int validDays)
{

    if (!keyPair.getPrivateKey() || !keyPair.getPublicKey())
    {
        cerr << "Invalid key pair" << endl;
        return false;
    }

    unique_ptr<X509, X509_Deleter> cert(X509_new());
    if (!cert)
    {
        cerr << "Failed to create X509 structure" << endl;
        return false;
    }

    if (!X509_set_version(cert.get(), 2))
    {
        cerr << "Failed to set version" << endl;
        return false;
    }

    if (!ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), 1))
    {
        cerr << "Failed to set serial number" << endl;
        return false;
    }

    if (!X509_gmtime_adj(X509_get_notBefore(cert.get()), 0) ||
        !X509_gmtime_adj(X509_get_notAfter(cert.get()), validDays * 24 * 3600))
    {
        cerr << "Failed to set validity period" << endl;
        return false;
    }

    if (!X509_set_pubkey(cert.get(), keyPair.getPublicKey()))
    {
        cerr << "Failed to set public key" << endl;
        return false;
    }

    if (!setSubjectFromString(cert.get(), subject))
    {
        cerr << "Failed to set subject: " << subject << endl;
        return false;
    }

    if (!X509_set_issuer_name(cert.get(), X509_get_subject_name(cert.get())))
    {
        cerr << "Failed to set issuer name" << endl;
        return false;
    }

    // 签名前验证SM3可用性
    const EVP_MD *md = EVP_sm3();
    if (!md)
    {
        std::cerr << "SM3 digest not available" << std::endl;
        return false;
    }

    // 添加扩展
    addExtension(cert.get(), NID_basic_constraints, "critical,CA:TRUE");
    addExtension(cert.get(), NID_key_usage, "critical,keyCertSign,cRLSign");
    addExtension(cert.get(), NID_subject_key_identifier, "hash");

    if (!X509_sign(cert.get(), keyPair.getPrivateKey(), EVP_sm3()))
    {
        print_openssl_errors();
        cerr << "Failed to sign certificate with SM3" << endl;
        return false;
    }

    if (m_cert)
        X509_free(m_cert);
    m_cert = cert.release();
    return true;
}

void SM2Certificate::addExtension(X509 *cert_, int nid, const char *value)
{
    X509V3_CTX ctx;
    X509V3_set_ctx_nodb(&ctx);
    X509V3_set_ctx(&ctx, cert_, cert_, nullptr, nullptr, 0);

    X509_EXTENSION *ex = X509V3_EXT_conf_nid(nullptr, &ctx, nid, value);
    if (!ex)
    {
        cerr << "Unable to create extension" << endl;
    }

    if (X509_add_ext(cert_, ex, -1) != 1)
    {
        X509_EXTENSION_free(ex);
        cerr << "Unable to add extension to certificate" << endl;
    }

    X509_EXTENSION_free(ex);
}

void SM2Certificate::printBasicInfo()
{
    if (!m_cert)
        return;

    BIO_ptr bio(BIO_new(BIO_s_file()));
    BIO_set_fp(bio.get(), stdout, BIO_NOCLOSE);

    std::cout << "=== X.509 Certificate Basic Information ===" << std::endl;

    // 打印主题名称
    X509_NAME *subject = X509_get_subject_name(m_cert);
    std::cout << "Subject: ";
    X509_NAME_print_ex(bio.get(), subject, 0, XN_FLAG_ONELINE);
    std::cout << std::endl;

    // 打印颁发者名称
    X509_NAME *issuer = X509_get_issuer_name(m_cert);
    std::cout << "Issuer:  ";
    X509_NAME_print_ex(bio.get(), issuer, 0, XN_FLAG_ONELINE);
    std::cout << std::endl;

    // 打印序列号
    ASN1_INTEGER *serial = X509_get_serialNumber(m_cert);
    BIGNUM *bn = ASN1_INTEGER_to_BN(serial, nullptr);
    char *hex = BN_bn2hex(bn);
    std::cout << "Serial Number (hex): " << hex << std::endl;
    OPENSSL_free(hex);
    BN_free(bn);

    // 打印有效期
    std::cout << "Validity:" << std::endl;
    ASN1_TIME *notBefore = X509_get_notBefore(m_cert);
    std::cout << "  Not Before: ";
    ASN1_TIME_print(bio.get(), notBefore);
    std::cout << std::endl;

    ASN1_TIME *notAfter = X509_get_notAfter(m_cert);
    std::cout << "  Not After:  ";
    ASN1_TIME_print(bio.get(), notAfter);
    std::cout << std::endl;

    // 打印公钥信息
    std::cout << "Public Key: ";

    EVP_PKEY *pkey = getPublicKey();
    int nid = EVP_PKEY_get_id(pkey);
    if (nid != EVP_PKEY_KEYMGMT)
    { // 不是provider密钥
        const char *name = OBJ_nid2sn(nid);
    }
    const char* type_name = EVP_PKEY_get0_type_name(pkey);
    std::cout << type_name <<" (";
    int bits = EVP_PKEY_bits(pkey);
    std::cout<<bits<<" bits)";
    std::cout << std::endl;

    // 打印签名算法
    std::cout << "Signature Algorithm: ";
    const char *sig_alg = OBJ_nid2ln(X509_get_signature_nid(m_cert));
    std::cout << (sig_alg ? sig_alg : "Unknown") << std::endl;
}

void SM2Certificate::printExtensions()
{
    if (!m_cert)
        return;

    BIO_ptr bio(BIO_new(BIO_s_file()));
    BIO_set_fp(bio.get(), stdout, BIO_NOCLOSE);

    std::cout << "\n=== X.509 Certificate Extensions ===" << std::endl;

    int ext_count = X509_get_ext_count(m_cert);
    if (ext_count <= 0)
    {
        std::cout << "No extensions found." << std::endl;
        return;
    }

    for (int i = 0; i < ext_count; i++)
    {
        X509_EXTENSION *ext = X509_get_ext(m_cert, i);
        ASN1_OBJECT *obj = X509_EXTENSION_get_object(ext);

        std::cout << std::left << std::setw(30) << OBJ_nid2ln(OBJ_obj2nid(obj));

        BIO_ptr ext_bio(BIO_new(BIO_s_mem()));
        if (X509V3_EXT_print(ext_bio.get(), ext, 0, 0))
        {
            BUF_MEM *buf;
            BIO_get_mem_ptr(ext_bio.get(), &buf);
            std::string ext_value(buf->data, buf->length);
            // 替换换行符为空格
            std::replace(ext_value.begin(), ext_value.end(), '\n', ' ');
            std::cout << ext_value;
        }
        else
        {
            // 如果扩展无法打印，使用原始数据
            ASN1_STRING *data = X509_EXTENSION_get_data(ext);
            BIO_write(ext_bio.get(), data->data, data->length);
            BUF_MEM *buf;
            BIO_get_mem_ptr(ext_bio.get(), &buf);
            std::cout << "[DER encoded data] " << std::string(buf->data, buf->length);
        }
        std::cout << std::endl;
    }
}

void SM2Certificate::printFingerPrints()
{
    if (!m_cert)
        return;

    std::cout << "\n=== X.509 Certificate Fingerprints ===" << std::endl;

    // 计算各种哈希算法的指纹
    const EVP_MD *digests[] = {
        EVP_sha1(),
        EVP_sha256(),
        EVP_sha512(),
        EVP_md5(),
        EVP_sm3()};

    const char *digest_names[] = {
        "SHA-1",
        "SHA-256",
        "SHA-512",
        "MD5",
        "SM3"};

    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int n;

    for (size_t i = 0; i < sizeof(digests) / sizeof(digests[0]); i++)
    {
        if (X509_digest(m_cert, digests[i], md, &n))
        {
            std::cout << std::left << std::setw(10) << digest_names[i] << ": ";
            for (unsigned int j = 0; j < n; j++)
            {
                printf("%02X", md[j]);
                if (j != n - 1)
                    printf(":");
            }
            std::cout << std::endl;
        }
    }
}

bool SM2Certificate::saveCertificate(const string &filename)
{
    if (!m_cert)
        return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio)
        return false;

    return PEM_write_bio_X509(bio.get(), m_cert) == 1;
}

bool SM2Certificate::loadCertificate(const string &filename)
{
    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "r"));
    if (!bio)
        return false;

    X509 *cert = PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr);
    if (!cert)
        return false;

    if (m_cert)
        X509_free(m_cert);
    m_cert = cert;

    return true;
}

void SM2Certificate::printCertificate()
{
    if (!m_cert)
    {
        std::cerr << "Invalid certificate pointer" << std::endl;
        return;
    }

    printBasicInfo();
    printExtensions();
    printFingerPrints();
}

EVP_PKEY *SM2Certificate::getPublicKey() const
{
    if (!m_cert)
        return nullptr;
    return X509_get_pubkey(m_cert);
}

// ==================== SM2Crypto Implementation ====================

vector<unsigned char> SM2Crypto::encrypt(EVP_PKEY *publicKey,
                                         const vector<unsigned char> &plaintext)
{
    if (!publicKey || plaintext.empty())
        return {};

    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(publicKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx)
        handleOpenSSLError("Failed to create EVP_PKEY_CTX");

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

vector<unsigned char> SM2Crypto::decrypt(EVP_PKEY *privateKey,
                                         const vector<unsigned char> &ciphertext)
{
    if (!privateKey || ciphertext.empty())
        return {};

    unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(privateKey, nullptr), &EVP_PKEY_CTX_free);
    if (!ctx)
        handleOpenSSLError("Failed to create EVP_PKEY_CTX");

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

vector<unsigned char> SM2Crypto::sign(EVP_PKEY *privateKey,
                                      const vector<unsigned char> &message)
{
    if (!privateKey || message.empty())
        return {};

    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx)
        handleOpenSSLError("Failed to create EVP_MD_CTX");

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

bool SM2Crypto::verify(EVP_PKEY *publicKey,
                       const vector<unsigned char> &message,
                       const vector<unsigned char> &signature)
{
    if (!publicKey || message.empty() || signature.empty())
        return false;

    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!md_ctx)
        handleOpenSSLError("Failed to create EVP_MD_CTX");

    if (EVP_DigestVerifyInit(md_ctx.get(), nullptr, EVP_sm3(), nullptr, publicKey) <= 0)
        handleOpenSSLError("Failed to initialize verification");

    return EVP_DigestVerify(md_ctx.get(), signature.data(), signature.size(),
                            message.data(), message.size()) == 1;
}

vector<unsigned char> SM2Crypto::stringToVector(const string &str)
{
    return vector<unsigned char>(str.begin(), str.end());
}

string SM2Crypto::vectorToString(const vector<unsigned char> &vec)
{
    return string(vec.begin(), vec.end());
}

string SM2Crypto::toHex(const vector<unsigned char> &data)
{
    ostringstream oss;
    oss << hex << setfill('0');
    for (unsigned char byte : data)
    {
        oss << setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}
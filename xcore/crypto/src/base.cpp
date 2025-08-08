#include "base.h"

using namespace std;

bool KeyPair::savePrivateKey(const string &filename, const string &passphrase)
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

bool KeyPair::savePublicKey(const string &filename)
{
    if (!m_publicKey)
        return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio)
        return false;

    return PEM_write_bio_PUBKEY(bio.get(), m_publicKey) == 1;
}

bool KeyPair::loadPrivateKey(const string &filename, const string &passphrase)
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

bool KeyPair::loadPublicKey(const string &filename)
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

void Base::print_openssl_errors()
{
    BIO *bio = BIO_new(BIO_s_mem());
    ERR_print_errors(bio);
    char *buf;
    long len = BIO_get_mem_data(bio, &buf);
    std::cerr << "OpenSSL errors:\n"
              << std::string(buf, len) << std::endl;
    BIO_free(bio);
}

bool Base::setSubjectFromString(X509_NAME *name, const string &subjectStr)
{
    if (!name)
        return false;

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

vector<pair<string, string>> Base::parseSubjectString(const string &subject)
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

bool Cert::createSelfSigned(KeyPair &keyPair, const EVP_MD *md,
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

    if (!setSubjectFromString(X509_get_subject_name(cert.get()), subject))
    {
        cerr << "Failed to set subject: " << subject << endl;
        return false;
    }

    if (!X509_set_issuer_name(cert.get(), X509_get_subject_name(cert.get())))
    {
        cerr << "Failed to set issuer name" << endl;
        return false;
    }

    if (!md)
    {
        std::cerr << "digest not available" << std::endl;
        return false;
    }

    if (!X509_sign(cert.get(), keyPair.getPrivateKey(), EVP_sm3()))
    {
        print_openssl_errors();
        cerr << "Failed to sign certificate with SM3" << endl;
        return false;
    }

    if (m_cert)
        X509_free(m_cert);
    m_cert = cert.release();

    // 添加扩展
    addExtension(NID_basic_constraints, "critical,CA:TRUE");
    addExtension(NID_key_usage, "critical,keyCertSign,cRLSign");
    addExtension(NID_subject_key_identifier, "hash");

    return true;
}

bool Cert::saveCertificate(const string &filename)
{
    if (!m_cert)
        return false;

    unique_ptr<BIO, BIO_Deleter> bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio)
        return false;

    return PEM_write_bio_X509(bio.get(), m_cert) == 1;
}

bool Cert::loadCertificate(const string &filename)
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

Cert Cert::signedCertificate(CertReq &req,KeyPair &caKeyPair,const EVP_MD* md, int validDays)
{
    X509_ptr cert_(X509_new());
    if (!cert_)
    {
        throw CertificateException("Unable to create X509 structure");
    }
    // 设置版本
    if (X509_set_version(cert_.get(), 2) != 1)
    {
        throw CertificateException("Unable to set certificate version");
    }

    // 设置序列号 (使用时间戳)
    if (ASN1_INTEGER_set(X509_get_serialNumber(cert_.get()), time(nullptr)) != 1)
    {
        throw CertificateException("Unable to set serial number");
    }

    // 设置有效期
    if (X509_gmtime_adj(X509_get_notBefore(cert_.get()), 0) == nullptr)
    {
        throw CertificateException("Unable to set notBefore time");
    }

    if (X509_gmtime_adj(X509_get_notAfter(cert_.get()), validDays * 24 * 3600) == nullptr)
    {
        throw CertificateException("Unable to set notAfter time");
    }

    // 设置主题名称 (从请求中复制)
    if (X509_set_subject_name(cert_.get(), X509_REQ_get_subject_name(req.getCertificateRequest())) != 1)
    {
        throw CertificateException("Unable to set subject name");
    }

    // 设置颁发者名称 (CA的名称)
    if (X509_set_issuer_name(cert_.get(), X509_get_subject_name(m_cert)) != 1)
    {
        throw CertificateException("Unable to set issuer name");
    }

    // 设置公钥
    EVP_PKEY *req_pubkey = X509_REQ_get_pubkey(req.getCertificateRequest());
    if (!req_pubkey)
    {
        throw CertificateException("Unable to get public key from request");
    }

    if (X509_set_pubkey(cert_.get(), req_pubkey) != 1)
    {
        EVP_PKEY_free(req_pubkey);
        throw CertificateException("Unable to set public key");
    }
    EVP_PKEY_free(req_pubkey);

    // 添加扩展
    addExtension(NID_basic_constraints, "critical,CA:FALSE");
    addExtension(NID_key_usage, "critical,digitalSignature,keyEncipherment");
    addExtension(NID_ext_key_usage, "serverAuth,clientAuth");
    addExtension(NID_subject_key_identifier, "hash");
    addExtension(NID_authority_key_identifier, "keyid:always");

    // 签名
    if (X509_sign(cert_.get(), caKeyPair.getPrivateKey(),md) <= 0)
    {
        throw CertificateException("Unable to sign certificate");
    }
}

void Cert::printCertificate()
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

EVP_PKEY *Cert::getPublicKey() const
{
    if (!m_cert)
        return nullptr;
    return X509_get_pubkey(m_cert);
}

void Cert::addExtension(int nid, const char *value)
{
    X509V3_CTX ctx;
    X509V3_set_ctx_nodb(&ctx);
    X509V3_set_ctx(&ctx, m_cert, m_cert, nullptr, nullptr, 0);

    X509_EXTENSION *ex = X509V3_EXT_conf_nid(nullptr, &ctx, nid, value);
    if (!ex)
    {
        cerr << "Unable to create extension" << endl;
    }

    if (X509_add_ext(m_cert, ex, -1) != 1)
    {
        X509_EXTENSION_free(ex);
        cerr << "Unable to add extension to certificate" << endl;
    }

    X509_EXTENSION_free(ex);
}

void Cert::printBasicInfo()
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
    const char *type_name = EVP_PKEY_get0_type_name(pkey);
    std::cout << type_name << " (";
    int bits = EVP_PKEY_bits(pkey);
    std::cout << bits << " bits)";
    std::cout << std::endl;

    // 打印签名算法
    std::cout << "Signature Algorithm: ";
    const char *sig_alg = OBJ_nid2ln(X509_get_signature_nid(m_cert));
    std::cout << (sig_alg ? sig_alg : "Unknown") << std::endl;
}

void Cert::printExtensions()
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

void Cert::printFingerPrints()
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

bool CertReq::createCertificateRequest(KeyPair &keyPair, const std::string &subject)
{

    if (!keyPair.getPrivateKey() || !keyPair.getPublicKey())
    {
        return false;
    }

    unique_ptr<X509_REQ, X509_REQ_Deleter> req(X509_REQ_new());

    // 设置版本
    if (X509_REQ_set_version(req.get(), 0) != 1)
    {
        return false;
    }

    // 设置公钥
    if (X509_REQ_set_pubkey(req.get(), keyPair.getPublicKey()) != 1)
    {
        return false;
    }

    // 设置主题名称
    X509_NAME *name = X509_REQ_get_subject_name(req.get());
    Base::setSubjectFromString(name, subject);

    // 签名请求
    if (X509_REQ_sign(req.get(), keyPair.getPublicKey(), EVP_sm3()) != 0)
    {
        return false;
    }
    return true;
}

bool CertReq::saveCertificateRequest(const std::string &filename)
{
    if (!m_req)
        return false;

    BIO_ptr bio(BIO_new_file(filename.c_str(), "w"));
    if (!bio)
    {
        return false;
    }

    if (!PEM_write_bio_X509_REQ(bio.get(), m_req))
    {
        return false;
    }
    return true;
}

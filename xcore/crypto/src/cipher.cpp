#include "cipher.h"

using namespace x::crypto;

Cipher x::crypto::Cipher::Generator::maker(Algorithm algo, Mode mode)
{
    Cipher::Params params_ = get_default_params(algo, mode);
    if (params_.mode != Mode::ECB && (params_.key_size == 0 || params_.iv_size == 0))
    {
        throw std::runtime_error("Unsupported algorithm/mode combination");
    }
    return std::move(Cipher(get_EVP_CIPHER(algo, mode), params_));
}

Cipher::Params Cipher::Generator::get_default_params(Algorithm algo, Mode mode)
{
    Params params;
    params.algorithm = algo;
    params.mode = mode;

    switch (algo)
    {
    case Algorithm::AES_128:
        params.key_size = 16;
        params.block_size = 16;
        break;
    case Algorithm::AES_192:
        params.key_size = 24;
        params.block_size = 16;
        break;
    case Algorithm::AES_256:
        params.key_size = 32;
        params.block_size = 16;
        break;
    case Algorithm::SM4:
        params.key_size = 16;
        params.block_size = 16;
        break;
    default:
        return {}; // Unsupported algorithm
    }

    switch (mode)
    {
    case Mode::ECB:
        params.iv_size = 0;
        params.tag_size = 0;
        break;
    case Mode::CBC:
    case Mode::CFB:
    case Mode::OFB:
    case Mode::CTR:
        params.iv_size = params.block_size;
        params.tag_size = 0;
        break;
    case Mode::GCM:
        params.iv_size = 12;  // Recommended IV size for GCM
        params.tag_size = 16; // Typical tag size for GCM
        break;
    default:
        return {}; // Unsupported mode
    }

    return params;
}

EVP_CIPHER_ptr x::crypto::Cipher::Generator::get_EVP_CIPHER(Algorithm algo, Mode mode)
{
    static const std::unordered_map<Algorithm, std::unordered_map<Mode, const char *>> cipher_map = {
        {Algorithm::AES_128, {{Mode::ECB, "AES-128-ECB"}, {Mode::CBC, "AES-128-CBC"}, {Mode::CFB, "AES-128-CFB"}, {Mode::OFB, "AES-128-OFB"}, {Mode::GCM, "AES-128-GCM"}, {Mode::CTR, "AES-128-CTR"}}},
        {Algorithm::AES_192, {{Mode::ECB, "AES-192-ECB"}, {Mode::CBC, "AES-192-CBC"}, {Mode::CFB, "AES-192-CFB"}, {Mode::OFB, "AES-192-OFB"}, {Mode::GCM, "AES-192-GCM"}, {Mode::CTR, "AES-192-CTR"}}},
        {Algorithm::AES_256, {{Mode::ECB, "AES-256-ECB"}, {Mode::CBC, "AES-256-CBC"}, {Mode::CFB, "AES-256-CFB"}, {Mode::OFB, "AES-256-OFB"}, {Mode::GCM, "AES-256-GCM"}, {Mode::CTR, "AES-256-CTR"}}},
        {Algorithm::SM4, {{Mode::ECB, "SM4-ECB"}, {Mode::CBC, "SM4-CBC"}, {Mode::CFB, "SM4-CFB"}, {Mode::OFB, "SM4-OFB"}, {Mode::GCM, "SM4-GCM"}, {Mode::CTR, "SM4-CTR"}}}};

    auto algo_it = cipher_map.find(algo);
    if (algo_it == cipher_map.end())
    {
        return nullptr;
    }

    auto mode_it = algo_it->second.find(mode);
    if (mode_it == algo_it->second.end())
    {
        return nullptr;
    }

    return std::move(EVP_CIPHER_ptr(EVP_CIPHER_fetch(nullptr, mode_it->second, nullptr)));
}

void x::crypto::Cipher::set_key_iv(const Bytes &key, const Bytes &iv)
{
    this->m_key = key;
    if (iv.size() > 0)
        this->m_iv = iv;
    else
        this->m_iv = generate_random_iv();
}

Bytes x::crypto::Cipher::encrypto(const Bytes &plaintext)
{
    if (m_params.mode == Mode::GCM)
        throw std::runtime_error("gcm must provide tag buffer");

    Bytes b(0);
    return encrypto(plaintext, b);
}

Bytes x::crypto::Cipher::encrypto(const Bytes &plaintext, Bytes &tag_out, const Bytes &aad)
{
    validate_key_iv();
    EVP_CIPHER_CTX_ptr ctx(EVP_CIPHER_CTX_new());
    OSSL_ASSERT_PTR(ctx);
    // Initialize encryption
    OSSL_ASSERT_FUNC(EVP_EncryptInit_ex(ctx.get(), m_cipher.get(), nullptr, nullptr, nullptr));
    // Set IV length for modes that need it
    if (m_params.mode != Mode::ECB)
        OSSL_ASSERT_FUNC(EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_AEAD_SET_IVLEN, m_params.iv_size, nullptr));
    // Set key and IV
    OSSL_ASSERT_FUNC(EVP_EncryptInit_ex(ctx.get(), nullptr, nullptr, m_key.c_cui8(), m_iv.c_cui8()));
    // Provide AAD data if in GCM mode and AAD is provided
    if (m_params.mode == Mode::GCM && !aad.empty())
    {
        int len;
        OSSL_ASSERT_FUNC(EVP_EncryptUpdate(ctx.get(), nullptr, &len, aad.c_cui8(), aad.size()));
    }

    Bytes ciphertext(plaintext.size() + m_params.block_size); // 预留填充空间
    int len = 0;
    int ciphertext_len = 0;

    OSSL_ASSERT_FUNC(EVP_EncryptUpdate(ctx.get(), ciphertext.c_ui8(), &len, plaintext.c_cui8(), plaintext.size()));
    ciphertext_len = len;

    OSSL_ASSERT_FUNC(EVP_EncryptFinal_ex(ctx.get(), ciphertext.c_ui8() + len, &len));
    ciphertext_len += len;

    ciphertext.resize(ciphertext_len);

    // Get the authentication tag if in GCM mode
    if (m_params.mode == Mode::GCM)
    {
        tag_out.resize(m_params.tag_size);
        OSSL_ASSERT_FUNC(EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_AEAD_GET_TAG, tag_out.size(), tag_out.c_ptr()));
    }

    return std::move(ciphertext);
}

Bytes x::crypto::Cipher::decrypto(const Bytes &ciphertext, const Bytes &tag, const Bytes &aad)
{
    validate_key_iv();
    EVP_CIPHER_CTX_ptr ctx(EVP_CIPHER_CTX_new());
    OSSL_ASSERT_PTR(ctx);
    // Initialize encryption
    OSSL_ASSERT_FUNC(EVP_DecryptInit_ex(ctx.get(), m_cipher.get(), nullptr, nullptr, nullptr));
    // Set IV length for modes that need it
    if (m_params.mode != Mode::ECB)
        OSSL_ASSERT_FUNC(EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_AEAD_SET_IVLEN, m_params.iv_size, nullptr));
    // Set key and IV
    OSSL_ASSERT_FUNC(EVP_DecryptInit_ex(ctx.get(), nullptr, nullptr, m_key.c_cui8(), m_iv.c_cui8()));
    // Provide AAD data if in GCM mode and AAD is provided
    if (m_params.mode == Mode::GCM && !aad.empty())
    {
        int len;
        OSSL_ASSERT_FUNC(EVP_DecryptUpdate(ctx.get(), nullptr, &len, aad.c_cui8(), aad.size()));
    }
    // 提供要解密的密文
    Bytes plaintext(ciphertext.size() + m_params.block_size);
    int len = 0;
    int plaintext_len = 0;

    OSSL_ASSERT_FUNC(EVP_DecryptUpdate(ctx.get(), plaintext.c_ui8(), &len, ciphertext.c_cui8(), ciphertext.size()));
    plaintext_len = len;

    // Set expected authentication tag if in GCM mode
    if (m_params.mode == Mode::GCM && !tag.empty())
    {
        OSSL_ASSERT_FUNC(EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_AEAD_SET_TAG, tag.size(), const_cast<void *>(tag.c_cptr())));
    }

    // 完成解密操作
    OSSL_ASSERT_FUNC(EVP_DecryptFinal_ex(ctx.get(), plaintext.c_ui8() + len, &len));
    plaintext_len += len;

    // 调整明文大小为实际大小
    plaintext.resize(plaintext_len);
    return std::move(plaintext);
}

void x::crypto::Cipher::validate_key_iv()
{
    if (m_key.size() != m_params.key_size)
    {
        throw std::runtime_error("Invalid key size");
    }

    if (m_params.mode != Mode::ECB && m_iv.size() != m_params.iv_size)
    {
        throw std::runtime_error("Invalid IV size");
    }
}

Bytes x::crypto::Cipher::generate_random_iv()
{
    if (m_params.mode == Mode::ECB)
        return {};
    Bytes iv(m_params.iv_size);
    OSSL_ASSERT_FUNC(RAND_bytes(iv.c_ui8(), iv.size()));

    return std::move(iv);
}

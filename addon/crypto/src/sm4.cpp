#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/provider.h>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "sm4.h"

// 自定义删除器用于智能指针管理OpenSSL资源
struct EVP_CIPHER_CTX_Deleter {
    void operator()(EVP_CIPHER_CTX* ctx) const {
        EVP_CIPHER_CTX_free(ctx);
    }
};

struct OSSL_PROVIDER_Deleter {
    void operator()(OSSL_PROVIDER* provider) const {
        OSSL_PROVIDER_unload(provider);
    }
};

// 错误处理函数
void handle_openssl_error(const std::string& msg) {
    char err_buf[256];
    ERR_error_string_n(ERR_get_error(), err_buf, sizeof(err_buf));
    std::cerr << msg << ": " << err_buf << std::endl;
    throw std::runtime_error(msg);
}

// 初始化OpenSSL 3.0+环境
void init_openssl() {
    // 加载默认和legacy提供者
    std::unique_ptr<OSSL_PROVIDER, OSSL_PROVIDER_Deleter> default_provider(OSSL_PROVIDER_load(nullptr, "default"));
    if (!default_provider) {
        handle_openssl_error("Failed to load default provider");
    }

    std::unique_ptr<OSSL_PROVIDER, OSSL_PROVIDER_Deleter> legacy_provider(OSSL_PROVIDER_load(nullptr, "legacy"));
    if (!legacy_provider) {
        // 在OpenSSL 3.0中SM4可能在default提供者中，所以legacy不是必须的
        std::cerr << "Warning: Failed to load legacy provider (may not be needed for SM4)" << std::endl;
    }
}

// SM4加密函数 (CBC模式)
std::vector<unsigned char> sm4_encrypt(const std::vector<unsigned char>& plaintext,
                                      const std::vector<unsigned char>& key,
                                      const std::vector<unsigned char>& iv) {
    std::unique_ptr<EVP_CIPHER_CTX, EVP_CIPHER_CTX_Deleter> ctx(EVP_CIPHER_CTX_new());
    if (!ctx) {
        handle_openssl_error("Failed to create EVP_CIPHER_CTX");
    }

    // 初始化加密操作
    if (1 != EVP_EncryptInit_ex(ctx.get(), EVP_sm4_cbc(), nullptr, key.data(), iv.data())) {
        handle_openssl_error("Failed to initialize encryption");
    }

    // 提供要加密的明文
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int ciphertext_len = 0;

    if (1 != EVP_EncryptUpdate(ctx.get(), ciphertext.data(), &len, plaintext.data(), plaintext.size())) {
        handle_openssl_error("Failed to encrypt data");
    }
    ciphertext_len = len;

    // 完成加密操作
    if (1 != EVP_EncryptFinal_ex(ctx.get(), ciphertext.data() + len, &len)) {
        handle_openssl_error("Failed to finalize encryption");
    }
    ciphertext_len += len;

    // 调整密文大小为实际大小
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

// SM4解密函数 (CBC模式)
std::vector<unsigned char> sm4_decrypt(const std::vector<unsigned char>& ciphertext,
                                      const std::vector<unsigned char>& key,
                                      const std::vector<unsigned char>& iv) {
    std::unique_ptr<EVP_CIPHER_CTX, EVP_CIPHER_CTX_Deleter> ctx(EVP_CIPHER_CTX_new());
    if (!ctx) {
        handle_openssl_error("Failed to create EVP_CIPHER_CTX");
    }

    // 初始化解密操作
    if (1 != EVP_DecryptInit_ex(ctx.get(), EVP_sm4_cbc(), nullptr, key.data(), iv.data())) {
        handle_openssl_error("Failed to initialize decryption");
    }

    // 提供要解密的密文
    std::vector<unsigned char> plaintext(ciphertext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int plaintext_len = 0;

    if (1 != EVP_DecryptUpdate(ctx.get(), plaintext.data(), &len, ciphertext.data(), ciphertext.size())) {
        handle_openssl_error("Failed to decrypt data");
    }
    plaintext_len = len;

    // 完成解密操作
    if (1 != EVP_DecryptFinal_ex(ctx.get(), plaintext.data() + len, &len)) {
        handle_openssl_error("Failed to finalize decryption");
    }
    plaintext_len += len;

    // 调整明文大小为实际大小
    plaintext.resize(plaintext_len);
    return plaintext;
}

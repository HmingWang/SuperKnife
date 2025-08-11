#include "catch2/catch_test_macros.hpp"
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/bn.h>
#include <iostream>
#include <vector>
#include <catch2/catch_all.hpp>
#include "openssl.h"

// 检查是否有公钥（支持 RSA / EC / SM2）
bool has_public_key(EVP_PKEY *pkey) {
    int type = EVP_PKEY_get_base_id(pkey);

    if (type == EVP_PKEY_RSA || type == EVP_PKEY_RSA_PSS) {
        BIGNUM *n = nullptr;
        if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, &n)) {
            BN_free(n);
            return true;
        }
        return false;
    } else { // EC / SM2 等
        size_t len = 0;
        if (EVP_PKEY_get_octet_string_param(
                pkey, OSSL_PKEY_PARAM_PUB_KEY, nullptr, 0, &len)) {
            return len > 0;
        }
        return false;
    }
}

// 检查是否有私钥（支持 RSA / EC / SM2）
bool has_private_key(EVP_PKEY *pkey) {
    int type = EVP_PKEY_get_base_id(pkey);

    if (type == EVP_PKEY_RSA || type == EVP_PKEY_RSA_PSS) {
        BIGNUM *d = nullptr;
        if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_D, &d)) {
            BN_free(d);
            return true;
        }
        return false;
    } else { // EC / SM2 等
        size_t len = 0;
        if (EVP_PKEY_get_octet_string_param(
                pkey, OSSL_PKEY_PARAM_PRIV_KEY, nullptr, 0, &len)) {
            return len > 0;
        }
        return false;
    }
}

// 检查公钥和私钥是否匹配
bool keys_match(EVP_PKEY *pkey) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) return false;
    int ok = EVP_PKEY_pairwise_check(ctx);
    EVP_PKEY_CTX_free(ctx);
    return ok == 1;
}

// 辅助函数：生成一个 SM2 密钥对（可替换成 RSA / EC）
EVP_PKEY *generate_sm2_key() {
    EVP_PKEY_CTX_ptr pctx( EVP_PKEY_CTX_new_id(EVP_PKEY_SM2, nullptr));

    if (EVP_PKEY_keygen_init(pctx.get()) <= 0) {
        return nullptr;
    }

    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(pctx.get(), &pkey) <= 0) {
        return nullptr;
    }

    return pkey;
}

TEST_CASE(" main demo ") {
    OpenSSL_add_all_algorithms();

    EVP_PKEY *pkey = generate_sm2_key();
    if (!pkey) {
        std::cerr << "生成密钥失败\n";
        REQUIRE(false) ;
    }

    std::cout << "有公钥: " << has_public_key(pkey) << "\n";
    std::cout << "有私钥: " << has_private_key(pkey) << "\n";
    std::cout << "公私钥匹配: " << keys_match(pkey) << "\n";

    EVP_PKEY_free(pkey);
    REQUIRE(true) ;
}

#include "digest.h"

using namespace x::crypto;
Digest Digest::Generator::sm3()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_sm3()))));
}

Digest Digest::Generator::sha1()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_sha1()))));
}

Digest Digest::Generator::sha256()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_sha256()))));
}

Digest Digest::Generator::sha512()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_sha512()))));
}

Digest Digest::Generator::md5()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_md5()))));
}

Bytes x::crypto::Digest::hash_file(std::string_view filename)
{

    std::ifstream file(filepath, std::ios::binary);
    OSSL_ASSERT_PTR(file);


    size_t buffer_size = 4096; // 4k缓冲
    Bytes buffer(buffer_size);
    EVP_MD_CTX_ptr ctx(EVP_MD_CTX_new());
    OSSL_ASSERT_PTR(ctx);
    OSSL_ASSERT_FUNC(EVP_DigestInit_ex(ctx, get_EVP_MD(), nullptr));

    while(file){
        file.readsome(buffer.c_ptr(),buffer_size);
        std::streamsize bytes_read = file.gcount();
        if (bytes_read > 0) {
            EVP_DigestUpdate(buffer.data(), static_cast<size_t>(bytes_read));
        }
    }
}

#include "digest.h"
#include "openssl.h"
#include "file.h"

using namespace x::crypto;
Digest Digest::Generator::sm3()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_MD_fetch(nullptr, "sm3", nullptr)))));
}

Digest Digest::Generator::sha1()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_MD_fetch(nullptr, "sha1", nullptr)))));
}

Digest Digest::Generator::sha256()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_MD_fetch(nullptr, "sha256", nullptr)))));
}

Digest Digest::Generator::sha512()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_MD_fetch(nullptr, "sha512", nullptr)))));
}

Digest Digest::Generator::md5()
{
    return std::move(Digest(std::move(EVP_MD_ptr(EVP_MD_fetch(nullptr, "md5", nullptr)))));
}

Bytes x::crypto::Digest::hash_file(const std::string_view filename)
{

    EVP_MD_CTX_ptr ctx(EVP_MD_CTX_new());
    OSSL_ASSERT_PTR(ctx);
    OSSL_ASSERT_FUNC(EVP_DigestInit_ex(ctx.get(), get_EVP_MD(), nullptr));

    File f(filename.data());
    Bytes buffer(4096);
    while (f.good())
    {
        auto read_bytes = f.readData(buffer);
        if (read_bytes > 0)
            OSSL_ASSERT_FUNC(EVP_DigestUpdate(ctx.get(), buffer.c_cptr(), read_bytes));
    }

    unsigned int md_len = EVP_MD_size(m_md.get());
    Bytes digest(md_len);

    OSSL_ASSERT_FUNC(EVP_DigestFinal_ex(ctx.get(), (unsigned char *)digest.c_ptr(), &md_len));
    digest.resize(md_len);
    return std::move(digest);
}

Bytes x::crypto::Digest::hash(Bytes data)
{
    unsigned int md_len = EVP_MD_size(m_md.get());
    Bytes digest(md_len);
    OSSL_ASSERT_FUNC(EVP_Digest(data.c_cptr(),data.size(),(unsigned char *)digest.c_ptr(),&md_len,get_EVP_MD(),nullptr));
    digest.resize(md_len);
    return std::move(digest);
}

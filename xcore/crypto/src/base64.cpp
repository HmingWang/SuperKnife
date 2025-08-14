#include <base64.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string>

using namespace x::crypto;
std::string Base64::Encoder::encode(const Bytes &in, const bool wrap)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    if (!wrap)
    {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // No newlines
    }
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, in.c_cptr(), in.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    return std::move(std::string(bufferPtr->data, bufferPtr->length));
}

Bytes Base64::Decoder::decode(const std::string_view in)
{
    // 预处理输入：移除所有非Base64字符（包括换行符）
    std::string cleaned_input;
    cleaned_input.reserve(in.size());

    for (char c : in)
    {
        if (isalnum(c) || (c == '+') || (c == '/') || (c == '='))
        {
            cleaned_input.push_back(c);
        }
    }

    BIO *bio, *b64;
    int decodeLen = cleaned_input.length();
    char *buffer = (char *)malloc(decodeLen);
    memset(buffer, 0, decodeLen);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // No newlines
    bio = BIO_new_mem_buf(cleaned_input.c_str(), -1);
    bio = BIO_push(b64, bio);

    int length = BIO_read(bio, buffer, decodeLen);
    BIO_free_all(bio);

    Bytes out((std::byte *)buffer, length);
    free(buffer);
    return std::move(out);
}

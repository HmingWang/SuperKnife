#include <base64.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string>

// 检查字符是否为有效的Base64字符
bool is_base64(char c)
{
    return (isalnum(c) || (c == '+') || (c == '/') || (c == '='));
}

std::string base64_encode(const std::string &in, const bool wrap)
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

    BIO_write(bio, in.c_str(), in.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    return std::string(bufferPtr->data, bufferPtr->length);
}

std::string base64_decode(const std::string &in)
{

    // 预处理输入：移除所有非Base64字符（包括换行符）
    std::string cleaned_input;
    cleaned_input.reserve(in.size());

    for (char c : in)
    {
        if (is_base64(c) || c == '=')
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

    std::string out(buffer, length);
    free(buffer);
    return out;
}

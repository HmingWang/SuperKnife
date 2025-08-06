#include<base64.h>
#include<openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string>

std::string base64_encode(const std::string &in, const bool wrap) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    if (wrap) {
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

std::string base64_decode(const std::string &in) {
    BIO *bio, *b64;
    int decodeLen = in.length();
    char *buffer = (char *)malloc(decodeLen);
    memset(buffer, 0, decodeLen);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // No newlines
    bio = BIO_new_mem_buf(in.c_str(), -1);
    bio = BIO_push(b64, bio);

    int length = BIO_read(bio, buffer, decodeLen);
    BIO_free_all(bio);

    std::string out(buffer, length);
    free(buffer);
    return out;
}

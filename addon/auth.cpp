#include "auth.h"
#include <sqlite3.hpp>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <iostream>

std::string sha256(const std::string &str, const std::string &salt)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha256();
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int hash_len;

    std::string salted = str + salt;

    // compute SHA-256 hash
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, salted.c_str(), salted.size());
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);

    // convert to hex string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool nativeVerifyLogin(const std::string &username, const std::string &password)
{
    try
    {
        SQLiteDB db("./resources/store.db");
        std::cout << "Database opened successfully." << std::endl;
        auto result = db.query("SELECT password, salt FROM userinfo WHERE username = ?", {username});
        auto passwordHash = result.empty() ? "" : result[0][0];
        auto salt = result.empty() ? "" : result[0][1];
        std::cout << "Query executed successfully." << std::endl;
        std::string inputHash = sha256(password, salt);
        std::cout << "Input hash: " << inputHash << ", Password hash: " << passwordHash << std::endl;
        return (inputHash == passwordHash);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return false;
    }
}

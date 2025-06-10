#include "auth.h"
#include <sqlite3.h>
#include <openssl/evp.h>  
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string sha256(const std::string &str, const std::string &salt) {
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
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool nativeVerifyLogin(const std::string& username, const std::string& password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    bool result = false;
    std::string dbPath = "./resources/auth.db";  // adjust the path as needed

    if(sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        return false;
    }

    const char* sql = "SELECT password_hash, salt FROM users WHERE username = ?";
    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ROW) {
        const char* storedHash = (const char*)sqlite3_column_text(stmt, 0);
        const char* salt = (const char*)sqlite3_column_text(stmt, 1);
        std::string inputHash = sha256(password, salt);
        result = (inputHash == storedHash);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}


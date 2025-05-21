#include "auth.h"
#include <sqlite3.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

using namespace Napi;

std::string sha256(const std::string &str, const std::string &salt) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    std::string salted = str + salt;
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, salted.c_str(), salted.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool nativeVerifyLogin(const std::string& username, const std::string& password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    bool result = false;
    std::string dbPath = "users.db";  // 实际应用中应该使用绝对路径

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

Napi::Boolean VerifyLogin(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "需要两个字符串参数").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    std::string username = info[0].As<Napi::String>();
    std::string password = info[1].As<Napi::String>();

    bool result = nativeVerifyLogin(username, password);
    return Napi::Boolean::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("verifyLogin", Napi::Function::New(env, VerifyLogin));
    return exports;
}

#include "auth.h"
#include "sqlitedb.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include "digest.h"

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

        std::string inputHash = x::crypto::Digest::Generator::sha256().hash((String(password)+salt).to_bytes()).to_hex_string();
        std::cout << "Input hash: " << inputHash << ", Password hash: " << passwordHash << std::endl;
        return (inputHash == passwordHash);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return false;
    }
}

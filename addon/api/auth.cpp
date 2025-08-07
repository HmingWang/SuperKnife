#include "auth.h"
#include "sqlitedb.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include "sha256.h"

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

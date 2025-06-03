#pragma once
#include <string>

std::string sha256(const std::string &str, const std::string &salt);

bool nativeVerifyLogin(const std::string& username, const std::string& password);

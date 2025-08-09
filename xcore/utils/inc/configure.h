#pragma once

#include "headers.h"
#include "xstring.h"

class Configure
{

    Configure(String filePath="config.ini");

public:
    String getValueDef(String key, String def = "");

    int getIntDef(String key, int def = 0);

    static Configure &getInstance();

    bool load(String filePath);

private:
    std::unordered_map<String, String, String::Hash> configData;
};

#define gConfig Configure::getInstance()
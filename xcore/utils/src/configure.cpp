
#include "configure.h"
#include "exceptions.h"
#include "istream"


bool Configure::load(String filePath) {
    std::fstream fs;
    fs.open(filePath.to_string(), std::ios::in);
    if (!fs.good())
        return false;

    String line;
    String value;
    String key;

    configData.clear();
    while (getline(fs, line.get())) {
        if (line = line.trim(); line.empty() || line.at(0) == '#')
            continue;
        std::istringstream is_line(line.to_string());
        if (std::getline(is_line, key.get(), '=')) {
            if (std::getline(is_line, value.get())) {
                configData[key.trim()]=value.trim();
            }
        }
    }

    fs.close();
    return true;
}

Configure::Configure(String filePath)
{
    load(filePath);
}

String Configure::getValueDef(String key, String def)
{
    return configData[key].empty() ? def : configData[key];
}

Configure &Configure::getInstance() {
    static Configure instance;
    return instance;
}

int Configure::getIntDef(String key, int def) {
    String v = getValueDef(key);
    return v.empty() ? def : std::stoi(v.to_string());
}

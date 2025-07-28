#include <base64.h>
#include <string>
#include <vector>

std::string base64_encode(const std::string &in, const bool wrap_at_76)
{
    static const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;

    for (unsigned char c : in)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        out.push_back(base64_chars[((val << (valb + 6)) & 0x3F)]);

    while (out.size() % 4 != 0)
        out.push_back('=');

    if (wrap_at_76)
    {
        for (size_t i = 76; i < out.size(); i += 77)
            out.insert(i, "\n");
    }

    return out;
}

// Base64 decoding implementation
std::string base64_decode(const std::string &in)
{
    static const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++)
        T[base64_chars[i]] = i;

    std::string out;
    int val = 0, valb = -8;
    for (unsigned char c : in)
    {
        if (T[c] == -1)
            continue; // Ignore invalid characters
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0)
        {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

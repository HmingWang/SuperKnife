#pragma once

#include "openssl.h"
#include "headers.h"
#include <string_view>

namespace x::crypto
{

    class Utils
    {

    public:
        static void add_extensions(X509 *cert, int nid, std::string_view value);
        static void set_subject_name(X509_NAME *name, std::string_view subject);
    };

}

#include "utils.h"
#include "openssl.h"
#include "osslexception.h"
#include <openssl/x509.h>

namespace x::crypto
{

  void Utils::add_extensions(X509 *cert, int nid, std::string_view value)
  {
    OSSL_ASSERT_PTR(cert);

    X509V3_CTX ctx;
    X509V3_set_ctx_nodb(&ctx);
    X509V3_set_ctx(&ctx, cert, cert, nullptr, nullptr, 0);

    X509_EXTENSION_ptr ex(X509V3_EXT_conf_nid(nullptr, &ctx, nid, value.data()));
    OSSL_ASSERT_PTR(ex);
    OSSL_ASSERT_FUNC(X509_add_ext(cert, ex.get(), -1));
  }

  void Utils::set_subject_name(X509_NAME *name, std::string_view subject)
  {
    for (auto &r : String(subject).split(","))
    {
      auto p = r.split("=");
      auto k = p[0].trim();
      auto v = p[1].trim();
      OSSL_ASSERT_FUNC(X509_NAME_add_entry_by_txt(name, k.to_cstr(), MBSTRING_ASC, reinterpret_cast<const unsigned char *>(v.to_cstr()), -1, -1, 0));
    }
  }

}

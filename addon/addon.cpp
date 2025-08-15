#include "api.h"
#include <iostream>
#include <napi.h>

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    std::cout << "Initializing addon..." << std::endl;
    
    exports.Set(Napi::String::New(env, "verifyLogin"), Napi::Function::New(env, VerifyLogin));
    exports.Set(Napi::String::New(env, "base64Encode"), Napi::Function::New(env, Base64Encode));
    exports.Set(Napi::String::New(env, "base64Decode"), Napi::Function::New(env, Base64Decode));

    std::cout << "Addon initialized." << std::endl;
    return exports;
}


NODE_API_MODULE(api, Init)
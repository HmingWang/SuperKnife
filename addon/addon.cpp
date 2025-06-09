#include "addon.h"
#include <string>
#include "auth.h"
#include <iostream>

Napi::Boolean VerifyLogin(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "需要两个字符串参数").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    std::string username = info[0].As<Napi::String>();
    std::string password = info[1].As<Napi::String>();

    std::cout<<"Received username: " << username << ", password: " << password << std::endl;
    bool result = nativeVerifyLogin(username, password);
    return Napi::Boolean::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("verifyLogin", Napi::Function::New(env, VerifyLogin));
    return exports;
}



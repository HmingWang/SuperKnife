#include "addon.h"
#include <string>
#include "auth.h"
#include <iostream>

Napi::String Method(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    return Napi::String::New(env, "world"); 
}

Napi::Boolean VerifyLogin(const Napi::CallbackInfo &info)
{
    std::cout << "verifylogin..." << std::endl;

    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "need two arguments").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    std::string username = info[0].As<Napi::String>();
    std::string password = info[1].As<Napi::String>();

    std::cout << "Received username: " << username << ", password: " << password << std::endl;
    bool result = nativeVerifyLogin(username, password);
    return Napi::Boolean::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    std::cout << "Initializing addon..." << std::endl;
    exports.Set(Napi::String::New(env, "verifyLogin"), Napi::Function::New(env, VerifyLogin));
    exports.Set(Napi::String::New(env, "hello"), Napi::Function::New(env, Method));

    std::cout << "Addon initialized." << std::endl;
    return exports;
}

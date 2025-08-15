#include "api.h"
#include <string>
#include "auth.h"
#include <iostream>
#include "base64.h"

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

Napi::String Base64Encode(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return Napi::String::New(env, "null");
    }
    std::string input = info[0].As<Napi::String>();
    bool wrap = false;
    if (info.Length() > 1 && info[1].IsBoolean())
    {
        wrap = info[1].As<Napi::Boolean>();
    }
    std::string output = base64_encode(input, wrap);
    return Napi::String::New(env, output);
}

Napi::String Base64Decode(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return Napi::String::New(env, "null");
    }
    std::string input = info[0].As<Napi::String>();
    std::string output = base64_decode(input);
    return Napi::String::New(env, output);
}
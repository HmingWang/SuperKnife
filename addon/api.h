#pragma once
#include <napi.h>

Napi::Boolean VerifyLogin(const Napi::CallbackInfo &info);
Napi::String Base64Encode(const Napi::CallbackInfo &info);
Napi::String Base64Decode(const Napi::CallbackInfo &info);
Napi::Boolean MQSendMessage(const Napi::CallbackInfo &info);
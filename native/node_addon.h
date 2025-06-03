#pragma once

#include <napi.h>

// 声明暴露给 Node.js 的方法
Napi::Boolean VerifyLogin(const Napi::CallbackInfo& info);
// 模块初始化函数
Napi::Object Init(Napi::Env env, Napi::Object exports);
// 注册模块
NODE_API_MODULE(auth, Init);

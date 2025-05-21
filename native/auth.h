#ifndef AUTH_H
#define AUTH_H

#include <napi.h>

// 暴露给 Node.js 的方法
Napi::Boolean VerifyLogin(const Napi::CallbackInfo& info);

// 模块初始化
Napi::Object Init(Napi::Env env, Napi::Object exports);

// 注册模块
NODE_API_MODULE(auth, Init)

#endif // AUTH_H

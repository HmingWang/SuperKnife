#include <catch2/catch_all.hpp>
#include "headers.h"
#include <dlfcn.h>



TEST_CASE("load dll") {
    // Your test code here
    #if XCORE_PLATFORM== XCORE_PLATFORM_WINDOWS
        typedef int (WINAPI *FUNC_void)();

        HINSTANCE hdll;
        hdll = LoadLibrary("backend_d.dll");
        REQUIRE(hdll != NULL);
        FUNC_void hello = (FUNC_void)GetProcAddress(hdll, "hello");
        REQUIRE(hello != nullptr);
        int result = hello();
        REQUIRE(result == 1);
    #endif

    #if XCORE_PLATFORM == XCORE_PLATFORM_APPLE
    // Load the shared library
        void* handle = dlopen("libbackend_d.dylib", RTLD_LAZY);
        REQUIRE(handle != NULL);

        // Get the function pointer
        typedef int (*hello_func)();
        hello_func hello = (hello_func)dlsym(handle, "hello");
        REQUIRE(hello != NULL);

        // Call the function
        int result = hello();
        REQUIRE(result == 1);

        // Close the library
        dlclose(handle);
    #endif

}
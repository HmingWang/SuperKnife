#include <catch2/catch_all.hpp>
#include <windows.h>


typedef int (WINAPI *FUNC_void)();


TEST_CASE("load dll") {
    // Your test code here
    HINSTANCE hdll;
    hdll = LoadLibrary("backend_d.dll");
    REQUIRE(hdll != NULL);
    FUNC_void hello = (FUNC_void)GetProcAddress(hdll, "hello");
    REQUIRE(hello != nullptr);
    int result = hello();
    REQUIRE(result == 1);

}
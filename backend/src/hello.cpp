#include <hello.h>
#include <iostream>

int WINAPI hello(){
    std::cout << "Hello from DLL!" << std::endl;
    return 1;
}
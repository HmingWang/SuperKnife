#pragma once

///////////////////STL header//////////////////
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <cassert>
#include <numeric>
#include <string>
#include <string_view>
#include <algorithm>
#include <utility>
#include <bitset>
#include <cstddef>
#include <exception>
#include <format>
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>
#include <ctime>
#include <ostream>
#include <thread>
#include <regex>
#include <cstddef>
#include <cinttypes>
#include <climits>
#include <memory>

///////////////////xCore header//////////////////
#include "compiler.h"
#include "define.h"


///////////////////system header//////////////////
#if XCORE_PLATFORM == XCORE_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN             // exclude some useless Windows headers
    // Windows header
    #include <windows.h>

#endif


#include <catch2/catch_all.hpp>


void hello();

TEST_CASE("Test case name") {
    // Your test code here
    hello();
    REQUIRE(1 + 1 == 2);
}
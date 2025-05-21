#include "catch2/catch_all.hpp"

TEST_CASE("Catch2 Avaliable test")
{
    int i = 0;
    REQUIRE(i == 0);
}


{
    gLog.log(Logger::Level::Warning, "this is a int:{}", 1);
    LOG_INFO("this is a string :{}", "hh");
    LOG_DEBUG("this is a debug string :{},{}", "hh", 123123);
    LOG_ERROR("this is a error  string :{},{},{}", "hh", 123123, 3.1514);

    REQUIRE(true);
}

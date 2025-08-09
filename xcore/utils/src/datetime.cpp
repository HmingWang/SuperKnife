#include "headers.h"
#include "datetime.h"
#include "exceptions.h"
/**
 * 适应环境windows下用localtime_s 其他环境用localtime_r
 * 不适用localtime函数，因为其函数返回指针，线程不安全。
 */
#if XCORE_PLATFORM == XCORE_PLATFORM_WINDOWS
struct tm *localtime_r(time_t const *time, struct tm *result)
{
    if (localtime_s(result, time) != 0)
        return nullptr;
    return result;
}
struct tm *gmtime_r(time_t const *time, struct tm *result)
{
    if (gmtime_s(result, time) != 0)
        return nullptr;
    return result;
}
time_t timegm(struct tm *tm)
{
    return _mkgmtime(tm);
}
#endif

DateTime DateTime::now()
{
    auto time = std::chrono::system_clock::now();
    return std::move(DateTime(time));
}
DateTime::DateTime(std::chrono::system_clock::time_point tp) : _time(tp)
{
}
DateTime::DateTime(tm t)
{
    time_t tt = mktime(&t);
    _time = std::chrono::system_clock::from_time_t(tt);
}
String DateTime::to_string(String format)
{
    tm t;
    time_t tt = std::chrono::system_clock::to_time_t(_time);
    localtime_r(&tt, &t);
    std::ostringstream oss;
    oss << std::put_time(&t, format.to_cstr());
    return oss.str();
}

DateTime DateTime::from_string(String time, String format)
{
    std::tm t{};
    std::istringstream iss(time.to_string());
    iss >> std::get_time(&t, format.to_cstr());
    if (iss.fail())
        throw Exception("fail to convert time from string");
    return std::move(DateTime(t));
}

bool DateTime::operator<(const DateTime &t) const
{
    return _time < t._time;
}

bool DateTime::operator>(const DateTime &t) const
{
    return _time > t._time;
}

bool DateTime::operator==(const DateTime &t) const
{
    return _time == t._time;
}

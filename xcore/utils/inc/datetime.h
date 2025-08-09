#pragma once
#include "xstring.h"

class DateTime
{
public:
    DateTime(std::chrono::system_clock::time_point tp);
    DateTime(tm t);
    DateTime() = default;
    static DateTime now();
    //%Y-%m-%d %H:%M:%S
    String to_string(String format = "%Y-%m-%d %H:%M:%S");
    static DateTime from_string(String time, String format = "%Y-%m-%d %H:%M:%S");

    bool operator<(const DateTime &t) const;
    bool operator>(const DateTime &t) const;
    bool operator==(const DateTime &t) const;

private:
    std::chrono::system_clock::time_point _time;

    // int _sec;  // seconds after the minute - [0, 60] including leap second
    // int _min;  // minutes after the hour - [0, 59]
    // int _hour; // hours since midnight - [0, 23]
    // int _day;  // day of the month - [1, 31]
    // int _mon;  // months since January - [0, 11]
    // int _year; // years since 1900
};

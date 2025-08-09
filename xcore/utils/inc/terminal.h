#pragma once

#include "headers.h"

class Terminal
{
    inline static const std::string CSI = "\033[";

public:
    enum class Color
    {
        Cyan = 14,
        Red = 9,
        Orange = 214,
        Yellow = 11,
        Green = 2,
        Blue = 12,
        Purple = 5,
        White = 15,
        Black = 0,
        Gray = 8
    };

    static void move_to(int row, int col, std::ostream &os = std::cout);
    static void set_fore_color(int id, std::ostream &os = std::cout);
    static void set_back_color(int id, std::ostream &os = std::cout);
    static void clean_screen(std::ostream &os = std::cout);
    static void reset_color(std::ostream &os = std::cout);
    static void hide_cursor(std::ostream &os = std::cout);
    static void show_cursor(std::ostream &os = std::cout);
    static void set_blod(std::ostream &os = std::cout);
    static void set_underline(std::ostream &os = std::cout);
};

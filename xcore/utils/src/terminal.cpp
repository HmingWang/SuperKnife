#include "terminal.h"




    void Terminal::move_to(int row, int col, std::ostream &os)
    {
        os << CSI << row << ';' << col << 'H';
    }

    void Terminal::set_fore_color(int id, std::ostream &os)
    {
        os << CSI << "38;5;" << id << 'm';
    }

    void Terminal::set_back_color(int id, std::ostream &os)
    {
        os << CSI << "48;5;" << id << 'm';
    }

    void Terminal::clean_screen(std::ostream &os)
    {
        os << CSI << "2J";
    }

    void Terminal::reset_color(std::ostream &os)
    {
        os << CSI << "0m";
    }

    void Terminal::hide_cursor(std::ostream &os)
    {
        os << CSI << "?25l";
    }

    void Terminal::show_cursor(std::ostream &os)
    {
        os << CSI << "?25h";
    }

    void Terminal::set_blod(std::ostream &os)
    {
        os << CSI << "1m";
    }

    void Terminal::set_underline(std::ostream &os)
    {
        os << CSI << "4m";
    }


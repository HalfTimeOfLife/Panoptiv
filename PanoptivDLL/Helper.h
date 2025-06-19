#ifndef HELPER_H
#define HELPER_H

#include<windows.h>

void set_console_color(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

#endif // HELPER_H
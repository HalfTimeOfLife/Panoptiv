#ifndef HELPER_H
#define HELPER_H

#include<windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

static int random_number = 0;

/* 
* @brief Sets the console text color.
* 
* This function changes the color of the text in the console window, according to the
* trigger case.
* 
* @param color The color to set the console text to, using Windows console color codes.
*/
void set_console_color(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}


/*
* @brief Generates a random number using the Windows Cryptography API.
*
* This function generates a random number and stores it in the global variable `random_number`.
* 
* @return Returns 0 on success, or -1 if there was an error generating the random number.
*/
int generate_random_number() {
    if(BCryptGenRandom(NULL, (PUCHAR)&random_number, sizeof(random_number), BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        return -1;
	}

    return 0;
}

#endif // HELPER_H
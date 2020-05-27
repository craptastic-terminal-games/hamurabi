#ifndef NCURSES_WINDOWS
#define NCURSES_WINDOWS

//------------------------------------- headers -------------------------------
#include <ncurses.h>
#include "city.h"

//------------------------------------- Globals -------------------------------
WINDOW *MAIN_WIN,*INFO_WIN;

//------------------------------------- prototypes ----------------------------
void initScreen(void);
void updateInfo(const int year, const city_st *const cty);

#endif
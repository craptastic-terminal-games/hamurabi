//------------------------------------- headers -------------------------------
#include "ncurses_windows.h"

//------------------------------------- prototypes ----------------------------
static void makeWindows(void);

//------------------------------------- code ----------------------------------
void initScreen(void) {
	initscr();
	curs_set(0);	//dont display cursor
	makeWindows();
}

static void makeWindows(void) {
	WINDOW *main_border = newwin(17,60,0,0);
	WINDOW *info_border = newwin(6,24,0,61);
	MAIN_WIN            = newwin(15,58,1,1);
	INFO_WIN            = newwin(4,22,1,62);
	wborder(main_border,'|','|','-', '-', '+', '+', '+', '+');
	wborder(info_border,'|','|','-', '-', '+', '+', '+', '+');	
	refresh();
	wrefresh(main_border);
	wrefresh(info_border);
	wrefresh(MAIN_WIN);
	wrefresh(INFO_WIN);
}

void updateInfo(const int year, const city_st *const cty) {
	wclear(INFO_WIN);
	mvwprintw(INFO_WIN,0,0,"year:       %d",year);
	mvwprintw(INFO_WIN,1,0,"acres:      %d",acres(cty));
	mvwprintw(INFO_WIN,2,0,"bushles:    %d",bushels(cty));
	mvwprintw(INFO_WIN,3,0,"population: %d",population(cty));
	wrefresh(INFO_WIN);
}
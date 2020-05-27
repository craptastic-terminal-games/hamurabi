/*
   This an ancient game. Have a gander at this:

   10 REM *** CONVERTED FROM THE ORIGINAL FOCAL PROGRAM AND MODIFIED
   20 REM *** FOR EDUSYSTEM 70 BY DAVID AHL, DIGITAL
   30 REM *** MODIFIED FOR 8K MICROSOFT BASIC BY PETER TURNBULL

   Well, now it has been transliterated from the BASIC to C.

   The MIT License

   Copyright (c) 2010 Brian L. Troutwine

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated
   documentation files (the "Software"), to deal in the
   Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to
   do so, subject to the following conditions:

   The above copyright notice and this permission notice shall
   be included in all copies or substantial portions of the
   Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
   KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
   PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
   OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* 
    Ncurses functionality added by github/return5  @May,2020
        I tried to keep as much of the original code intact as i could.
        I only altered main.c, left all code in city.c and city.h alone.  added ncurses_windows.{c,h}
        All camelCase functions are ones I've added. original functions have all been left as snake_case. 
        I changed all the goto statements into their own functions.
        Indented with four spaces instead of two.  
        This was a very quick and low effort job on my part so may not be 100% functional. I present it as is. 
*/
//------------------------------------- headers -------------------------------
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include "ncurses_windows.h"

//------------------------------------- prototypes ----------------------------
uint16_t input(void);
void retire();
void storm_out();
void ejected();
void game_end(const city_st *cty);
void nero_end();
void not_so_bad_end(const city_st *cty);
void buyAcres(city_st *const cty);
void feedPeople(city_st *const cty);
void plantSeed(city_st *const cty);
void gameLoop(void);
void sellAcres(city_st *const cty);
void printYearlyReport(const uint16_t year,city_st *const cty);

//------------------------------------- code ----------------------------------
uint16_t input(void) {
    wrefresh(MAIN_WIN);
    const uint16_t inp_size = 10;
    char input[inp_size], *endptr;
    wgetstr(MAIN_WIN,input);
    errno = 0;
    const uint16_t q = (uint16_t) strtol(input, &endptr, inp_size);
    if ((errno == ERANGE && (q == LONG_MAX || q == LONG_MIN)) || (errno != 0 && q == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == input) {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }
    return q;
}

/*
   Client functions

   Someday I dream of being in curses.
 */
void retire() {
    wprintw(MAIN_WIN,"So long for now.\n");
    wrefresh(MAIN_WIN);
    getch();
    endwin();
    exit(EXIT_SUCCESS);
}

void storm_out() {
    wprintw(MAIN_WIN,"Hamurabi: I cannot do what you wish.\n");
    wprintw(MAIN_WIN,"Get yourself another steward!!!!!\n");
    retire();
}

void ejected() {
    wprintw(MAIN_WIN,"Due to this extreme mismanagement you have not only\n");
    wprintw(MAIN_WIN,"been impeached and thrown out of office but you have\n");
    wprintw(MAIN_WIN,"also been declared 'National Fink' !!\n");
    retire();
}

void game_end(const city_st *cty) {
    uint16_t l = (uint16_t) (acres(cty)/(population(cty)));
    wprintw(MAIN_WIN,"In your 10-year term of office %d percent of the\n",avg_starved(cty));
    wprintw(MAIN_WIN,"population starved per year on average, i.e., a total of\n");
    wprintw(MAIN_WIN,"%d people died!!\n\n", tot_died(cty));
    wprintw(MAIN_WIN,"You started with 10 acres per person and ended with\n");
    wprintw(MAIN_WIN,"%d acres per person.\n\n", l);

    if ((avg_starved(cty)>33) || (l<7)) ejected();
    if ((avg_starved(cty)>10) || (l<9)) nero_end();
    if ((avg_starved(cty)>3)  || (l<10)) not_so_bad_end(cty);
}

void nero_end() {
    wprintw(MAIN_WIN,"Your heavy handed performance smacks of Nero and Ivan IV.\n");
    wprintw(MAIN_WIN,"The people (remaining) find you an unpleasant ruler, and,\n");
    wprintw(MAIN_WIN,"frankly, hate your guts!\n");
      retire();
}

void not_so_bad_end(const city_st *cty) {
    wprintw(MAIN_WIN,"Your performance could have been somewhat better, but\n");
    wprintw(MAIN_WIN,"really wasn't too bad at all. ");
    wprintw(MAIN_WIN,"%d people would ", RAND(population(cty)));
    wprintw(MAIN_WIN,"dearly like to see you assassinated but we all have our\n");
    wprintw(MAIN_WIN,"trivial problems.\n");
    retire();
}

void buyAcres(city_st *const cty) {
    wprintw(MAIN_WIN,"Land is trading at %d bushels per acre.\n",trade_val(cty));
    wprintw(MAIN_WIN,"How many acres do you wish to buy?\n");
    const uint16_t inp = input();
    if (inp < 0) {
        storm_out();
    }
    else if (inp == 0) {
        return sellAcres(cty);
    }
    if (buy_acres(cty, inp) != 0) {
        wprintw(MAIN_WIN,"Hamurabi: Think again. You have only %d",bushels(cty));
        wprintw(MAIN_WIN," bushels of grain. Now then,\n");
        return buyAcres(cty);
    }
}

void sellAcres(city_st *const cty) {
    wprintw(MAIN_WIN,"How many acres do you wish to sell?\n");
    const uint16_t inp = input();
    if (inp < 0) {
        storm_out();
    }
    else if (inp > 0) {
        if (sell_acres(cty, inp) != 0) {
            wprintw(MAIN_WIN,"Hamurabi: Think again. You have only %d acres.",acres(cty));
            wprintw(MAIN_WIN," Now then,\n");
            return sellAcres(cty);
        }
    }
}

void feedPeople(city_st *const cty) {
    wprintw(MAIN_WIN,"How many bushels do you wish to feed your people?\n");
    wprintw(MAIN_WIN,"(No starvation: %d bushels.)\n",population(cty)*20);
    const uint16_t inp = input();
    if (inp < 0) {
        storm_out();
    }
    else if (inp > 0) {
        if (feed_populace(cty, inp) != 0) {
          wprintw(MAIN_WIN,"Hamurabi: Think again. We only have %d bushels.",bushels(cty));
          wprintw(MAIN_WIN," Now then,\n");
          return feedPeople(cty);
        }
    }
}

void plantSeed(city_st *const cty) {
    wprintw(MAIN_WIN,"How many acres do you wish to plant with seed?\n");
    wprintw(MAIN_WIN,"(MAX: %d acres)\n", 10*population(cty));
    const uint16_t inp = input();
    if (inp < 0) {
        storm_out();
    }
    else if (inp > 0) {
        switch(plant_seed(cty, inp)) {
            case EACREAGE:
                wprintw(MAIN_WIN,"Hamurabi: Think again. You have only %d acres.",acres(cty));
                wprintw(MAIN_WIN," Now then,\n");
                return plantSeed(cty);
            case EPOP: 
                wprintw(MAIN_WIN,"But you only have %d people to tend the fields.",population(cty));
                wprintw(MAIN_WIN," Now then,\n");
                return plantSeed(cty);
            case EBUSHELS:
                wprintw(MAIN_WIN,"Hamurabi: Think again. We only have %d bushels.",bushels(cty));
                wprintw(MAIN_WIN," Now then,\n");
                return plantSeed(cty);
            case ESTARVE:
            case OKAY: break;
        }
    }
}

void printYearlyReport(const uint16_t year, city_st *const cty) {
    wprintw(MAIN_WIN,"Hamurabi: I beg to report to you,\nin year %d, ", year);
    wprintw(MAIN_WIN,"%d people starved %d came to the city.\n",starved(cty), migrated(cty));
    add_migrants(cty);
    if (RAND(2) == 1) {
        wprintw(MAIN_WIN,"A horrible plague struck! Many people have died!\n");
        plague(cty);
    }
    wprintw(MAIN_WIN,"\nPopulation is now %d\n", population(cty));
    wprintw(MAIN_WIN,"The city now owns %d acres\n", acres(cty));
    wprintw(MAIN_WIN,"You have harvested %d bushels per acre.\n",yield(cty));
    wprintw(MAIN_WIN,"Rats ate %d bushels.\n\n",pests(cty));
    wprintw(MAIN_WIN,"You now have %d bushels in store.\n\n",bushels(cty));
    wrefresh(MAIN_WIN);
    wgetch(MAIN_WIN);
}

void updateScreen(const int year, const city_st *const cty) {
    wrefresh(MAIN_WIN); 
    updateInfo(year,cty);   
    wclear(MAIN_WIN);
}

void gameLoop(void) {
    city_st *cty = new_city_st();
    for (uint16_t year=0; year < 11; year++) {
        printYearlyReport(year,cty);
        updateInfo(year,cty);
        wclear(MAIN_WIN);
        buyAcres(cty);
        updateScreen(year,cty);
        feedPeople(cty);
        updateInfo(year,cty);
        wclear(MAIN_WIN);
        plantSeed(cty);
        updateInfo(year,cty);
        wclear(MAIN_WIN);
        if (step(cty) == ESTARVE) {
            wprintw(MAIN_WIN,"You starved %d people in one year!!!\n",starved(cty));
            ejected();
        }
    }
    game_end(cty);
    free_city_st(cty);
}

int main(void) {
    initScreen();
    wprintw(MAIN_WIN,"Try your hand at governing Ancient Sumeria\nsuccessfully for a 10 year term of office.\n");
    gameLoop();
}

#include <stdio.h>
#include <ncurses.h>

#include "ProgramStates/MainMenu.h"
#include "ProgramStates/NewGameMenu.h"
#include "ProgramStates/LogInMenu.h"
#include "ProgramStates/MainGame.h"
#include "GlobalDefines.h"
#include "Globals.h"



int main(){
    initscr();
    start_color();

    init_color(COLOR_GRAY0, 150, 150, 150);

    getmaxyx(stdscr, scrH, scrW);

    initMainMenu();
    initNewGameMenu();
    initLogInMenu();
    initMainGame();
    

    engineState = &maineMenu;

    (*(engineState->enter))();

    while(!terminate){
        (*(engineState->update))();
        (*(engineState->render))();
    }

    endwin();
}
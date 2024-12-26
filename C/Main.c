#include "MainMenu.h"
#include "NewGameMenu.h"
#include <ncurses.h>
#include <stdio.h>
#include "GlobalDefines.h"
#include "Globals.h"
#include "LogInMenu.h"
#include "MainGame.h"



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
#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <wchar.h>

#include "ProgramStates/MainMenu.h"
#include "ProgramStates/NewGameMenu.h"
#include "ProgramStates/LogInMenu.h"
#include "ProgramStates/MainGame.h"
#include "ProgramStates/SignupMenu.h"
#include "UiElements/PopUp.h"
#include "GlobalDefines.h"
#include "Globals.h"



int main(){
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_color(10, 150, 150, 150);
    init_color(12, 100, 100, 100);
    init_pair(C_BG_GRAY0, COLOR_WHITE, 10);
    init_pair(C_BG_GRAY1, COLOR_WHITE, 12);
    init_pair(C_BG_BLACK, COLOR_WHITE, COLOR_BLACK);


    getmaxyx(stdscr, scrH, scrW);

    initMainMenu();
    initNewGameMenu();
    initLogInMenu();
    initMainGame();
    initSignUpMenu();
    

    engineState = &maineMenu;

    (*(engineState->enter))();

    while(!terminate){
        (*(engineState->update))();
        (*(engineState->render))();
    }

    endwin();
}
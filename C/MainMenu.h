#ifndef MAINMENU
#define MAINMENU
#include <ncurses.h>
#include <stdio.h>
#include "EngineState.h"

void initMainMenu();
void enterMainMenu();
void updateMainMenu();
void renderMainMenu();
void exitMainMenu();

extern EngineState maineMenu;



#endif
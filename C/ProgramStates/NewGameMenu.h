#ifndef NGAMEMENU
#define NGAMEMENU

#include "EngineState.h"

void initNewGameMenu();
void enterNewGameMenu();
void updateNewGameMenu(int key);
void renderNewGameMenu();
void exitNewGameMenu();

extern EngineState newGameMenu;

#endif 
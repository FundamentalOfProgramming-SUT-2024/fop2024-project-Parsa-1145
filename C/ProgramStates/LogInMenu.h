#ifndef LOGINMENU
#define LOGINMENU

#include "EngineState.h"

void initLogInMenu();
void enterLogInMenu();
void updateLogInMenu(int key);
void renderLogInMenu();
void exitLogInMenu();

void logIn();

extern EngineState logInMenu;

#endif
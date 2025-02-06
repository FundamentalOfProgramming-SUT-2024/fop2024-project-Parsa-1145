#ifndef SETTINGSMENU
#define SETTINGSMENU
#include "EngineState.h"

void initSettingsMenu();
void enterSettingsMenu();
void updateSettingsMenu(int key);
void renderSettingsMenu();
void exitSettingsMenu();

extern EngineState settingsMenu;


#endif
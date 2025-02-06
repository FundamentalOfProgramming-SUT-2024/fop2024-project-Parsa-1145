#ifndef LOADNEWGAME
#define LOADNEWGAME

#include "EngineState.h"

extern EngineState loadNewGame;

void enterNewGameLoading();
void updateNewGameLoading(int ch);
void renderNewGameLoading();
void exitNewGameLoading();

#endif
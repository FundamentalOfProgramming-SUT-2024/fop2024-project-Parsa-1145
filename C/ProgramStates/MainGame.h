#ifndef MAINGAME
#define MAINGAME

#include "EngineState.h"

void initMainGame();
void enterMainGame();
void updateMainGame();
void renderMainGame();
void exitMainGame();

extern EngineState mainGame;

void addRoom();

#endif 
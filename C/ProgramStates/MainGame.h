#ifndef MAINGAME
#define MAINGAME

#include "EngineState.h"
#include "../GameObjects/Camera.h"

void initMainGame();
void enterMainGame();
void updateMainGame();
void renderMainGame();
void exitMainGame();

extern EngineState mainGame;
extern Camera mainCamera;

#endif 
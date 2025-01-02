#ifndef MAINGAME
#define MAINGAME

#include "EngineState.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/Floor.h"

void initMainGame();
void enterMainGame();
void updateMainGame();
void renderMainGame();
void exitMainGame();

extern EngineState mainGame;
extern Camera mainCamera;
extern Player player;
extern Floor* floors;

#endif 
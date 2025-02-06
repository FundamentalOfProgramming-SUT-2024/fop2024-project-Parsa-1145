#ifndef SCOREBOARD
#define SCOREBOARD

#include "EngineState.h"

void initScoreBoard();
void enterScoreBoard();
void updateScoreBoard(int key);
void renderScoreBoard();
void exitScoreBoard();

extern EngineState scoreboard;

#endif